/*
                             This file is part of the Opie Project
                             Copyright (C) 2003-2005 by Michael 'Mickey' Lauer <mickey@Vanille.de>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
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

#ifndef OSTATION_H
#define OSTATION_H

#include <opie2/onetutils.h>

#include <qlist.h>
#include <qstring.h>
#include <qhostaddress.h>
#include <qobject.h>
#include <qvaluelist.h>

#include <sys/types.h>

namespace Opie {
namespace Net  {

class OStation;


typedef QList<OStation> OStationList;

/*======================================================================================
 * OStation
 *======================================================================================*/

class OStation
{
  public:
    OStation();
    ~OStation();

    void dump();

    /* Ethernet */
    QString type;
    OMacAddress macAddress;
    QHostAddress ipAddress;

    /* WaveLan */
    QString ssid;
    OMacAddress apAddress;
    int channel;
    int level;
    bool encrypted;
    QValueList <int> rates;
  private:
    class Private;
    Private *d;
};

}
}

#endif // OSTATION_H

