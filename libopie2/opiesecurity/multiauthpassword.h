/**
 * \file multiauthpassword.h
 * \brief Password Dialog dropin.
 * \author Cl�ment S�veillac (clement . seveillac (at) via . ecp . fr)
 */
/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2004 Opie Developer Team <opie-devel@lists.sourceforge.net>
           .>+-=
 _;:,     .>    :=|.         This library is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This library is distributed in the hope that
     +  .  -:.       =       it will be useful,  but WITHOUT ANY WARRANTY;
    : ..    .:,     . . .    without even the implied warranty of
    =_        +     =;=|`    MERCHANTABILITY or FITNESS FOR A
  _.=:.       :    :=>`:     PARTICULAR PURPOSE. See the GNU
..}^=.=       =       ;      Library General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           Library General Public License along with
    --        :-=`           this library; see the file COPYING.LIB.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.

*/


#ifndef OPIE_SEC_MULTIAUTHPASSWORD_H
#define OPIE_SEC_MULTIAUTHPASSWORD_H

namespace Opie {
namespace Security {

enum lockMode {
    IfPowerOn,
    IfResume,
    TestNow,
    LockNow };
/**
 * This is the dropin replacement for the libqpe Password class.
 * If you call authenticate() a widget will cover the whole screen
 * and only return if the user is able to authenticate with any of the
 * configured Authentication Plugins.
 * It uses the Opie::Security::MultiauthMainWindow QDialog internally.
 *
 * @author Clement S�veillac, Holger Freyther
 */
class MultiauthPassword {

public:
    static void authenticate(int authMode = LockNow);
    static bool needToAuthenticate( bool atpoweron = false );
    static bool isAuthenticating();
};


}
}
#endif
