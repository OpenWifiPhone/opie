/*
               =.            This file is part of the OPIE Project
             .=l.            Copyright (c)  2002 Maximilian Reiss <max.reiss@gmx.de>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <,   >  .   <=           redistribute it and/or  modify it under
:=1 )Y*s>-.--   :            the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; version 2 of the License.
     ._= =}       :
    .%+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|     MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>:      PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .     .:        details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=            this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/
/*
 * The Bluetooth OBEX manipulating class implementation
 */

#include "btobex.h"
#include <openobex/obex_const.h>


/* OPIE */
#include <opie2/oprocess.h>
#include <opie2/odebug.h>
#include <opie2/obluetooth.h>

/* QT */
#include <qfileinfo.h>
#include <qstring.h>
#include <qmap.h>
#include <qmessagebox.h>

using namespace  OpieObex;

using namespace Opie::Core;
using namespace Opie::Bluez;
/* TRANSLATOR OpieObex::Obex */
using namespace OpieTooth;

BtObex::BtObex( QObject *parent, const char* name )
  : ObexBase(parent, name )
{
    m_rec = 0;
    m_send = 0;
}

BtObex::~BtObex() {
    delete m_rec;
    delete m_send;
}

void BtObex::receive()
{
    ObexBase::receive();
    m_rec = new ObexServer(OBEX_TRANS_BLUETOOTH);

    odebug << "BT OBEX do receive" << oendl;
    // TODO mbhaynie: No idea if this actually works -- maybe opd is better.
    // connect to the necessary slots
    connect(m_rec,  SIGNAL(processExited(Opie::Core::OProcess*) ),
            this,  SLOT(slotExited(Opie::Core::OProcess*) ) );

    connect(m_rec,  SIGNAL(receivedStdout(Opie::Core::OProcess*, char*,  int ) ),
            this,  SLOT(slotStdOut(Opie::Core::OProcess*, char*, int) ) );

    if(!m_rec->start(OProcess::NotifyOnExit, OProcess::AllOutput) ) {
        emit done( false );
        delete m_rec;
        m_rec = 0;
    }
}

void BtObex::send( const QString& fileName, const QString& bdaddr)
{
    ObexBase::send(fileName, bdaddr);
    // if currently receiving stop it send receive
    if (m_send != 0) {
        if (m_send->isSending())
            return;
        else {
            delete m_send;
            m_send = 0;
        }
    }
    if (m_rec != 0 ) {
        if (m_rec->isRunning() ) {
            emit error(-1 );
            delete m_rec;
            m_rec = 0;

        }
        else {
            emit error( -1 ); // we did not delete yet but it's not running slotExited is pending
            return;
        }
    }
    // Now we need to find out if the OBEX push is supported for this device
    // And get the port number
    OBluetoothInterface *intf = OBluetooth::instance()->defaultInterface();
    if( intf ) {
        // Just laziness in case the interface is the same one as last time (which it probably will be)
        disconnect( intf, SIGNAL( deviceFound( OBluetoothDevice *, bool ) ), this, SLOT( slotFoundDevice( OBluetoothDevice *, bool ) ) );
        connect( intf, SIGNAL( deviceFound( OBluetoothDevice *, bool ) ), this, SLOT( slotFoundDevice( OBluetoothDevice *, bool ) ) );
        disconnect( intf, SIGNAL( deviceServicesFound( OBluetoothDevice * ) ), this, SLOT( slotFoundServices( OBluetoothDevice * ) ) );
        connect( intf, SIGNAL( deviceServicesFound( OBluetoothDevice * ) ), this, SLOT( slotFoundServices( OBluetoothDevice * ) ) );
        intf->findDeviceCreate( bdaddr );
    }
}

/**
 * This function reacts on the service discovery finish
 */
void BtObex::slotFoundDevice( OBluetoothDevice *dev, bool )
{
    dev->discoverServices();
}

/**
 * This function reacts on the service discovery finish
 */
