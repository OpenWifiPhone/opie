/*
                             This file is part of the Opie Project
              =.             (C) 2000-2002 Trolltech AS
            .=l.             (C) 2002-2005 The Opie Team <opie-devel@lists.sourceforge.net>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
    :`=1 )Y*s>-.--   :       the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
    :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

// I need access to some things you don't normally get access to.
#ifndef _MSC_VER
  //### revise to allow removal of translators under MSVC
#define private public
#define protected public
#endif
#include "firstuse.h"
#include "inputmethods.h"
#include "applauncher.h"
#include "serverapp.h"
#include "calibrate.h"
#include "documentlist.h"

/* OPIE */
#include <opie2/odebug.h>
#include <opie2/oresource.h>
#include <qtopia/qcopenvelope_qws.h>
#include <qtopia/config.h>
#include <qtopia/fontmanager.h>
using namespace Opie::Core;

/* QT */
#include <qfile.h>
#include <qpainter.h>
#include <qsimplerichtext.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qtimer.h>

#if defined( Q_WS_QWS )
#include <qwsdisplay_qws.h>
#include <qgfx_qws.h>
#endif

/* STD */
#include <stdlib.h>
#include <sys/types.h>
#if defined(Q_OS_LINUX) || defined(_OS_LINUX_)
#include <unistd.h>
#endif


struct settingsTable_t {
    bool enabled : 1;
    const char *app;
    const char *start;
    const char *stop;
    const char *desc;
};

static settingsTable_t settingsTable [] =
{
    { FALSE, "language", "raise()", "accept()", // No tr
	QT_TR_NOOP("Language") },
    { FALSE, "doctab", "raise()", "accept()", // No tr
	QT_TR_NOOP("DocTab") },
#ifndef Q_OS_WIN32
    { FALSE, "systemtime", "raise()", "accept()", // No tr
	QT_TR_NOOP("Time and Date") },
#endif
    { FALSE, "addressbook", "editPersonalAndClose()", "accept()", // No tr
	QT_TR_NOOP("Personal Information") },
    { FALSE, 0, 0, 0, 0 }
};


FirstUse::FirstUse(QWidget* parent, const char * name, WFlags wf) :
    QDialog( parent, name, TRUE, wf),
    transApp(0), transLib(0), needCalibrate(FALSE), currApp(-1),
    waitForExit(-1), waitingForLaunch(FALSE), needRestart(FALSE)
{
    ServerApplication::allowRestart = FALSE;
    // we force our height beyound the maximum (which we set anyway)
    QRect desk = qApp->desktop()->geometry();
    setGeometry( 0, 0, desk.width(), desk.height() );

    connect(qwsServer, SIGNAL(newChannel(const QString&)),
	this, SLOT(newQcopChannel(const QString&)));

    // Create a DocumentList so appLauncher has appLnkSet to search
    docList = new DocumentList( 0, FALSE );
    appLauncher = new AppLauncher( this );
    connect( appLauncher, SIGNAL(terminated(int,const QString&)),
	    this, SLOT(terminated(int,const QString&)) );

    // more hackery
    // I will be run as either the main server or as part of the main server
    QWSServer::setScreenSaverIntervals(0);
    loadPixmaps();

    //check if there is a language program
#ifndef Q_OS_WIN32
    QString exeSuffix;
#else
    QString exeSuffix(".exe");
#endif

    for ( int i = 0; settingsTable[i].app; i++ ) {
	QString file = OPIE_BINDIR + QString("/");
	file += settingsTable[i].app;
	file += exeSuffix;
	if ( QFile::exists(file) )
	    settingsTable[i].enabled = TRUE;
    }

    setFocusPolicy(NoFocus);

    taskBar = new QWidget(0, 0, WStyle_Tool | WStyle_Customize | WStyle_StaysOnTop | WGroupLeader);

    inputMethods = new InputMethods(taskBar);
    connect(inputMethods, SIGNAL(inputToggled(bool)),
	    this, SLOT(calcMaxWindowRect()));

    back = new QPushButton(tr("<< Back"), taskBar);
    back->setFocusPolicy(NoFocus);
    connect(back, SIGNAL(clicked()), this, SLOT(previousDialog()) );

    next = new QPushButton(tr("Next >>"), taskBar);
    next->setFocusPolicy(NoFocus);
    connect(next, SIGNAL(clicked()), this, SLOT(nextDialog()) );

    // need to set the geom to lower corner
    QSize sz = inputMethods->sizeHint();
    int buttonWidth = (width() - sz.width()) / 2;
    int x = 0;

    controlHeight = back->sizeHint().height();

    inputMethods->setGeometry(0,0, sz.width(), controlHeight );
    x += sz.width();

    back->setGeometry(x, 0, buttonWidth, controlHeight);
    x += buttonWidth;
    next->setGeometry(x, 0, buttonWidth, controlHeight);

    taskBar->setGeometry( 0, height() - controlHeight, desk.width(), controlHeight);
    taskBar->hide();

#if defined(Q_WS_QWS) && !defined(QT_NO_COP)
    odebug << "Setting up QCop to QPE/System" << oendl;
    QCopChannel* sysChannel = new QCopChannel( "QPE/System", this );
    connect(sysChannel, SIGNAL(received(const QCString&,const QByteArray&)),
	    this, SLOT(message(const QCString&,const QByteArray&)) );
#endif
    calcMaxWindowRect();

    m_calHandler = ( QWSServer::mouseHandler() && QWSServer::mouseHandler()->inherits("QCalibratedMouseHandler") ) ? true : false;

    if ( m_calHandler) {
        if ( !QFile::exists("/etc/pointercal") ) {
            needCalibrate = TRUE;
            grabMouse();
        }
    }

    Config config("locale");
    config.setGroup( "Language");
    lang = config.readEntry( "Language", "en");

    defaultFont = font();

    //###language/font hack; should look it up somewhere
#ifdef Q_WS_QWS
    if ( lang == "ja" || lang == "zh_CN" || lang == "zh_TW" || lang == "ko" ) {
	QFont fn = FontManager::unicodeFont( FontManager::Proportional );
	qApp->setFont( fn, TRUE );
    }
#endif
}

