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

#include "tzselect.h"
#include "resource.h"
#include "global.h"
#include "config.h"
#include <qtoolbutton.h>
#include <qfile.h>
#include <stdlib.h>

#ifdef Q_WS_QWS
#include <qcopchannel_qws.h>
#endif

TZCombo::TZCombo( QWidget *p, const char* n )
    : QComboBox( p, n )
{
    updateZones();
    // check to see if TZ is set, if it is set the current item to that
    QString tz = getenv("TZ");
    if ( !tz.isNull() ) {
	int n = 0,
            index = 0;
        for ( QStringList::Iterator it=identifiers.begin(); 
	      it!=identifiers.end(); ++it) {
	    if ( *it == tz )
		index = n;
	    n++;
	}
	setCurrentItem(index);
    } else {
	setCurrentItem(0);
    }



    // listen on QPE/System
#if defined(Q_WS_QWS)
#if !defined(QT_NO_COP)
    QCopChannel *channel = new QCopChannel( "QPE/System", this );
    connect( channel, SIGNAL(received(const QCString&, const QByteArray&)),
	this, SLOT(handleSystemChannel(const QCString&, const QByteArray&)) );
#endif
#endif


}

TZCombo::~TZCombo()
{
}

void TZCombo::updateZones()
{
    QString cur = currentText();
    clear();
    identifiers.clear();
    int curix=0;
    QString tz = getenv("TZ");
    bool tzFound = FALSE;
    Config cfg("CityTime");
    cfg.setGroup("TimeZones");
    int i=0;
    for ( ; 1; i++ ) {
	QString zn = cfg.readEntry("Zone"+QString::number(i), QString::null);
	if ( zn.isNull() )
	    break;
	if ( zn == tz )
	    tzFound = TRUE;
	QString nm = cfg.readEntry("ZoneName"+QString::number(i));
	identifiers.append(zn);
	insertItem(nm);
	if ( nm == cur )
	    curix = i;
    }
    if ( !tzFound && !tz.isEmpty()) {
	int i =  tz.find( '/' );
	QString nm = tz.mid( i+1 );
	identifiers.append(tz);
	insertItem(nm);
	if ( nm == cur )
	    curix = i;
	++i;
    }
    for (QStringList::Iterator it=extras.begin(); it!=extras.end(); ++it) {
	insertItem(*it);
	identifiers.append(*it);
	if ( *it == cur )
	    curix = i;
	++i;
    }
    if ( !i ) {
        QStringList list = timezoneDefaults();
        for ( QStringList::Iterator it = list.begin(); it!=list.end(); ++it ) {
            identifiers.append(*it); ++it;
            insertItem(*it);
        }
    }
    setCurrentItem(curix);
}


void TZCombo::keyPressEvent( QKeyEvent *e )
{
    // ### should popup() in Qt 3.0 (it's virtual there)
//    updateZones();
    QComboBox::keyPressEvent(e);
}

void TZCombo::mousePressEvent(QMouseEvent*e)
{
    // ### should popup() in Qt 3.0 (it's virtual there)
//    updateZones();
    QComboBox::mousePressEvent(e);
}

QString TZCombo::currZone() const
{
    return identifiers[currentItem()];
}

void TZCombo::setCurrZone( const QString& id )
{
    for (int i=0; i< count(); i++) {
	if ( identifiers[i] == id ) {
	    setCurrentItem(i);
	    return;
	}
    }
    insertItem(id);
    setCurrentItem( count() - 1);
    identifiers.append(id);
    extras.append(id);
}



void TZCombo::handleSystemChannel(const QCString&msg, const QByteArray&)
{
    if ( msg == "timeZoneListChange()" ) {
	updateZones();
    }
}


TimeZoneSelector::TimeZoneSelector(QWidget* p, const char* n) :
    QHBox(p,n)
{
    // build the combobox before we do any updates...
    cmbTz = new TZCombo( this, "timezone combo" );

    cmdTz = new QToolButton( this, "timezone button" );
    QPixmap pixGlobe = Resource::loadPixmap( "citytime_icon" );
    cmdTz->setPixmap( pixGlobe );
    cmdTz->setMaximumSize( cmdTz->sizeHint() );

    // set up a connection to catch a newly selected item and throw our
    // signal
    QObject::connect( cmbTz, SIGNAL( activated( int ) ),
                      this, SLOT( slotTzActive( int ) ) );
    QObject::connect( cmdTz, SIGNAL( clicked() ),
                      this, SLOT( slotExecute() ) );
}

TimeZoneSelector::~TimeZoneSelector()
{
}


QString TimeZoneSelector::currentZone() const
{
    return cmbTz->currZone();
}

void TimeZoneSelector::setCurrentZone( const QString& id )
{
    cmbTz->setCurrZone( id );
}

void TimeZoneSelector::slotTzActive( int )
{
    emit signalNewTz( cmbTz->currZone() );
}

void TimeZoneSelector::slotExecute( void )
{
    // execute the city time application...
    Global::execute( "citytime" );
}

QStringList timezoneDefaults( void )
{
    QStringList tzs;
    // load up the list just like the file format (citytime.cpp)
    tzs.append( "America/New_York" );
    tzs.append( "New York" );
    tzs.append( "America/Los_Angeles" );
    tzs.append( "Los Angeles" );
    tzs.append( "Australia/Brisbane" );
    tzs.append( "Brisbane" );
    tzs.append( "Europe/Oslo" );
    tzs.append( "Oslo" );
    tzs.append( "Asia/Tokyo" );
    tzs.append( "Tokyo" );
    tzs.append( "Asia/Hong_Kong" );
    tzs.append( "Hong Kong" );
    return tzs;
}


