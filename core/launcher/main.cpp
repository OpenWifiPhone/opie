/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/

#include "desktop.h"
#include "taskbar.h"
#include "stabmon.h"

#include <qpe/qpeapplication.h>
#include <qpe/network.h>
#include <qpe/config.h>
#if defined( QT_QWS_SL5XXX ) || defined( QT_QWS_IPAQ )
#include <qpe/custom.h>
#endif

#include <opie/odevice.h>

#include <qmessagebox.h>
#include <qfile.h>
#include <qimage.h>
#include <qwindowsystem_qws.h>
#include <qwsmouse_qws.h>
#include <qpe/qcopenvelope_qws.h>
#include <qpe/alarmserver.h>

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include "../calibrate/calibrate.h"

using namespace Opie;

void initEnvironment()
{
    int rot;
    Config config("locale");

    config.setGroup( "Location" );
    QString tz = config.readEntry( "Timezone", getenv("TZ") );

  // if not timezone set, pick New York
    if (tz.isNull())
        tz = "America/New_York";

    setenv( "TZ", tz, 1 );
    config.writeEntry( "Timezone", tz);

    config.setGroup( "Language" );
    QString lang = config.readEntry( "Language", getenv("LANG") );
    if ( !lang.isNull() )
        setenv( "LANG", lang, 1 );

#if !defined(QT_QWS_CASSIOPEIA) && !defined(QT_QWS_IPAQ) && !defined(QT_QWS_SL5XXX)
    setenv( "QWS_SIZE", "240x320", 0 );
#endif

    QString env(getenv("QWS_DISPLAY"));
    if (env.contains("Transformed")) {
        // transformed driver default rotation is controlled by the hardware.
        Config config("qpe");
        config.setGroup( "Rotation" );
        if ( ( rot = config.readNumEntry( "Rot", -1 ) ) == -1 )
            rot = ODevice::inst ( )-> rotation ( ) * 90;

        setenv("QWS_DISPLAY", QString("Transformed:Rot%1:0").arg(rot), 1);
        QPEApplication::defaultRotation ( ); /* to ensure deforient matches reality */
    }
}


int initApplication( int argc, char ** argv )
{
    initEnvironment();

    //Don't flicker at startup:
    QWSServer::setDesktopBackground( QImage() );

    DesktopApplication a( argc, argv, QApplication::GuiServer );

    ODevice::inst ( )-> setSoftSuspend ( true );

    { // init backlight
        QCopEnvelope e("QPE/System", "setBacklight(int)" );
  	e << -3; // Forced on
    }

    AlarmServer::initialize();

    Desktop *d = new Desktop();

    QObject::connect( &a, SIGNAL(power()),   d, SLOT(togglePower()) );
    QObject::connect( &a, SIGNAL(backlight()),   d, SLOT(toggleLight()) );
    QObject::connect( &a, SIGNAL(symbol()),   d, SLOT(toggleSymbolInput()) );
    QObject::connect( &a, SIGNAL(numLockStateToggle()),   d, SLOT(toggleNumLockState()) );
    QObject::connect( &a, SIGNAL(capsLockStateToggle()),   d, SLOT(toggleCapsLockState()) );
    QObject::connect( &a, SIGNAL(prepareForRestart()),   d, SLOT(terminateServers()) );

    (void)new SysFileMonitor(d);
    Network::createServer(d);

    if ( QWSServer::mouseHandler() ->inherits("QCalibratedMouseHandler") ) {
	if ( !QFile::exists( "/etc/pointercal" ) ) {
	    // Make sure calibration widget starts on top.
	    Calibrate *cal = new Calibrate;
	    cal->exec();
	    delete cal;
	}
    }

    d->show();

    int rv =  a.exec();

    delete d;

	ODevice::inst ( )-> setSoftSuspend ( false );

    return rv;
}

static const char *pidfile_path = "/var/run/opie.pid";

void create_pidfile ( )
{
	FILE *f;

	if (( f = ::fopen ( pidfile_path, "w" ))) {
		::fprintf ( f, "%d", getpid ( ));
		::fclose ( f );
	}
}

void remove_pidfile ( )
{
	::unlink ( pidfile_path );
}

void handle_sigterm ( int /* sig */ )
{
	if ( qApp )
		qApp-> quit ( );
}

int main( int argc, char ** argv )
{
    ::signal ( SIGCHLD, SIG_IGN );

	::signal ( SIGTERM, handle_sigterm );
	::signal ( SIGINT, handle_sigterm );

	::setsid ( );
	::setpgid ( 0, 0 );

	::atexit ( remove_pidfile );
	create_pidfile ( );

    int retVal = initApplication ( argc, argv );

    // Kill them. Kill them all.
    ::kill ( 0, SIGTERM );
    ::sleep ( 1 );
    ::kill ( 0, SIGKILL );

    return retVal;
}