FirstUse::~FirstUse()
{
    delete appLauncher;
    delete docList;
    delete taskBar;
    ServerApplication::allowRestart = TRUE;
}

void FirstUse::calcMaxWindowRect()
{
#ifdef Q_WS_QWS
    QRect wr;
    int displayWidth  = qApp->desktop()->width();
    QRect ir = inputMethods->inputRect();
    if ( ir.isValid() ) {
	wr.setCoords( 0, 0, displayWidth-1, ir.top()-1 );
    } else {
	wr.setCoords( 0, 0, displayWidth-1,
		qApp->desktop()->height() - controlHeight-1);
    }

#if QT_VERSION < 0x030000
    QWSServer::setMaxWindowRect( qt_screen->mapToDevice(wr,
		QSize(qt_screen->width(),qt_screen->height()))
			       );
#else
    QWSServer::setMaxWindowRect( wr );
#endif
#endif
}

/* cancel current dialog, and bring up next */
void FirstUse::nextDialog()
{
    int prevApp = currApp;
    do {
	currApp++;
    odebug << "currApp = " << currApp << "" << oendl;
	if ( settingsTable[currApp].app == 0 ) {
	    if ( prevApp >= 0 && appLauncher->isRunning(settingsTable[prevApp].app) ) {
		// The last application is still running.
		// Tell it to stop, and when its done we'll come back
		// to nextDialog and exit.
        odebug << "Waiting for " << settingsTable[prevApp].app << " to exit" << oendl;
		QCopEnvelope e(QCString("QPE/Application/") + settingsTable[prevApp].app,
			settingsTable[prevApp].stop );
		currApp = prevApp;
	    } else {
        odebug << "Done!" << oendl;
		Config config( "qpe" );
		config.setGroup( "Startup" );
		config.writeEntry( "FirstUse", FALSE );
		QLabel *lblWait = new QLabel("Please Wait...", 0, "wait hack!", // No tr
			QWidget::WStyle_Customize | QWidget::WDestructiveClose |
			QWidget::WStyle_NoBorder | QWidget::WStyle_Tool |
			QWidget::WStyle_StaysOnTop);
		lblWait->setAlignment( QWidget::AlignCenter );
		lblWait->setGeometry( qApp->desktop()->geometry() );
		lblWait->show();
		qApp->processEvents();
		QTimer::singleShot( 1000, lblWait, SLOT(close()) );
		repaint();
		close();
		ServerApplication::allowRestart = TRUE;
	    }
	    return;
	}
    } while ( !settingsTable[currApp].enabled );

    if ( prevApp >= 0 && appLauncher->isRunning(settingsTable[prevApp].app) ) {
    odebug << "Shutdown: " << settingsTable[prevApp].app << "" << oendl;
	QCopEnvelope e(QCString("QPE/Application/") + settingsTable[prevApp].app,
			settingsTable[prevApp].stop );
	waitForExit = prevApp;
    } else {
    odebug << "Startup: " << settingsTable[currApp].app << "" << oendl;
	QCopEnvelope e(QCString("QPE/Application/") + settingsTable[currApp].app,
		settingsTable[currApp].start );
	waitingForLaunch = TRUE;
    }

    updateButtons();
}

