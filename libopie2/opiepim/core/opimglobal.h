/*
                             This file is part of the Opie Project
                             Copyright (C) Stefan Eilers <eilers.stefan@epost.de>
              =.             Copyright (C) The Opie Team <opie-devel@lists.sourceforge.net>
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
    .%`+i>       _;_.
    .i_,=:_.      -<s.       This program is distributed in the hope that
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
#ifndef __OPIMGLOBAL_H_
#define __OPIMGLOBAL_H_

#include <qarray.h>

namespace Opie{

/**
 * The unique identifier for every
 * PIM record. For now it is a negative
 * int but we could change it to long or QUuid
 * in the future
 */
typedef int UID;
typedef QArray<UID> UIDArray;

namespace Pim{


	/**
	 * Contains global types and enums for the PIM-API
	 */
class OPimGlobal{
 public:
	enum PimType {
		TODOLIST,
		CONTACTLIST,
		DATEBOOK,
        NOTES,
		_END_PimType
	};

	enum DatabaseStyle {
		DEFAULT,      // Use default Database
		UNKNOWN,      // Unknown database style
		XML,
		SQL,
		VCARD,        // Also used for VCAL !
        TEXT,         // Plain text (memos only)
		_END_DatabaseStyle
	};


};

}
}


#endif
