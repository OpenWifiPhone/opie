#include "editwindow.h"

#include "qwhatsthis.h"

using namespace Opie;
using namespace Opie::Notes;

editWindowWidget::editWindowWidget( QWidget* parent,  const char* name, bool modal, WFlags fl ) : QDialog( parent, name, modal, fl )
{
    setCaption( tr( "Information:" ) );
    QGridLayout *gridLayout = new QGridLayout(this, 2, 1, 5, 5);
    m_editArea = new QMultiLineEdit(this, "m_editArea");
    gridLayout->addWidget(m_editArea, 0, 0);
    m_editArea->setWordWrap(QMultiLineEdit::WidgetWidth);
    
    // Category
//    label = new QLabel( tr( "Category:" ), container );
//    layout->addWidget( label, 3, 0 );
//    QWhatsThis::add( label, tr( "Select a category to organize this note with." ) );
    m_catSelect = new CategorySelect( this );
    gridLayout->addWidget( m_catSelect, 1, 0 );
    QWhatsThis::add( m_catSelect, tr( "Select a category to organize this note with." ) );

    showMaximized();
}

editWindowWidget::~editWindowWidget()
{
}

void editWindowWidget::readMemo( const OPimMemo &memo )
{
    m_editArea->setText( memo.text() );
    m_catSelect->setCategories( memo.categories(), "Notes", tr( "Notes" ) );
}

void editWindowWidget::writeMemo( OPimMemo &memo )
{
    memo.setText( m_editArea->text() );
    
    QArray<int> arr = m_catSelect->currentCategories();
    memo.setCategories( arr );
}

