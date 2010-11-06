/*
    This file is part of MagRead.

    MagRead is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MagRead is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MagRead.  If not, see <http://www.gnu.org/licenses/>.
    
    Written by Jeffrey Malone <ieatlint@tehinterweb.com>
    http://blog.tehinterweb.com
*/
#include "magread.h"

#include <QDebug>

MagRead::MagRead(QWidget *parent) : QMainWindow(parent) {
	mainPage();

	captureAudio = false;
	partialRead = false;

	qRegisterMetaType<MagCard>( "MagCard" );

	connect( &audioInput, SIGNAL( cardRead( const MagCard& ) ), this, SLOT( cardRead( const MagCard& ) ) );
	connect( &audioInput, SIGNAL( error( const QString& ) ), this, SLOT( audioInputError( const QString& ) ) );
}

void MagRead::maemoNotice( QString msg, int msec ) {
	QMaemo5InformationBox infoBox;

	infoBox.information( 0, msg, msec );
	infoBox.show();
}

void MagRead::cardRead( const MagCard _card ) {
	card = _card;
	cardDetect.setCard( &card );

	if( card.type & MagCard::CARD_CC && card.accountValid ) {
		creditPage();
	} else if( card.type == MagCard::CARD_AAMVA ) {
		aamvaPage();
	} else if( card.swipeValid ) {
		miscPage();
	} else if( partialRead ) {
		maemoNotice( "Show data from a partial read; may be incomplete/invalid!", 1000 );
		miscPage( true );
	} else {
		maemoNotice( "Swipe Failed! Please Retry", 1000 );
	}
}

void MagRead::audioInputError( const QString msg ) {
	maemoNotice( msg, 5000 );
	captureAudio = false;
}

/* Main Page */
void MagRead::mainPage() {
	QWidget *widget = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout( widget );

	widget->setLayout( layout );

	QLabel *label;
	label = new QLabel( "<span style=\"font-size:48pt;\">MagRead</span>" );
	layout->addWidget( label, 1, Qt::AlignHCenter );

	QCheckBox *cbox = new QCheckBox( "Show Partial Data" );
	layout->addWidget( cbox );
	connect( cbox, SIGNAL( toggled( bool ) ), this, SLOT( togglePartialRead( bool ) ) );

	QPushButton *button = new QPushButton( "Start" );
	layout->addWidget( button );
	connect( button, SIGNAL( clicked() ), this, SLOT( toggleRead() ) );

	onMainPage = true;

	setCentralWidget( widget );
}

void MagRead::toggleRead() {
	QPushButton *button = qobject_cast<QPushButton *>( QObject::sender() );

	if( captureAudio ) {
		audioInput.stop();
		if( onMainPage )
			button->setText( "Start" );
		else
			button->setText( "Back to Main Page" );
		captureAudio = false;
	} else if( onMainPage ) {
		audioInput.start();
		button->setText( "Stop" );
		captureAudio = true;
	} else {
		mainPage();
	}

}

void MagRead::togglePartialRead( bool _partialRead ) {
	partialRead = _partialRead;
}

/* Credit Page */
void MagRead::creditPage() {
	maemoNotice( "Successfully Read Credit Card", 1000 );
	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout( widget );

	onMainPage = false;

	widget->setLayout( layout );

	QLabel *label;

	QString accountNumber = card.accountNumber;
	if( card.type == MagCard::CARD_AMEX ) {
		accountNumber.insert( 10, '\t' );
		accountNumber.insert( 4, '\t' );
	} else {
		accountNumber.insert( 12, '\t' );
		accountNumber.insert( 8, '\t' );
		accountNumber.insert( 4, '\t' );
	}

	accountNumber.prepend( "<span style=\"font-size:48pt;\"><br>\n" );
	accountNumber.append( "</span>" );
	
	if( !card.accountHolder.isEmpty() ) {
		accountNumber.append( "\n<span style=\"font-size:36pt;\"><div align=center>" );
		accountNumber.append( card.accountHolder );
		accountNumber.append( "</div></span>" );
	}

	label = new QLabel( accountNumber );
	layout->addWidget( label, 0, 0, 1, 2, Qt::AlignHCenter );

	label = new QLabel( "<span style=\"font-size:24pt;\">Expiration Date</span>" );
	layout->addWidget( label, 1, 0, 1, 1, Qt::AlignHCenter | Qt::AlignBottom );

	label = new QLabel( "<span style=\"font-size:24pt;\">Issuer</span>" );
	layout->addWidget( label, 1, 1, 1, 1, Qt::AlignHCenter | Qt::AlignBottom );

	QString expDate = card.expirationDate.toString( "MM/yy" );
	if( card.expirationDate < QDate::currentDate() ) {
		expDate.prepend( "<font color=\"red\">" );
		expDate.append( "</font>" );
	}

	label = new QLabel( QString( "<span style=\"font-size:32pt;\">%1</span>" ).arg( expDate ) );
	layout->addWidget( label, 2, 0, 1, 1, Qt::AlignHCenter | Qt::AlignTop );

	label = new QLabel( QString( "<span style=\"font-size:32pt;\">%1</span>" ).arg( card.accountIssuer ) );
	layout->addWidget( label, 2, 1, 1, 1, Qt::AlignHCenter | Qt::AlignTop );

	QPushButton *button = new QPushButton( "Stop" );
	layout->addWidget( button, 3, 0, 1, 2 );
	connect( button, SIGNAL( clicked() ), this, SLOT( toggleRead() ) );

	setCentralWidget( widget );
}