/* accept current dialog and bring up previous */
void FirstUse::previousDialog()
{
    int prevApp = currApp;
    do {
	currApp--;
	if ( currApp < 0 ) {
	    currApp = prevApp;
	    return;
	}
    } while ( !settingsTable[currApp].enabled );

    if ( prevApp >= 0 ) {
    odebug << "Shutdown: " << settingsTable[prevApp].app << "" << oendl;
	QCopEnvelope e(QCString("QPE/Application/") + settingsTable[prevApp].app,
			settingsTable[prevApp].stop );
/*
	if (settingsTable[prevApp].app == QString("systemtime"))
	    QCopEnvelope e("QPE/Application/citytime", "close()");
*/
	waitForExit = prevApp;
    } else {
    odebug << "Startup: " << settingsTable[currApp].app << "" << oendl;
	QCopEnvelope e(QCString("QPE/Application/") + settingsTable[currApp].app,
		settingsTable[currApp].start );
	waitingForLaunch = TRUE;
    }

    updateButtons();
}

void FirstUse::message(const QCString &msg, const QByteArray &data)
{
    QDataStream stream( data, IO_ReadOnly );
    if ( msg == "timeChange(QString)" ) {
	QString t;
	stream >> t;
	if ( t.isNull() )
	    unsetenv("TZ");
	else
	    setenv( "TZ", t.latin1(), 1 );
    }
}

void FirstUse::terminated( int, const QString &app )
{
    odebug << "--- terminated: " << app << "" << oendl;
    if ( waitForExit != -1 && settingsTable[waitForExit].app == app ) {
    odebug << "Startup: " << settingsTable[currApp].app << "" << oendl;
	if ( settingsTable[waitForExit].app == "language" ) { // No tr
	    Config config("locale");
	    config.setGroup( "Language");
	    QString l = config.readEntry( "Language", "en");
	    if ( l != lang ) {
		reloadLanguages();
		needRestart = TRUE;
		lang = l;
	    }
	}
	QCopEnvelope e(QCString("QPE/Application/") + settingsTable[currApp].app,
		settingsTable[currApp].start );
	waitingForLaunch = TRUE;
	updateButtons();
	repaint();
	waitForExit = -1;
    } else if ( settingsTable[currApp].app == app ) {
	nextDialog();
    } else {
	back->setEnabled(TRUE);
	next->setEnabled(TRUE);
    }
}

void FirstUse::newQcopChannel(const QString& channelName)
{
    odebug << "channel " << channelName.data() << " added" << oendl;
    QString prefix("QPE/Application/");
    if (channelName.startsWith(prefix)) {
	QString appName = channelName.mid(prefix.length());
	if ( currApp >= 0 && appName == settingsTable[currApp].app ) {
        odebug << "Application: " << settingsTable[currApp].app << " started" << oendl;
	    waitingForLaunch = FALSE;
	    updateButtons();
	    repaint();
	} else if (appName != "quicklauncher") {
	    back->setEnabled(FALSE);
	    next->setEnabled(FALSE);
	}
    }
}

