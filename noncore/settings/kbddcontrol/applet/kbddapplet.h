/*
               =.            This file is part of the Opie Project
             .=l.            Copyright (C) 2009 Opie Developer Team <opie-devel@lists.sourceforge.net>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU General Public
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

#ifndef KBDDAPPLET_H
#define KBDDAPPLET_H

/* Opie */
#include <qpe/taskbarappletinterface.h>

/* Qt */
#include <qwidget.h>
#include <qlist.h>
#include <qpopupmenu.h>


class KbddApplet : public QWidget {
    Q_OBJECT
public:
    KbddApplet(QWidget *parent);
    ~KbddApplet();
    static int position();
private:
    void mousePressEvent( QMouseEvent* );
    void paintEvent( QPaintEvent* );
    void updateStatus(void);
    QPixmap *m_pix;
};


#endif // KBDDAPPLET_H