void MagRead::aamvaPage() {
	maemoNotice( "Successfully Read AAMVA Card", 1000 );
	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout( widget );

	onMainPage = false;

	widget->setLayout( layout );

	QLabel *label;

	label = new QLabel( "<span style=\"font-size:36pt;\">" + card.aamvaIssuerName + "</span>" );
	layout->addWidget( label, 0, 0, 1, 2, Qt::AlignHCenter );

	label = new QLabel( "<span style=\"font-size:32pt;\">" + card.accountNumber + "</span>" );
	layout->addWidget( label, 1, 0, 1 ,2, Qt::AlignHCenter | Qt::AlignTop );
	layout->setRowStretch( 1, 1 );

	/* Calculate the age */
	QDate curDate = QDate::currentDate();

	QString ageStr = QString ( "<span style=\"font-size:32pt;\">Age %1 </span>" ).arg( card.aamvaAge );
	if( card.aamvaAge < 18 ) {
		ageStr.prepend( "<font color=\"red\">" );
		ageStr.append( "</font>" );
	} else if( card.aamvaAge < 21 ) {
		ageStr.prepend( "<font color=\"yellow\">" );
		ageStr.append( "</font>" );
	}

	label = new QLabel( ageStr );
	layout->addWidget( label, 2, 0, 1, 2, Qt::AlignHCenter );

	label = new QLabel( "<span style=\"font-size:24pt;\">Expiration Date</font>" );
	layout->addWidget( label, 3, 0, 1, 1, Qt::AlignHCenter );

	label = new QLabel( "<span style=\"font-size:24pt;\">Date of Birth</span>" );
	layout->addWidget( label, 3, 1, 1, 1, Qt::AlignHCenter );

	QString expDate = card.expirationDate.toString( "MM/dd/yy" );
	expDate.prepend( "<span style=\"font-size:24pt;\">" );
	if( card.expirationDate < QDate::currentDate() ) {
		expDate.prepend( "<font color=\"red\"><div align=\"center\">" );
		expDate.append( "<br>\nEXPIRED</font></div>" );
	}
	expDate.append( "</span>" );
	label = new QLabel( expDate );
	layout->addWidget( label, 4, 0, 1, 1, Qt::AlignHCenter );


	label = new QLabel(  "<span style=\"font-size:24pt;\">" + card.aamvaBirthday.toString( "MM/dd/yy" ) + "</span>" );
	layout->addWidget( label, 4, 1, 1, 1, Qt::AlignHCenter );

	QPushButton *button = new QPushButton( "Stop" );
	layout->addWidget( button, 5, 0, 1, 2 );
	connect( button, SIGNAL( clicked() ), this, SLOT( toggleRead() ) );

	setCentralWidget( widget );
}

/* Misc Page */
void MagRead::miscPage( bool partial ) {
	if( !partial )
		maemoNotice( "Successfully Read an Unknown Card", 1000 );
	
	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout( widget );

	onMainPage = false;

	widget->setLayout( layout );

	QScrollArea *scroll = new QScrollArea;

	QString tmpStr = card.charStream;
	if( tmpStr.contains( '%' ) ) {
		tmpStr.replace( '^', "<br>\n[Field Separator]<br>\n" );
	} else {
		tmpStr.replace( '=', "<br>\n[Field Separator]<br>\n" );
	}
	tmpStr.replace( '|', "<font color=\"red\">|</font>" );

	tmpStr.prepend( "<span style=\"font-size:36pt;\"><div align=\"center\">" );
	tmpStr.append( "</div></span>" );

	QLabel *label = new QLabel( tmpStr );
	scroll->setWidget( label );
	scroll->setWidgetResizable( true );
	layout->addWidget( scroll, 0, 0, 1, 2, Qt::AlignHCenter );

	label->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

	QPushButton *button = new QPushButton( "Stop" );
	layout->addWidget( button, 1, 0, 1, 2 );
	connect( button, SIGNAL( clicked() ), this, SLOT( toggleRead() ) );

	setCentralWidget( widget );
}

