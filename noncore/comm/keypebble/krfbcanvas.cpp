#include "krfbconnection.h"
#include "krfbcanvas.h"
#include "krfbbuffer.h"

#include <qpe/qpeapplication.h>

#include <qclipboard.h>

KRFBCanvas::KRFBCanvas( QWidget *parent, const char *name )
  : QScrollView( parent, name )
{
    connection_ = new KRFBConnection();
    connect( connection_, SIGNAL( loggedIn() ),
	    this, SLOT( loggedIn() ) );

    loggedIn_ = false;

		//QPEApplication::setStylusOperation(viewport(), QPEApplication::RightOnHold);

    viewport()->setFocusPolicy( QWidget::StrongFocus );
    viewport()->setFocus();

    nextRightClick=0;
    nextDoubleClick=0;
}

KRFBCanvas::~KRFBCanvas()
{
}


void KRFBCanvas::openConnection(KRFBServer server)
{

  
		QCString host = server.hostname.latin1();
		password=server.password;
		connection_->connectTo( server);
}


void KRFBCanvas::openURL( const QUrl &url )
{
  if ( loggedIn_ ) {
      qWarning( "openURL invoked when logged in\n" );
      return;
  }

  QCString host = url.host().latin1();
  int display = url.port();

//  connection_->connectTo( host, display );
}

void KRFBCanvas::closeConnection()
{
  loggedIn_ = false;
  connection_->disconnect();

  viewport()->setMouseTracking( false );
  viewport()->setBackgroundMode( PaletteDark );
  setBackgroundMode( PaletteDark );
  update();
}


void KRFBCanvas::bell()
{
  if ( connection_->options()->deIconify ) {
    topLevelWidget()->raise();
    topLevelWidget()->show();
  }
}

void KRFBCanvas::loggedIn()
{
  qWarning( "Ok, we're logged in" );

  //
  // Get ready for action
  //
  loggedIn_ = true;
  viewport()->setMouseTracking( true );
  viewport()->setBackgroundMode( NoBackground );
  setBackgroundMode( NoBackground );

  // Start using the buffer
  connect( connection_->buffer(), SIGNAL( sizeChanged( int, int ) ),
           this, SLOT( resizeContents(int,int) ) );
  connect( connection_->buffer(), SIGNAL( updated( int, int, int, int ) ),
           this, SLOT( viewportUpdate(int,int,int,int) ) );
  connect( connection_->buffer(), SIGNAL( bell() ),
           this, SLOT( bell() ) );
  connect( qApp->clipboard(), SIGNAL( dataChanged() ),
           this, SLOT( clipboardChanged() ) );
}

void KRFBCanvas::viewportPaintEvent( QPaintEvent *e )
{
  QRect r = e->rect();

  if ( loggedIn_ ) {
			QPixmap p;

    bitBlt( viewport(), r.x(), r.y(),
	    connection_->buffer()->pixmap(),
	    r.x() + contentsX(), r.y() + contentsY(),
	    r.width(), r.height() );
  }
  else {
    QScrollView::viewportPaintEvent( e );
  }
}

void KRFBCanvas::viewportUpdate( int x, int y, int w, int h )
{
  updateContents( x, y, w, h );
}

void KRFBCanvas::contentsMousePressEvent( QMouseEvent *e )
{

  if (nextDoubleClick) {
    connection_->buffer()->mouseEvent( &QMouseEvent(QEvent::MouseButtonPress, e->pos(),LeftButton,LeftButton));
    connection_->buffer()->mouseEvent( &QMouseEvent(QEvent::MouseButtonRelease, e->pos(),LeftButton,0));
    connection_->buffer()->mouseEvent( &QMouseEvent(QEvent::MouseButtonRelease, e->pos(),LeftButton,0));
    connection_->buffer()->mouseEvent( &QMouseEvent(QEvent::MouseButtonPress, e->pos(),NoButton,NoButton));
    connection_->buffer()->mouseEvent( &QMouseEvent(QEvent::MouseButtonRelease, e->pos(),NoButton,0));
  } if (nextRightClick) {
    connection_->buffer()->mouseEvent( &QMouseEvent(QEvent::MouseButtonPress, e->pos(),RightButton,RightButton));
    connection_->buffer()->mouseEvent( &QMouseEvent(QEvent::MouseButtonRelease, e->pos(),RightButton,0));
    connection_->buffer()->mouseEvent( &QMouseEvent(QEvent::MouseButtonPress, e->pos(),NoButton,NoButton));
    connection_->buffer()->mouseEvent( &QMouseEvent(QEvent::MouseButtonRelease, e->pos(),NoButton,0));
  } else if ( loggedIn_ )
    connection_->buffer()->mouseEvent( e );

}

void KRFBCanvas::contentsMouseReleaseEvent( QMouseEvent *e )
{
  if ( loggedIn_ && !nextRightClick && !nextDoubleClick) {
    connection_->buffer()->mouseEvent( e );
  }

  nextRightClick=0;
  nextDoubleClick=0;
}

void KRFBCanvas::contentsMouseMoveEvent( QMouseEvent *e )
{
  if ( loggedIn_ )
    connection_->buffer()->mouseEvent( e );
}

void KRFBCanvas::keyPressEvent( QKeyEvent *e )
{
  if ( loggedIn_ )
    connection_->buffer()->keyPressEvent( e );
}

void KRFBCanvas::keyReleaseEvent( QKeyEvent *e )
{
  if ( loggedIn_ )
    connection_->buffer()->keyReleaseEvent( e );
}

void KRFBCanvas::refresh()
{
  if ( loggedIn_ )
    connection_->refresh();
}

void KRFBCanvas::clipboardChanged()
{
  if ( loggedIn_ ) {
      connection_->sendCutText( qApp->clipboard()->text() );
  }
}
void KRFBCanvas::sendCtlAltDel( void)
{

  if ( loggedIn_ ) {
    connection_->buffer()->keyPressEvent( &QKeyEvent(QEvent::KeyPress,Qt::Key_Control, 0,0));
    connection_->buffer()->keyPressEvent( &QKeyEvent(QEvent::KeyPress,Qt::Key_Alt, 0,0));
		connection_->buffer()->keyPressEvent( &QKeyEvent(QEvent::KeyPress,Qt::Key_Delete, 0,0));
		connection_->buffer()->keyPressEvent( &QKeyEvent(QEvent::KeyRelease,Qt::Key_Control, 0,0));
		connection_->buffer()->keyPressEvent( &QKeyEvent(QEvent::KeyRelease,Qt::Key_Alt, 0,0));
    connection_->buffer()->keyPressEvent( &QKeyEvent(QEvent::KeyRelease,Qt::Key_Delete, 0,0));
	}
}

void KRFBCanvas::markDoubleClick( void)
{
  nextRightClick=1;
}

void KRFBCanvas::markRightClick( void)
{
  nextRightClick=1;
}
