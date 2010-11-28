#include "accountcard.h"

#include <QDebug>

AccountCard::AccountCard( MagCard *_card ) {
	QLabel *label;

	layout = new QVBoxLayout;
	setLayout( layout );

	//smallFont.setPointSize( 12 );
	//medFont.setPointSize( 16 );
	smallFont.setPointSize( 32 );
	medFont.setPointSize( 32 );

	accountNumber = new QLabel;
	layout->addWidget( accountNumber, 1, Qt::AlignHCenter );

	accountHolder = new QLabel;
	accountHolder->setFont( medFont );
	layout->addWidget( accountHolder, 1, Qt::AlignHCenter );

	label = new QLabel( "Expiration Date" );
	label->setFont( smallFont );
	vboxExpirationDate = new QVBoxLayout;
	vboxExpirationDate->addWidget( label, 1, Qt::AlignHCenter );
	expirationDate = new QLabel;
	expirationDate->setFont( smallFont );
	vboxExpirationDate->addWidget( expirationDate, 1, Qt::AlignHCenter );
	vWidgetExpirationDate = new QWidget;
	vWidgetExpirationDate->setLayout( vboxExpirationDate );

	label = new QLabel( "Issuer" );
	label->setFont( smallFont );
	vboxAccountIssuer = new QVBoxLayout;
	vboxAccountIssuer->addWidget( label, 1, Qt::AlignHCenter );
	accountIssuer = new QLabel;
	accountIssuer->setFont( smallFont );
	vboxAccountIssuer->addWidget( accountIssuer, 1, Qt::AlignHCenter );
	vWidgetAccountIssuer = new QWidget;
	vWidgetAccountIssuer->setLayout( vboxAccountIssuer );

	vFlexBox = new QVBoxLayout;
	vFlexBox->addWidget( vWidgetExpirationDate );
	vFlexBox->addWidget( vWidgetAccountIssuer );

	hFlexBox = new QHBoxLayout;
	hFlexBox->addWidget( vWidgetExpirationDate );
	hFlexBox->addWidget( vWidgetAccountIssuer );

	layout->addLayout( vFlexBox );

	//Setup the auto-reorientation
	orientation = UNSET;
	reorient();
	installEventFilter( this );
	
	if( _card ) {
		card = _card;
		showData();
	}
}

void AccountCard::reorient() {
//	QSize geometry = size();
	QRect geometry = QApplication::desktop()->screenGeometry();

	if( geometry.width() > geometry.height() ) {
		//landscape
		if( orientation != LANDSCAPE ) {
			qDebug() << "Landscape Mode";
			accountNumberFont.setPointSize( 48 );//18
			accountNumber->setFont( accountNumberFont );

			layout->removeItem( layout->itemAt( 2 ) );
			hFlexBox->setParent( 0 );
			layout->addLayout( hFlexBox );
			orientation = LANDSCAPE;
		}
	} else {
		//portrait
		if( orientation != PORTRAIT ) {
			qDebug() << "Portrait Mode";
			accountNumberFont.setPointSize( 32 );//12
			accountNumber->setFont( accountNumberFont );

			layout->removeItem( layout->itemAt( 2 ) );
			vFlexBox->setParent( 0 );
			layout->addLayout( vFlexBox );
			orientation = PORTRAIT;
		}
	}
}

bool AccountCard::eventFilter( QObject *obj, QEvent *event ) {
	if( event->type() == QEvent::Resize )
		reorient();
	
	return QObject::eventFilter( obj, event );
}

void AccountCard::setCard( MagCard *_card ) {
	card = _card;
	if( card )
		showData();
}

void AccountCard::showData() {
	QString tmpStr;

	/* Account Number */
	tmpStr = card->accountNumber;
	if( card->type == MagCard::CARD_AMEX ) {
		tmpStr.insert( 10, ' ' );
		tmpStr.insert( 4, ' ' );
	} else {
		tmpStr.insert( 12, ' ' );
		tmpStr.insert( 8, ' ' );
		tmpStr.insert( 4, ' ' );
	}

	accountNumber->setText( tmpStr );

	if( !card->accountHolder.isEmpty() )
		accountHolder->setText( card->accountHolder );
	else 
		accountHolder->clear();

	/* Expiration Date */
	tmpStr = card->expirationDate.toString( "MMM dd, yyyy" );
	if( card->expirationDate < QDate::currentDate() ) {
		tmpStr.prepend( "<font color=\"red\">" );
		tmpStr.append( "</font>" );
	}

	expirationDate->setText( tmpStr );

	/* Account Issuer */
	accountIssuer->setText( card->accountIssuer );
}
