/**********************************************************************
** Copyright (C) 2002 Michael 'Mickey' Lauer.  All rights reserved.
**
** This file is part of Opie Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**********************************************************************/

#include "wellenreiter.h"

#ifdef QWS
#include <qpe/qpeapplication.h>
#else
#include <qapplication.h>
#endif

int main( int argc, char **argv )
{
#ifdef QWS
    QPEApplication a( argc, argv );
#else
    QApplication a( argc, argv );
#endif

    Wellenreiter e;

    e.setCaption( Wellenreiter::tr("Wellenreiter") );
#ifdef QWS
    a.showMainWidget(&e);
#else
    a.setMainWidget(&e);
    e.show();
#endif

    return a.exec();
}
