/*
                             This file is part of the Opie Project

                             Copyright (C) Opie Team <opie-devel@lists.sourceforge.net>
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

#include <stdlib.h>
#include <cmath>
#include <cctype>

#include <opie2/odebug.h>
#include <opie2/opimrecurrence.h>
#include <opie2/oresource.h>

#include <qpe/config.h>
#include <qpe/qpeapplication.h>

#include <qcombobox.h>
#include <qlineedit.h>
#include <qpopupmenu.h>

#include "mainwindow.h"
//#include "tableitems.h"
#include "tableview.h"

using namespace Todo;

namespace {
    static const int BoxSize = 14;
    static const int RowHeight = 20;
}

TableView::EditorWidget::EditorWidget() : m_wid(0l), m_row(-1), m_col(-1)
{
}

void TableView::EditorWidget::setCellWidget(QWidget* wid, int row, int col )
{
    m_wid = wid;
    m_row = row;
    m_col = col;
}

void TableView::EditorWidget::releaseCellWidget()
{
    m_wid = 0;
    m_row = m_col = -1;
}

QWidget* TableView::EditorWidget::cellWidget() const
{
    return m_wid;
}

int TableView::EditorWidget::cellRow() const
{
    return m_row;
}

int TableView::EditorWidget::cellCol() const
{
    return m_col;
}


void TableView::initConfig()
{
    Config config( "todo" );
    config.setGroup( "Options" );
    m_completeStrokeWidth = config.readNumEntry( "CompleteStrokeWidth", 8 );
    for ( int i = 0; i < numCols(); i++ ) {
        int width = config.readNumEntry("Width"+QString::number(i), -1 );
        setColumnWidth(i, width == -1 ? columnWidth(i) : width );
    }
    // We need to use columnClicked so that the QTable knows we're sorting by the specified column
    // This will call sortColumn() hence we need to have m_initialising set during init or we will get a segfault
    int sortColumn = config.readNumEntry( "SortColumn", 0 );
    columnClicked(sortColumn);
    setAscending( config.readBoolEntry( "SortAscending", true ) );
}

TableView::TableView( MainWindow* window, QWidget* wid, const QValueList<QPixmap> &pic_priority )
    : QTable(  wid ), TodoView( window ), m_pic_priority( pic_priority )
{
    setName("TableView");
    m_initialising = true;

    // Load icons
    m_pic_completed = Opie::Core::OResource::loadPixmap( "todo/completed" );

    setUpdatesEnabled( false );
    viewport()->setUpdatesEnabled( false );
    m_enablePaint = false;
    setNumRows(0);
    setNumCols(4);

    horizontalHeader()->setLabel( 0, QWidget::tr("C.") );
    horizontalHeader()->setLabel( 1, QWidget::tr("Priority") );
    horizontalHeader()->setLabel( 2, QWidget::tr("Summary" ) );
    horizontalHeader()->setLabel( 3, QWidget::tr("Deadline") );

    setShowDeadline( todoWindow()->showDeadline() );

    setSorting( TRUE );
    setSelectionMode( NoSelection );

    setLeftMargin( 0 );
    verticalHeader()->hide();

    QPEApplication::setStylusOperation( viewport(), QPEApplication::RightOnHold );
    connect((QTable*)this, SIGNAL( clicked(int,int,int,const QPoint&) ),
            this, SLOT( slotClicked(int,int,int,const QPoint&) ) );
    connect((QTable*)this, SIGNAL(valueChanged(int,int) ),
            this, SLOT( slotValueChanged(int,int) ) );
    connect((QTable*)this, SIGNAL(currentChanged(int,int) ),
            this, SLOT( slotCurrentChanged(int,int) ) );

    /* now let's init the config */
    initConfig();


    m_enablePaint = true;
    setUpdatesEnabled( true );
    viewport()->setUpdatesEnabled( true );
    viewport()->update();
    m_first = true;
    m_initialising = false;
}

/* a new day has started
 * update the day
 */
void TableView::newDay()
{
    clear();
    updateView();
}

TableView::~TableView()
{
    Config config( "todo" );
    config.setGroup( "Options" );
    for (int i = 0; i < numCols(); i++ )
        config.writeEntry("Width"+QString::number(i), columnWidth(i) );
    config.writeEntry( "SortColumn", sortOrder() );
    config.writeEntry( "SortAscending", ascending() );
}

QString TableView::type() const
{
    return QString::fromLatin1( tr("Table View") );
}

int TableView::current()
{
    if ( numRows() == 0 )
        return 0;

    int uid = sorted().uidAt(currentRow() );
    return uid;
}

int TableView::next()
{
    if ( numRows() == 0 )
        return 0;
    if ( currentRow() + 1 >= numRows() )
        return 0;

    setCurrentCell( currentRow() +1, currentColumn()  );
    return sorted().uidAt( currentRow() );
}

