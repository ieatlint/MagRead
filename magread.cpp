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

	audioFormat.setFrequency( 48000 );
	audioFormat.setChannels( 1 );
	audioFormat.setSampleSize( 16 );
	audioFormat.setCodec( "audio/pcm" );
	audioFormat.setByteOrder( QAudioFormat::LittleEndian );
	audioFormat.setSampleType( QAudioFormat::SignedInt );

	magDec = NULL;

	qRegisterMetaType<MagCard>( "MagCard" );

}

void MagRead::notice( QString msg ) {
	qDebug() << "NOTICE" << msg;
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
		notice( "Show data from a partial read; may be incomplete/invalid" );
		miscPage( true );
	} else {
		notice( "Swipe Failed! Please Retry" );
	}
}

/* Main Page */
void MagRead::mainPage() {
	QWidget *widget = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout( widget );

	widget->setLayout( layout );

	QLabel *label;
	label = new QLabel( "<span style=\"font-size:18pt;\">MagRead</span>" );
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
		captureStop();
		if( onMainPage )
			button->setText( "Start" );
		else
			button->setText( "Back to Main Page" );
		captureAudio = false;
	} else if( onMainPage ) {
		captureStart();
		button->setText( "Stop" );
		captureAudio = true;
	} else {
		mainPage();
	}

}

void MagRead::captureStart() {
	magDec = new MagDecode( this );
	connect( magDec, SIGNAL( cardRead( MagCard ) ), this, SLOT( cardRead( MagCard ) ) );
	connect( magDec, SIGNAL( errorMsg( QString ) ), this, SLOT( notice( QString ) ) );

	audioInput = new QAudioInput( audioFormat, this );
	magDec->start();
	audioInput->start( magDec );
}

void MagRead::captureStop() {
	if( !magDec )
		return;

	audioInput->stop();
	magDec->stop();
	delete audioInput;
	delete magDec;
	magDec = NULL;
	
}

void MagRead::togglePartialRead( bool _partialRead ) {
	partialRead = _partialRead;
}

/* Credit Page */
void MagRead::creditPage() {
	notice( "Successfully Read Credit Card" );
	QWidget *widget = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout( widget );

	onMainPage = false;

	AccountCard *accountCard = new AccountCard( &card );

	layout->addWidget( accountCard, 1 );

	QPushButton *button = new QPushButton( "Stop" );
	layout->addWidget( button, 0 );
	connect( button, SIGNAL( clicked() ), this, SLOT( toggleRead() ) );

	setCentralWidget( widget );
}

void MagRead::aamvaPage() {
	notice( "Successfully Read AAMVA Card" );
	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout( widget );

	onMainPage = false;

	widget->setLayout( layout );

	QLabel *label;

	label = new QLabel( "<span style=\"font-size:16pt;\">" + card.aamvaIssuerName + "</span>" );
	layout->addWidget( label, 0, 0, 1, 2, Qt::AlignHCenter );

	label = new QLabel( "<span style=\"font-size:16pt;\">" + card.accountNumber + "</span>" );
	layout->addWidget( label, 1, 0, 1 ,2, Qt::AlignHCenter | Qt::AlignTop );
	layout->setRowStretch( 1, 1 );

	/* Calculate the age */
	QDate curDate = QDate::currentDate();

	QString ageStr = QString ( "<span style=\"font-size:16pt;\">Age %1 </span>" ).arg( card.aamvaAge );
	if( card.aamvaAge < 18 ) {
		ageStr.prepend( "<font color=\"red\">" );
		ageStr.append( "</font>" );
	} else if( card.aamvaAge < 21 ) {
		ageStr.prepend( "<font color=\"yellow\">" );
		ageStr.append( "</font>" );
	}

	label = new QLabel( ageStr );
	layout->addWidget( label, 2, 0, 1, 2, Qt::AlignHCenter );

	label = new QLabel( "<span style=\"font-size:12pt;\">Expiration Date</font>" );
	layout->addWidget( label, 3, 0, 1, 1, Qt::AlignHCenter );

	label = new QLabel( "<span style=\"font-size:12pt;\">Date of Birth</span>" );
	layout->addWidget( label, 3, 1, 1, 1, Qt::AlignHCenter );

	QString expDate = card.expirationDate.toString( "MM/dd/yy" );
	expDate.prepend( "<span style=\"font-size:12pt;\">" );
	if( card.expirationDate < QDate::currentDate() ) {
		expDate.prepend( "<font color=\"red\"><div align=\"center\">" );
		expDate.append( "<br>\nEXPIRED</font></div>" );
	}
	expDate.append( "</span>" );
	label = new QLabel( expDate );
	layout->addWidget( label, 4, 0, 1, 1, Qt::AlignHCenter );


	label = new QLabel(  "<span style=\"font-size:12pt;\">" + card.aamvaBirthday.toString( "MM/dd/yy" ) + "</span>" );
	layout->addWidget( label, 4, 1, 1, 1, Qt::AlignHCenter );

	QPushButton *button = new QPushButton( "Stop" );
	layout->addWidget( button, 5, 0, 1, 2 );
	connect( button, SIGNAL( clicked() ), this, SLOT( toggleRead() ) );

	setCentralWidget( widget );
}

/* Misc Page */
void MagRead::miscPage( bool partial ) {
	if( !partial )
		notice( "Successfully Read an Unknown Card" );
	
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

	tmpStr.prepend( "<span style=\"font-size:18pt;\"><div align=\"center\">" );
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

