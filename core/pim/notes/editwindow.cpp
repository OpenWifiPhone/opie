/*
                             This file is part of the Opie Project
                             
              =.             Copyright (C) 2009 The Opie Team <opie-devel@lists.sourceforge.net>
            .=l.             Portions copyright (C) 2002 Henning Holtschneider <hh@holtschneider.com>
           .>+-=
 _;:,     .>    :=|.         This program is free software; you can
.> <`_,   >  .   <=          redistribute it and/or modify it under
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
..}^=.=       =       ;      General Public License for more
++=   -.     .`     .:       details.
 :     =  ...= . :.=-
 -.   .:....=;==+<;          You should have received a copy of the GNU
  -_. . .   )=.  =           General Public License along with
    --        :-=`           this file; see the file COPYING.
                             If not, write to the Free Software Foundation,
                             Inc., 59 Temple Place - Suite 330,
                             Boston, MA 02111-1307, USA.
*/

#include "editwindow.h"

#include "qwhatsthis.h"

using namespace Opie;
using namespace Opie::Notes;

editWindowWidget::editWindowWidget( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl ), m_changed( false )
{
    QGridLayout *gridLayout = new QGridLayout(this, 2, 1, 5, 5);
    m_editArea = new QMultiLineEdit(this, "m_editArea");
    gridLayout->addWidget(m_editArea, 0, 0);
    m_editArea->setWordWrap(QMultiLineEdit::WidgetWidth);
    connect( m_editArea, SIGNAL( textChanged() ), this, SLOT( slotChanged() ) );
    
    // Category
//    label = new QLabel( tr( "Category:" ), container );
//    layout->addWidget( label, 3, 0 );
//    QWhatsThis::add( label, tr( "Select a category to organize this note with." ) );
    m_catSelect = new CategorySelect( this );
    gridLayout->addWidget( m_catSelect, 1, 0 );
    QWhatsThis::add( m_catSelect, tr( "Select a category to organize this note with." ) );
    connect( m_catSelect, SIGNAL( sigCatChanged() ), this, SLOT( slotChanged() ) );

    showMaximized();
}

editWindowWidget::~editWindowWidget()
{
}

void editWindowWidget::readMemo( const OPimMemo &memo )
{
    m_editArea->setText( memo.text() );
    m_catSelect->setCategories( memo.categories(), "Notes", tr( "Notes" ) );

    m_changed = false;
}

void editWindowWidget::writeMemo( OPimMemo &memo )
{
    memo.setText( m_editArea->text() );
    
    QArray<int> arr = m_catSelect->currentCategories();
    memo.setCategories( arr );

    m_changed = false;
}

void editWindowWidget::slotChanged()
{
    m_changed = true;
}

void editWindowWidget::closeEvent( QCloseEvent *e )
{
    if ( !m_editArea->text().isEmpty() && m_changed ) {
        switch (QMessageBox::warning(0, tr("Discard changes?"), tr("<p>Do you really wish to discard changes to this note?</p>"),
                   tr("Discard"), tr("&Cancel") ) )
        {
            case 0:
                e->accept();
                break;
            case 1:
                e->ignore();
                break;
        }
    }
    else
        e->accept();
}