int TableView::prev()
{
    if ( numRows() == 0 )
        return 0;
    if ( currentRow() - 1 < 0 )
        return 0;

    setCurrentCell( currentRow() -1, currentColumn()  );
    return sorted().uidAt( currentRow() );
}

QString TableView::currentRepresentation()
{
    OPimTodo to = sorted()[currentRow()];
    return to.summary().isEmpty() ? to.description().left(20) : to.summary() ;
}

/* show overdue */
void TableView::showOverDue( bool )
{
    clear();
    updateView();
}

void TableView::updateView()
{
    m_row = false;
    static int id;
    id = startTimer(4000 );
    /* FIXME we want one page to be read!
     *
     * Calculate that screensize
     */
    todoWindow()->setReadAhead( 4 );
    sort();
    OPimTodoAccess::List::Iterator it, end;
    it = sorted().begin();
    end = sorted().end();

    QTime time;
    time.start();
    m_enablePaint = false;
    setUpdatesEnabled( false );
    viewport()->setUpdatesEnabled( false );

    setNumRows( it.count() );
    if ( it.count() == 0 )
        killTimer(id);

//    int elc = time.elapsed();
    setUpdatesEnabled( true );
    viewport()->setUpdatesEnabled( true );
    viewport()->update();

    m_enablePaint = true;
//    int el = time.elapsed();
}

void TableView::setTodo( int, const OPimTodo&)
{
    sort();

    /* repaint */
    repaint();
}

void TableView::addEvent( const OPimTodo&)
{
    /* fix problems of not showing the 'Haken' */
    updateView();
}

/*
 * find the event
 * and then replace the complete row
 */
void TableView::replaceEvent( const OPimTodo& ev)
{
    addEvent( ev );
}

/*
 * re aligning table can be slow too
 * FIXME: look what performs better
 * either this or the old align table
 */
void TableView::removeEvent( int )
{
    updateView();
}

void TableView::setShowCompleted( bool )
{
    updateView();
}

void TableView::setShowDeadline( bool b )
{
    if ( b )
        showColumn( 3 );
    else
        hideColumn( 3 );

    // Try to intelligently size columns
    // TODO - would use width() below, but doesn't have valid value at time of c'tor
    int col2width = 238;
    int width = m_pic_completed.width();
    setColumnWidth( 0, width );
    col2width -= width;
    width = fontMetrics().boundingRect( horizontalHeader()->label( 1 ) ).width() + 8;
    setColumnWidth( 1, width );
    col2width -= width;
    if ( b ) {
        width = fontMetrics().boundingRect( horizontalHeader()->label( 3 ) ).width() + 8;
        setColumnWidth( 3, width );
        col2width -= width;
    }
    setColumnWidth( 2, col2width );
}

void TableView::setShowCategory( const QString& str)
{
    if ( str != m_oleCat || m_first )
        updateView();

    m_oleCat = str;
    m_first = false;
}

void TableView::clear()
{
    setNumRows(0);
}

void TableView::slotClicked(int row, int col, int,
                            const QPoint& point)
{
    if ( m_editorWidget.cellWidget() ) {
        //setCellContentFromEditor(m_editorWidget.cellRow(), m_editorWidget.cellCol() );
        endEdit(m_editorWidget.cellRow(), m_editorWidget.cellCol(),
                true, true );
        m_editorWidget.releaseCellWidget();
    }

    if ( !cellGeometry(row, col ).contains(point ) )
        return;

    int ui = sorted().uidAt( row );

    switch( col ) {
        case 0:{
            int x = point.x() -columnPos( col );
            int y = point.y() -rowPos( row );
            int w = columnWidth( col );
            int h = rowHeight( row );
            if ( x >= ( w - BoxSize ) / 2 &&
                x <= ( w - BoxSize ) / 2 + BoxSize &&
                y >= ( h - BoxSize ) / 2 &&
                y <= ( h - BoxSize ) / 2 + BoxSize ) {
                completeItem( row );
            }
        }
            break;

            // Priority emit a double click...
        case 1:{
            QWidget* wid = beginEdit( row, col, FALSE );
            m_editorWidget.setCellWidget( wid, row, col );
        }
            break;

        case 2: {
            showTodo( ui );
            break;
        }
        case 3: {
            TodoView::edit( ui );
            break;
        }
    }
}

QWidget* TableView::widget()
{
    return this;
}

/*
 * We need to overwrite sortColumn
 * because we want to sort whole row
 * based
 * We event want to set the setOrder
 * to a sort() and update()
 */
