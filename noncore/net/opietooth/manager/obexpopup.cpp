
#include "obexdialog.h"
#include "obexpopup.h"

/* OPIE */
#include <qpe/qpeapplication.h>

/* QT */
#include <qtimer.h>

using namespace OpieTooth;

/*
 * c'tor init the QAction
 */
ObexPopup::ObexPopup()
        : QPopupMenu()
{
    qWarning("RfcCommPopup c'tor");

    QAction* a;

    /* connect action */
    a = new QAction( ); // so it's get deleted
    a->setText("Push file");
    a->addTo( this );
    connect( a, SIGNAL( activated() ),
             this, SLOT( slotPush() ) );
};


ObexPopup::~ObexPopup()
{}


void ObexPopup::slotPush()
{

    qWarning("push something");

    ObexDialog obexDialog;
    QPEApplication::execDialog( &obexDialog );
}


