#include "accountcard.h"

#include <QDebug>

AccountCard::AccountCard( MagCard *_card ) {
	QLabel *label;
	setLayout( &layout );

//	label = new QLabel( "<span style=\"font-size:16pt;\">Account Card</span>" );
//	layout.addWidget( label, 1, Qt::AlignHCenter | Qt::AlignTop );

	layout.addWidget( &accountNumber, 1, Qt::AlignHCenter );

	layout.addWidget( &accountHolder, 1, Qt::AlignHCenter );

	label = new QLabel( "<span style=\"font-size:12pt;\">Expiration Date</span>" );
	vboxExpirationDate.addWidget( label, 1, Qt::AlignHCenter );
	vboxExpirationDate.addWidget( &expirationDate, 1, Qt::AlignHCenter );
	vWidgetExpirationDate.setLayout( &vboxExpirationDate );

	label = new QLabel( "<span style=\"font-size:12pt;\">Issuer</span>" );
	vboxAccountIssuer.addWidget( label, 1, Qt::AlignHCenter );
	vboxAccountIssuer.addWidget( &accountIssuer, 1, Qt::AlignHCenter );
	vWidgetAccountIssuer.setLayout( &vboxAccountIssuer );

	vFlexBox.addWidget( &vWidgetExpirationDate );
	vFlexBox.addWidget( &vWidgetAccountIssuer );

	hFlexBox.addWidget( &vWidgetExpirationDate );
	hFlexBox.addWidget( &vWidgetAccountIssuer );

	layout.addLayout( &vFlexBox );

	//Setup the auto-reorientation
	orientation = PORTRAIT;
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
			layout.removeItem( layout.itemAt( 2 ) );
			hFlexBox.setParent( 0 );
			layout.addLayout( &hFlexBox );
			orientation = LANDSCAPE;
		}
	} else {
		//portrait
		if( orientation != PORTRAIT ) {
			qDebug() << "Portrait Mode";
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
		tmpStr.insert( 10, '\t' );
		tmpStr.insert( 4, '\t' );
	} else {
		tmpStr.insert( 12, '\t' );
		tmpStr.insert( 8, '\t' );
		tmpStr.insert( 4, '\t' );
	}


	tmpStr.prepend( "<span style=\"font-size:16pt;\"><br>\n" );
	tmpStr.append( "</span>" );

	accountNumber.setText( tmpStr );

	if( !card->accountHolder.isEmpty() )
		tmpStr = QString( "<span style=\"font-size:12pt;\">%1</span>" ).arg( card->accountHolder );
	else 
		tmpStr.clear();

	accountHolder.setText( tmpStr );
	
	/* Expiration Date */
	tmpStr = card->expirationDate.toString( "MMM dd, yyyy" );
	if( card->expirationDate < QDate::currentDate() ) {
		tmpStr.prepend( "<font color=\"red\">" );
		tmpStr.append( "</font>" );
	}

	expirationDate.setText( QString( "<span style=\"font-size:12pt;\">%1</span>" ).arg( tmpStr ) );

	/* Account Issuer */
	accountIssuer.setText( QString( "<span style=\"font-size:12pt;\">%1</span>" ).arg( card->accountIssuer ) );
}