void TableView::sortColumn( int col, bool asc, bool )
{
    switch(col) {
        case 1:
            col = Opie::OPimTodoAccess::Priority;
            break;
        case 2:
            col = Opie::OPimTodoAccess::SortSummary;
            break;
        case 3:
            col = Opie::OPimTodoAccess::Deadline;
            break;
        case 0:
        default:
            col = Opie::OPimTodoAccess::Completed;
            break;
    }

    setSortOrder( col );
    setAscending( asc );
    if( !m_initialising )
        updateView();
}

void TableView::viewportPaintEvent( QPaintEvent* e )
{
    if ( m_enablePaint )
        QTable::viewportPaintEvent( e );
}

/*
 * This segment is copyrighted by TT
 * it was taken from their todolist
 * application this code is GPL
 */
void TableView::paintCell(QPainter* p,  int row, int col, const QRect& cr, bool )
{
    const QColorGroup &cg = colorGroup();

    p->save();

    OPimTodo task = sorted()[row];

    // TODO - give user option for grid or bars?

    // Paint alternating background bars
    if (  (row % 2 ) == 0 ) {
        p->fillRect( 0, 0, cr.width(), cr.height(), cg.brush( QColorGroup::Base ) );
        p->setPen( QPen( cg.text() ) );
    }
    else {
        p->fillRect( 0, 0, cr.width(), cr.height(), cg.brush( QColorGroup::Background ) );
        p->setPen( QPen( cg.buttonText() ) );
    }

    // Paint grid
    //p->fillRect( 0, 0, cr.width(), cr.height(), cg.brush( QColorGroup::Base ) );
    //QPen op = p->pen();
    //p->setPen(cg.mid());
    //p->drawLine( 0, cr.height() - 1, cr.width() - 1, cr.height() - 1 );
    //p->drawLine( cr.width() - 1, 0, cr.width() - 1, cr.height() - 1 );
    //p->setPen(op);

    QFont f = p->font();
    QFontMetrics fm(f);

    int marg = ( cr.width() - BoxSize ) / 2;
    int x = 0;
    int y = ( cr.height() - BoxSize ) / 2;

    switch(col) {
        case 0:  // completed field
        {
            //p->setPen( QPen( cg.text() ) );
            //p->drawRect( x + marg, y, BoxSize, BoxSize );
            //p->drawRect( x + marg+1, y+1, BoxSize-2, BoxSize-2 );
            int uid = task.uid();
            bool completed;
            if( m_completed.contains( uid ) )
                completed = m_completed[uid];
            else
                completed = task.isCompleted();

            if ( completed )
                p->drawPixmap( x + marg, y, m_pic_completed );
        }
        break;

        case 1:  // priority field
        {
        p->drawPixmap( x + marg, y, m_pic_priority[ task.priority() - 1 ] );
        }
        break;

        case 2:  // description field
        {
            QString text = task.summary().isEmpty() ?
                        task.description().left(20) :
                        task.summary();
            p->drawText( 2, 2 + fm.ascent(), text );
        }
        break;
        case 3:
        {
            QString text;
            if (task.hasDueDate()) {
                int off = QDate::currentDate().daysTo( task.dueDate() );
                if( off == 0 )
                    text = tr("today");
                else if( off == 1 )
                    text = tr("1 day"); // "tomorrow" wouldn't fit
                else
                    text = tr("%1 days").arg(QString::number(off));
                /*
                * set color if not completed
                */
                if (!task.isCompleted() ) {
                    QColor color = Qt::black;
                    if ( off < 0 )
                        color = Qt::red;
                    else if ( off == 0 )
                        color = Qt::darkRed;
                    else if ( off > 0 )
                        color = Qt::green;
                    p->setPen( color );
                }
            }
            else {
                text = tr("None");
            }
            p->drawText(2,2 + fm.ascent(), text);
        }
        break;
    }
    p->restore();
}

QWidget* TableView::createEditor(int row, int col, bool ) const
{
    switch( col ) {
        case 1: {
            /* the priority stuff */
            QComboBox* combo = new QComboBox( viewport() );
            for ( int i = 0; i < 5; i++ ) {
                combo->insertItem( m_pic_priority[ i ] );
            }
            combo->setCurrentItem( sorted()[row].priority()-1 );
            return combo;
        }
            /* summary */
        case 2:{
            QLineEdit* edit = new QLineEdit( viewport() );
            edit->setText( sorted()[row].summary() );
            return edit;
        }
        case 0:
        default:
            return 0l;
    }
}

