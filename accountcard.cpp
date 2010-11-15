#include "accountcard.h"

#include <QDebug>

AccountCard::AccountCard( MagCard *_card ) {
	QLabel *label;
	setLayout( &layout );

	smallFont.setPointSize( 12 );
	medFont.setPointSize( 16 );

	layout.addWidget( &accountNumber, 1, Qt::AlignHCenter );

	accountHolder.setFont( medFont );
	layout.addWidget( &accountHolder, 1, Qt::AlignHCenter );

	label = new QLabel( "Expiration Date" );
	label->setFont( smallFont );
	vboxExpirationDate.addWidget( label, 1, Qt::AlignHCenter );
	expirationDate.setFont( smallFont );
	vboxExpirationDate.addWidget( &expirationDate, 1, Qt::AlignHCenter );
	vWidgetExpirationDate.setLayout( &vboxExpirationDate );

	label = new QLabel( "Issuer" );
	label->setFont( smallFont );
	vboxAccountIssuer.addWidget( label, 1, Qt::AlignHCenter );
	accountIssuer.setFont( smallFont );
	vboxAccountIssuer.addWidget( &accountIssuer, 1, Qt::AlignHCenter );
	vWidgetAccountIssuer.setLayout( &vboxAccountIssuer );

	vFlexBox.addWidget( &vWidgetExpirationDate );
	vFlexBox.addWidget( &vWidgetAccountIssuer );

	hFlexBox.addWidget( &vWidgetExpirationDate );
	hFlexBox.addWidget( &vWidgetAccountIssuer );

	layout.addLayout( &vFlexBox );

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
	QSize geometry = size();

	if( geometry.width() > geometry.height() ) {
		//landscape
		if( orientation != LANDSCAPE ) {
			qDebug() << "Landscape Mode";
			accountNumberFont.setPointSize( 18 );
			accountNumber.setFont( accountNumberFont );

			layout.removeItem( layout.itemAt( 2 ) );
			hFlexBox.setParent( 0 );
			layout.addLayout( &hFlexBox );
			orientation = LANDSCAPE;
		}
	} else {
		//portrait
		if( orientation != PORTRAIT ) {
			qDebug() << "Portrait Mode";
			accountNumberFont.setPointSize( 12 );
			accountNumber.setFont( accountNumberFont );

			layout.removeItem( layout.itemAt( 2 ) );
			vFlexBox.setParent( 0 );
			layout.addLayout( &vFlexBox );
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

	accountNumber.setText( tmpStr );

	if( !card->accountHolder.isEmpty() )
		accountHolder.setText( card->accountHolder );
	else 
		accountHolder.clear();

	/* Expiration Date */
	tmpStr = card->expirationDate.toString( "MMM dd, yyyy" );
	if( card->expirationDate < QDate::currentDate() ) {
		tmpStr.prepend( "<font color=\"red\">" );
		tmpStr.append( "</font>" );
	}

	expirationDate.setText( tmpStr );

	/* Account Issuer */
	accountIssuer.setText( card->accountIssuer );
}
