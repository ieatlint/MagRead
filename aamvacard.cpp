#include "aamvacard.h"

#include <QDebug>

AAMVACard::AAMVACard( MagCard *_card ) {
	QLabel *label;
	setLayout( &layout );

	layout.addWidget( &aamvaIssuerName, 1, Qt::AlignHCenter );

	layout.addWidget( &accountNumber, 1, Qt::AlignHCenter );

	layout.addWidget( &aamvaAge, 1, Qt::AlignHCenter );

	label = new QLabel( "<span style=\"font-size:12pt;\">Expiration Date</span>" );
	vboxExpirationDate.addWidget( label, 1, Qt::AlignHCenter );
	vboxExpirationDate.addWidget( &expirationDate, 1, Qt::AlignHCenter );
	vWidgetExpirationDate.setLayout( &vboxExpirationDate );


	label = new QLabel( "<span style=\"font-size:12pt;\">Date of Birth</span>" );
	vboxaamvaBirthday.addWidget( label, 1, Qt::AlignHCenter );
	vboxaamvaBirthday.addWidget( &aamvaBirthday, 1, Qt::AlignHCenter );
	vWidgetaamvaBirthday.setLayout( &vboxaamvaBirthday );

	vFlexBox.addWidget( &vWidgetExpirationDate );
	vFlexBox.addWidget( &vWidgetaamvaBirthday );

	hFlexBox.addWidget( &vWidgetExpirationDate );
	hFlexBox.addWidget( &vWidgetaamvaBirthday );

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

void AAMVACard::reorient() {
	QSize geometry = size();

	if( geometry.width() > geometry.height() ) {
		//landscape
		if( orientation != LANDSCAPE ) {
			qDebug() << "Landscape Mode";
			layout.removeItem( layout.itemAt( 3 ) );
			hFlexBox.setParent( 0 );
			layout.addLayout( &hFlexBox );
			orientation = LANDSCAPE;
		}
	} else {
		//portrait
		if( orientation != PORTRAIT ) {
			qDebug() << "Portrait Mode";
			layout.removeItem( layout.itemAt( 3 ) );
			vFlexBox.setParent( 0 );
			layout.addLayout( &vFlexBox );
			orientation = PORTRAIT;
		}
	}
}

bool AAMVACard::eventFilter( QObject *obj, QEvent *event ) {
	if( event->type() == QEvent::Resize )
		reorient();
	
	return QObject::eventFilter( obj, event );
}

void AAMVACard::setCard( MagCard *_card ) {
	card = _card;
	if( card )
		showData();
}

void AAMVACard::showData() {
	QString tmpStr;

	tmpStr = QString( "<span style=\"font-size:16pt;\">%1</span>" ).arg( card->aamvaIssuerName );
	aamvaIssuerName.setText( tmpStr );

	tmpStr = QString( "<span style=\"font-size:16pt;\">%1</span>" ).arg( card->accountNumber );
	accountNumber.setText( tmpStr );

	tmpStr = QString ( "<span style=\"font-size:16pt;\">Age %1 </span>" ).arg( card->aamvaAge );
	if( card->aamvaAge < 18 ) {
		tmpStr.prepend( "<font color=\"red\">" );
		tmpStr.append( "</font>" );
	} else if( card->aamvaAge < 21 ) {
		tmpStr.prepend( "<font color=\"yellow\">" );
		tmpStr.append( "</font>" );
	}
	aamvaAge.setText( tmpStr );

	tmpStr = card->expirationDate.toString( "MMM dd, yyyy" );
	tmpStr.prepend( "<span style=\"font-size:12pt;\">" );
	tmpStr.append( "</span>" );
	if( card->expirationDate < QDate::currentDate() ) {
		tmpStr.prepend( "<font color=\"red\">" );
		tmpStr.append( "</font>" );
	}
	expirationDate.setText( tmpStr );

	tmpStr = card->aamvaBirthday.toString( "MMM dd, yyyy" );
	tmpStr.prepend( "<span style=\"font-size:12pt;\">" );
	tmpStr.append( "</span>" );
	aamvaBirthday.setText( tmpStr );

}