void TableView::setCellContentFromEditor( int row, int col )
{
    if ( col == 1 ) {
        QWidget* wid = cellWidget(row, 1 );
        if ( wid->inherits("QComboBox") ) {
            int pri = ((QComboBox*)wid)->currentItem() + 1;
            OPimTodo todo = sorted()[row];
            if ( todo.priority() != pri ) {
                todo.setPriority( pri  );
                TodoView::update( todo.uid(), todo );
                updateView();
            }
        }
    }
    else if ( col == 2) {
        QWidget* wid = cellWidget(row, 2);
        if ( wid->inherits("QLineEdit") ) {
            QString text = ((QLineEdit*)wid)->text();
            OPimTodo todo = sorted()[row];
            if ( todo.summary() != text ) {
                todo.setSummary( text );
                TodoView::update( todo.uid(), todo );
                updateView();
            }
        }
    }
}

void TableView::slotPriority()
{
    setCellContentFromEditor( currentRow(), currentColumn() );
}

/*
 * We'll use the TimerEvent to read ahead or to keep the cahce always
 * filled enough.
 * We will try to read ahead 4 items in both ways
 * up and down. On odd or even we will currentRow()+-4 or +-9
 *
 */
void TableView::timerEvent( QTimerEvent*  )
{
    if (sorted().count() == 0 )
        return;

    int row = currentRow();
    if ( m_row ) {
        int ro = row-4;
        if (ro < 0 ) ro = 0;
        sorted()[ro];

        ro = row+4;
        sorted()[ro];
    }
    else {
        int ro = row + 8;
        sorted()[ro];

        ro = row-8;
        if (ro < 0 ) ro = 0;
        sorted()[ro];
    }

    m_row = !m_row;
}

// We want  a strike through completed ;)
// durchstreichen to complete
/*
 * MouseTracking is off this mean we only receive
 * these events if the mouse button is pressed
 * We've the previous point saved
 * We check if the previous and current Point are
 * in the same row.
 * Then we check if they're some pixel horizontal away
 * if the distance between the two points is greater than
 * 8 we mark the underlying todo as completed and do a repaint
 *
 * BUG: When clicking on the Due column and it's scrollable
 * the todo is marked as completed...
 * REASON: QTable is doing auto scrolling which leads to a move
 * in the x coordinate and this way it's able to pass the
 * m_completeStrokeWidth criteria
 * WORKAROUND: strike through needs to strike through the same
 * row and two columns!
 */
void TableView::contentsMouseReleaseEvent( QMouseEvent *e )
{
    int row = rowAt(m_prevP.y());
    int colOld = columnAt(m_prevP.x() );
    int colNew = columnAt(e->x() );
    if ( row == rowAt( e->y() ) && row != -1 &&
         colOld != colNew ) {
        completeItem( row );
        return;
    }
    QTable::contentsMouseReleaseEvent( e );
}

void TableView::contentsMousePressEvent( QMouseEvent *e )
{
    if ( e->button() == RightButton ) {
        QPopupMenu *menu = todoWindow()->contextMenu( current(), sorted()[currentRow()].recurrence().doesRecur() );
        menu->exec( QCursor::pos() );
    }
    else {
        m_prevP = e->pos();
        QTable::contentsMousePressEvent( e );
    }
}

void TableView::keyPressEvent( QKeyEvent* event)
{
    if ( m_editorWidget.cellWidget() ) {
//        setCellContentFromEditor(m_editorWidget.cellRow(), m_editorWidget.cellCol() );
        endEdit(m_editorWidget.cellRow(), m_editorWidget.cellCol(),
                true, true );
        m_editorWidget.releaseCellWidget();
        setFocus();
    }

    if ( sorted().count() < 1 ) {
        QTable::keyPressEvent( event );
        return;
    }

    int row = currentRow();
    int col = currentColumn();

    char key = ::toupper( event->ascii() );
    /* let QTable also handle the d letter */
    if ( key == 'D'  ) {
        event->accept();
        removeQuery( sorted().uidAt( row ) );
        return;
    }


    switch( event->key() ) {
        case Qt::Key_F33:
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Space:
            if ( col == 0 ) {
                completeItem( row );
            }
            else if ( col == 1 ) {
                QWidget* wid = beginEdit(row, col, FALSE );
                m_editorWidget.setCellWidget( wid, row, col );
            }
            else if ( col == 2 ) {
                showTodo( sorted().uidAt( currentRow() ) );
            }
            else if ( col == 3 ) {
                TodoView::edit( sorted().uidAt(row) );
            }
            event->accept();
            break;
        default:
            QTable::keyPressEvent( event );
    }
}

void TableView::completeItem( int row )
{
    // Make the item show as ticked/unticked immediately rather than having
    // to wait for the backend to write - makes the app feel more responsive
    const OPimTodo &todo( sorted()[row] );
    int uid = todo.uid();
    if( !m_completed.contains( uid ) )
        m_completed[uid] = todo.isCompleted();
    m_completed[uid] = !m_completed[uid];
    updateCell(row, 0);
    qApp->processEvents();
    m_completed.remove(uid);

    TodoView::complete(todo);
}