void BtObex::slotFoundServices( OBluetoothDevice *dev )
{
    odebug << "BtObex slotFoundServices" << oendl;
    OBluetoothServices::ValueList servicesList = dev->services();
    if (!servicesList.isEmpty() ) {
        int portNum = -1; //The desired port number
        QMap<int, QString> list;
        QMap<int, QString>::Iterator classIt;
        OBluetoothServices::ValueList::Iterator it2;
        for( it2 = servicesList.begin(); it2 != servicesList.end(); ++it2 ) {
            list = (*it2).classIdList();
            classIt = list.begin();
            if ( classIt != list.end() ) {
                if (classIt.key() == 4357) { // FIXME constant
                    portNum = (*it2).protocolDescriptorList().last().port();
                    break;
                }
            }
        }
        if( portNum == -1 ) {
            QMessageBox::critical(NULL, tr("Object send"),
                tr("Unable to send - no OBEX Push service on remote device"));
            emit error(-1);
        }
        else {
            m_port = portNum;
            sendNow();
        }
    }
    else {
        QMessageBox::critical(NULL, tr("Object send"), tr("Unable to send - no Bluetooth services found on remote device"));
        emit error(-1);
    }
}

void BtObex::sendNow()
{
    QString m_dst = "";
    int result; //function call result
    if ( m_count >= 25 ) { // could not send
        emit error(-1 );
        emit sent(false);
        return;
    }
    // OProcess inititialisation
    m_send = new ObexPush();
    // connect to slots Exited and and StdOut
    connect(m_send,  SIGNAL(sendComplete(int)),
            this, SLOT(slotPushComplete(int)) );
    connect(m_send,  SIGNAL(sendError(int)),
            this, SLOT(slotPushError(int)) );
    connect(m_send,  SIGNAL(status(QCString&)),
            this, SLOT(slotPushStatus(QCString&) ) );

    /*
     * FIXME: this delay is made because some cell phones understands an error
     * later.
     */
    ::sleep(4);
    // now start it
    result = m_send->send(m_bdaddr, m_port, m_file, m_dst);
    if (result > 0) //Sending process is actually running
        return;
    else if (result < 0) {
        m_count = 25;
        emit error(-1 );
        delete m_send;
        m_send=0;
    }
    // end
    m_count++;
    emit currentTry( m_count );
}

void BtObex::slotExited(OProcess* proc )
{
    odebug << proc->name() << " exited with result "
           << proc->exitStatus() << oendl;
    if (proc == m_rec )  // receive process
        received();

}

void BtObex::slotStdOut(OProcess* proc, char* buf, int len)
{
    if ( proc == m_rec ) { // only receive
        QByteArray ar( len  );
        memcpy( ar.data(), buf, len );
        m_outp.append( ar );
        QCString str(buf, len);
        odebug << str << oendl;
    }
}

void BtObex::slotPushComplete(int result)
{
    if (result == 0) {
        delete m_send;
        m_send=0;
        emit sent(true);
    }
    else if( result == OBEX_RSP_FORBIDDEN ) {
        delete m_send;
        m_send = 0;
        emit error( result );
    }
    else { // it failed maybe the other side wasn't ready
        // let's try it again
        delete m_send;
        m_send = 0;
        sendNow();
    }
}

void BtObex::slotPushError(int)
{
    delete m_send;
    m_send = 0;
    emit error( -1 );
}

void BtObex::slotPushStatus(QCString& str)
{
    odebug << str << oendl;
}

void BtObex::received()
{
    if (m_rec->normalExit() ) {
        if ( m_rec->exitStatus() == 0 ) { // we got one
            QString filename = parseOut();
            odebug << "OBEX " << filename << " received" << oendl;
            emit receivedFile( filename );
        }
    }
    else {
        emit done(false);
    }
    delete m_rec;
    m_rec = 0;
    receive();
}

// This probably doesn't do anything useful for bt.
QString BtObex::parseOut()
{
    QString path;
    QStringList list = QStringList::split("\n",  m_outp);
    QStringList::Iterator it;
    for (it = list.begin(); it != list.end(); ++it ) {
        odebug << (*it) << oendl;
        if ( (*it).startsWith("Wrote"  ) ) {
            int pos = (*it).findRev('(' );
            if ( pos > 0 ) {

                path = (*it).remove( pos, (*it).length() - pos );
                path = path.mid(6 );
                path = path.stripWhiteSpace();
            }
        }
    }
    return path;
}

/**
 * when sent is done slotError is called we  will start receive again
 */
void BtObex::slotError()
{
    ObexBase::slotError();
    if ( m_receive )
        receive();
};

void BtObex::setReceiveEnabled( bool receive )
{
    odebug << "BT OBEX setReceiveEnabled " << receive << oendl;
    ObexBase::setReceiveEnabled(receive);
    if ( !receive ) { //
        m_receive = false;
        shutDownReceive();
    }
}

void BtObex::shutDownReceive()
{
    if (m_rec != 0 ) {
        if (m_rec->isRunning() ) {
            emit error(-1 );
            delete m_rec;
            m_rec = 0;
        }
    }
}
