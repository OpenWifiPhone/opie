/*
                             This file is part of the Opie Project

                             Copyright (C) 2003 Michael 'Mickey' Lauer
                             <mickey@tm.informatik.uni-frankfurt.de>
              =.
            .=l.
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or  modify it under
:`=1 )Y*s>-.--   :           the terms of the GNU Library General Public
.="- .-=="i,     .._         License as published by the Free Software
 - .   .-<_>     .<>         Foundation; either version 2 of the License,
     ._= =}       :          or (at your option) any later version.
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

#include "oversatileviewdemo.h"
#include <opie2/oversatileview.h>
#include <opie2/oversatileviewitem.h>

#include <qstring.h>
#include <qpixmap.h>
#include <qlistview.h>

using namespace Opie::Ui;

OVersatileViewDemo::OVersatileViewDemo( QWidget* parent, const char* name, WFlags f )
                   :QVBox( parent, name, f )
{
    vv = new OVersatileView( this );

    vv->addColumn( "First" );
    vv->addColumn( "2nd" );
    vv->addColumn( "IIIrd" );

    QString counter;

    QPixmap leaf( "leaf.png" );
    QPixmap opened( "folder_opened.png" );
    QPixmap closed( "folder_closed.png" );

    QPixmap leaf32( "leaf32.png" );
    QPixmap opened32( "folder_opened32.png" );
    QPixmap closed32( "folder_closed32.png" );

    vv->setDefaultPixmaps( OVersatileView::Tree, leaf, opened, closed );
    vv->setDefaultPixmaps( OVersatileView::Icons, leaf32, opened32, closed32 );

    OVersatileViewItem* item;
    OVersatileViewItem* item2;

    for ( int i = 0; i < 5; ++i )
    {
        counter.sprintf( "%d", i );
        item = new OVersatileViewItem( vv, "Item", "Text", "Some more", counter );
        item->setRenameEnabled( true );
        item2 = new OVersatileViewItem( item, "OSubitem", "123", "...", counter );
        item2->setRenameEnabled( true );

    }

    connect( vv, SIGNAL( selectionChanged() ), this, SLOT( selectionChanged() ) );
    connect( vv, SIGNAL( selectionChanged(OVersatileViewItem*) ), this, SLOT( selectionChanged(OVersatileViewItem*) ) );
    connect( vv, SIGNAL( currentChanged(OVersatileViewItem*) ), this, SLOT( currentChanged(OVersatileViewItem*) ) );
    connect( vv, SIGNAL( clicked(OVersatileViewItem*) ), this, SLOT( clicked(OVersatileViewItem*) ) );
    connect( vv, SIGNAL( pressed(OVersatileViewItem*) ), this, SLOT( pressed(OVersatileViewItem*) ) );

    connect( vv, SIGNAL( doubleClicked(OVersatileViewItem*) ), this, SLOT( doubleClicked(OVersatileViewItem*) ) );
    connect( vv, SIGNAL( returnPressed(OVersatileViewItem*) ), this, SLOT( returnPressed(OVersatileViewItem*) ) );

    connect( vv, SIGNAL( onItem(OVersatileViewItem*) ), this, SLOT( onItem(OVersatileViewItem*) ) );
    connect( vv, SIGNAL( onViewport() ), this, SLOT( onViewport() ) );

    connect( vv, SIGNAL( expanded(OVersatileViewItem*) ), this, SLOT( expanded(OVersatileViewItem*) ) );
    connect( vv, SIGNAL( collapsed(OVersatileViewItem*) ), this, SLOT( collapsed(OVersatileViewItem*) ) );

    connect( vv, SIGNAL( moved() ), this, SLOT( moved() ) );

    connect( vv, SIGNAL( contextMenuRequested(OVersatileViewItem*,const QPoint&,int) ), this, SLOT( contextMenuRequested(OVersatileViewItem*,const QPoint&,int) ) );

}

OVersatileViewDemo::~OVersatileViewDemo()
{
}

void OVersatileViewDemo::selectionChanged()
{
    odebug << "received signal selectionChanged()" << oendl;
}
void OVersatileViewDemo::selectionChanged( OVersatileViewItem * item )
{
    odebug << "received signal selectionChanged(OVersatileViewItem*)" << oendl;
}
void OVersatileViewDemo::currentChanged( OVersatileViewItem * item )
{
    odebug << "received signal currentChanged( OVersatileViewItem * )" << oendl;
}
void OVersatileViewDemo::clicked( OVersatileViewItem * item )
{
    odebug << "received signal clicked( OVersatileViewItem * )" << oendl;
}
void OVersatileViewDemo::pressed( OVersatileViewItem * item )
{
    odebug << "received signal pressed( OVersatileViewItem * )" << oendl;
}

void OVersatileViewDemo::doubleClicked( OVersatileViewItem *item )
{
    odebug << "received signal doubleClicked( OVersatileViewItem *item )" << oendl;
}
void OVersatileViewDemo::returnPressed( OVersatileViewItem *item )
{
    odebug << "received signal returnPressed( OVersatileViewItem *item )" << oendl;
}

void OVersatileViewDemo::onItem( OVersatileViewItem *item )
{
    odebug << "received signal onItem( OVersatileViewItem *item )" << oendl;
}
void OVersatileViewDemo::onViewport()
{
    odebug << "received signal onViewport()" << oendl;
}

void OVersatileViewDemo::expanded( OVersatileViewItem *item )
{
    odebug << "received signal expanded( OVersatileViewItem *item )" << oendl;
}

void OVersatileViewDemo::collapsed( OVersatileViewItem *item )
{
    odebug << "received signal collapsed( OVersatileViewItem *item )" << oendl;
}

void OVersatileViewDemo::moved()
{
    odebug << "received signal moved( OVersatileViewItem *item )" << oendl;
}

void OVersatileViewDemo::contextMenuRequested( OVersatileViewItem *item, const QPoint& pos, int col )
{
    odebug << "received signal contextMenuRequested( OVersatileViewItem *item )" << oendl;
}