void FirstUse::reloadLanguages()
{
    // read language from config file.  Waiting on QCop takes too long.
    Config config("locale");
    config.setGroup( "Language");
    QString l = config.readEntry( "Language", "en");
    QString cl = getenv("LANG");
    owarn << "language message - " + l << oendl;
    // setting anyway...
    if (l.isNull() )
	unsetenv( "LANG" );
    else {
    owarn << "and its not null" << oendl;
	setenv( "LANG", l.latin1(), 1 );
    }
#ifndef QT_NO_TRANSLATION
    // clear old translators
#ifndef _MSC_VER
  //### revise to allow removal of translators under MSVC
  if(qApp->translators) {
        qApp->translators->setAutoDelete(TRUE);
        delete (qApp->translators);
        qApp->translators = 0;
    }
#endif

    // load translation tables
    transApp = new QTranslator(qApp);
    QString tfn = QPEApplication::qpeDir() + "i18n/"+l+"/qpe.qm";
    owarn << "loading " + tfn << oendl;
    if ( transApp->load(tfn) ) {
    owarn << "installing translator" << oendl;
	qApp->installTranslator( transApp );
    } else  {
	delete transApp;
	transApp = 0;
    }

    transLib = new QTranslator(qApp);
    tfn = QPEApplication::qpeDir() + "i18n/"+l+"/libqpe.qm";
    owarn << "loading " + tfn << oendl;
    if ( transLib->load(tfn) ) {
    owarn << "installing translator library" << oendl;
	qApp->installTranslator( transLib );
    } else  {
	delete transLib;
	transLib = 0;
    }
    loadPixmaps();
    //###language/font hack; should look it up somewhere
#ifdef Q_WS_QWS
    if ( l == "ja" || l == "zh_CN" || l == "zh_TW" || l == "ko" ) {
	QFont fn = FontManager::unicodeFont( FontManager::Proportional );
	qApp->setFont( fn, TRUE );
    } else {
	qApp->setFont( defaultFont, TRUE );
    }
#endif
#endif
}

void FirstUse::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    p.drawPixmap(0,0, splash);

    if ( currApp < 0 ) {
	drawText(p, tr( "Tap anywhere on the screen to continue." ));
    } else if ( settingsTable[currApp].app ) {
	if ( waitingForLaunch )
	    drawText(p, tr("Please wait, loading %1 settings.").arg(tr(settingsTable[currApp].desc)) );
    } else {
	drawText(p, tr("Please wait..."));
    }
}

void FirstUse::loadPixmaps()
{
    splash.convertFromImage( OResource::loadImage("launcher/firstuse", OResource::NoScale )
                             .smoothScale( width(), height() ) );

    setBackgroundPixmap(splash);
}

void FirstUse::drawText(QPainter &p, const QString &text)
{
    QString altered = "<CENTER>" + text + "</CENTER>";

    QSimpleRichText rt(altered, p.font());
    rt.setWidth(width() - 20);

    int h = (height() * 3) / 10; // start at 30%
    if (rt.height() < height() / 2)
	h += ((height() / 2) - rt.height()) / 2;
    rt.draw(&p, 10, h, QRegion(0,0, width()-20, height()), palette());
}

void FirstUse::updateButtons()
{
    if ( currApp >= 0 ) {
	taskBar->show();
    }

    int i = currApp-1;
    while ( i >= 0 && !settingsTable[i].enabled )
	i--;
    back->setText(tr("<< Back"));
    back->setEnabled( i >= 0 && !waitingForLaunch );

    i = currApp+1;
    while ( settingsTable[i].app && !settingsTable[i].enabled )
	i++;
    if ( !settingsTable[i].app )
	next->setText(tr("Finish"));
    else
	next->setText(tr("Next >>"));

    next->setEnabled( !waitingForLaunch );
}

void FirstUse::keyPressEvent( QKeyEvent *e )
{
    // Allow cancelling at first dialog, in case display is broken.
    if ( e->key() == Key_Escape && currApp < 0 )
	QDialog::keyPressEvent(e);
}

void FirstUse::mouseReleaseEvent( QMouseEvent * )
{
    if ( currApp < 0 ) {
	if ( m_calHandler && needCalibrate ) {
	    releaseMouse();
	    Calibrate *cal = new Calibrate;
	    cal->exec();
	    delete cal;
	}
	nextDialog();
    }
}
