/**********************************************************************
** Copyright (C) 2000 Trolltech AS.  All rights reserved.
**
** This file is part of Qtopia Environment.
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.trolltech.com/gpl/ for GPL licensing information.
**
** Contact info@trolltech.com if any conditions of this licensing are
** not clear to you.
**
**********************************************************************/


#include "qtetrix.h"

#include <opie2/oresource.h>

#include <qlabel.h>
#include <qdatetime.h>
#include <qlayout.h>
#include <qtimer.h>

#include "ohighscoredlg.h"


void drawTetrixButton( QPainter *p, int x, int y, int w, int h,
		       const QColor *color )
{
    QColor fc;
    if ( color ) {
        QPointArray a;
	a.setPoints( 3,  x,y+h-1, x,y, x+w-1,y );
	p->setPen( color->light() );
	p->drawPolyline( a );
	a.setPoints( 3, x+1,y+h-1, x+w-1,y+h-1, x+w-1,y+1 );
	p->setPen( color->dark() );
	p->drawPolyline( a );
	x++;
	y++;
	w -= 2;
	h -= 2;
	fc = *color;
    }
    else
	fc = p->backgroundColor();
    p->fillRect( x, y, w, h, fc );
}


ShowNextPiece::ShowNextPiece( QWidget *parent, const char *name )
    : QFrame( parent, name )
{
    setFrameStyle( QFrame::Panel | QFrame::Sunken );
    xOffset = -1;     // -1 until first resizeEvent.
}

void ShowNextPiece::resizeEvent( QResizeEvent *e )
{
    QSize sz = e->size();
    blockWidth  = (sz.width()  - 3)/5;
    blockHeight = (sz.height() - 3)/6;
    xOffset     = (sz.width()  - 3)/5;
    yOffset     = (sz.height() - 3)/6;
}


void ShowNextPiece::paintEvent( QPaintEvent * )
{
    QPainter p( this );
    drawFrame( &p );
    p.end();			// explicit end() so any slots can paint too
    emit update();
}


void ShowNextPiece::drawNextSquare(int x, int y,QColor *color)
{
    if (xOffset == -1)		// Before first resizeEvent?
        return;

    QPainter paint;
    paint.begin(this);
    drawTetrixButton( &paint, xOffset+x*blockWidth, yOffset+y*blockHeight,
		      blockWidth, blockHeight, color );
    paint.end();
}


QTetrix::QTetrix( QWidget *parent, const char *name, WFlags f )
    : QMainWindow( parent, name, f )
{
    setIcon( Opie::Core::OResource::loadPixmap( "tetrix_icon" ) );
    setCaption( tr("Tetrix" ) );

    QTime t = QTime::currentTime();
    TetrixPiece::setRandomSeed( (((double)t.hour())+t.minute()+t.second())/
                                 (24+60+60) );

    QWidget *gameArea = new QWidget( this );
    setCentralWidget( gameArea );

    QGridLayout *gl = new QGridLayout( gameArea, 5, 3, 8 );
    gl->setColStretch( 1, 5 );
    gl->setColStretch( 2, 10 );

    QLabel *l;
    l = new QLabel( tr("Next"), gameArea );
    gl->addWidget( l, 0, 0 );
    showNext    = new ShowNextPiece(gameArea);
    showNext->setBackgroundColor(QColor(0,0,0));
    gl->addWidget( showNext, 0, 1 );

    l = new QLabel( tr("Score"), gameArea );
    gl->addWidget( l, 1, 0 );
    showScore   = new QLabel(gameArea);
    gl->addWidget( showScore, 1, 1 );
    l = new QLabel( tr("Level"), gameArea );
    gl->addWidget( l, 2, 0 );
    showLevel   = new QLabel(gameArea);
    gl->addWidget( showLevel, 2, 1 );
    l = new QLabel( tr("Removed"), gameArea );
    gl->addWidget( l, 3, 0 );
    showLines   = new QLabel(gameArea);
    gl->addWidget( showLines, 3, 1 );

    board = new QTetrixBoard(gameArea);
    board->setBackgroundColor(QColor(0,0,0));
    gl->addMultiCellWidget( board, 0, 4, 2, 2 );

    QPushButton *pb = new QPushButton( tr("Start"), gameArea );
    pb->setFocusPolicy( NoFocus );
    connect( pb, SIGNAL( clicked() ), board, SLOT( start() ) );
    gl->addMultiCellWidget( pb, 4, 4, 0, 1 );

    connect( board, SIGNAL(gameOverSignal()), SLOT(gameOver()) );
    connect( board, SIGNAL(drawNextSquareSignal(int,int,QColor*)), this,
	     SLOT(setNext(int,int,QColor*)) );
    connect( showNext, SIGNAL(update()), board, SLOT(updateNext()) );
    connect( board, SIGNAL(updateScoreSignal(int)), showScore, SLOT(setNum(int)) );
    connect( board, SIGNAL(updateLevelSignal(int)), showLevel, SLOT(setNum(int)) );
    connect( board, SIGNAL(updateRemovedSignal(int)), showLines, SLOT(setNum(int)) );

    showScore->setNum( 0 );
    showLevel->setNum( 0 );
    showLines->setNum( 0 );
    board->revealNextPiece(TRUE);
    board->setFocusPolicy( StrongFocus );

    QTimer::singleShot( -1, this, SLOT(setup()) );
}

void QTetrix::setup()
{
    resizeEvent( 0x0 );
}

void QTetrix::gameOver()
{
	OHighscore *hs = new OHighscore( showScore->text().toInt() , showLevel->text().toInt() );
	if ( hs->isNewhighscore )
		hs->insertData( hs->getName(), showScore->text().toInt() , showLevel->text().toInt() );
	OHighscoreDialog hscdlg( hs, this, "OHighscoreDialog", true );
	hscdlg.exec();
}

void QTetrix::quit()
{
    close();
}

void QTetrix::setNext( int x, int y, QColor *color )
{
    resizeEvent( 0x0 );
    showNext->drawNextSquare( x, y, color );
}

void QTetrix::resizeEvent( QResizeEvent * )
{
    // Set size of board
    int widthFactor = board->QFrame::width() / board->boardWidth();
    if ( widthFactor < 1 )
        widthFactor = 1;
    int heightFactor = board->QFrame::height() / board->boardHeight();
    if ( heightFactor < 1 )
        heightFactor = 1;
    widthFactor > heightFactor ? board->resize( heightFactor * board->boardWidth() + 2,
                                                heightFactor * board->boardHeight() + 2 )
                               : board->resize( widthFactor * board->boardWidth() + 2,
                                                widthFactor * board->boardHeight() + 2 );

    // Set size of preview widget
    widthFactor = showNext->width() / 5;
    if ( widthFactor < 1 )
        widthFactor = 1;
    heightFactor = showNext->height() / 6;
    if ( heightFactor < 1 )
        heightFactor = 1;
    widthFactor > heightFactor ? showNext->resize( heightFactor * 5 + 2, heightFactor * 6 + 2 )
                               : showNext->resize( widthFactor * 5 + 2, widthFactor * 6 + 2 );
}
