/*
                             This file is part of the Opie Project
              =.             (C) 2006 Michael 'Mickey' Lauer <mickey@Vanille.de>
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

#ifndef ODEVICE_MOTOROLA_EZX
#define ODEVICE_MOTOROLA_EZX

#include "odevice_abstractmobiledevice.h"

/* QT */
#include <qfile.h>
#include <qwindowsystem_qws.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

// Audio
// Power Management
#define APM_IOCGEVTSRC          OD_IOR( 'A', 203, int )
#define APM_IOCSEVTSRC          OD_IORW( 'A', 204, int )
#define APM_EVT_POWER_BUTTON    (1 << 0)

// Vesa Standard
#define FB_BLANK_UNBLANK            0
#define FB_BLANK_POWERDOWN          4

namespace Opie {
namespace Core {
namespace Internal {

class Motorola_EZX : public OAbstractMobileDevice
{
  Q_OBJECT

  protected:
    virtual void init(const QString&);
    //void initHingeSensor();

  protected slots:
    //void hingeSensorTriggered();
    void systemMessage( const QCString &msg, const QByteArray & );

  public:
    virtual bool setDisplayStatus( bool on );

  protected:
    QFile m_hinge;
};

}
}
}
#endif
