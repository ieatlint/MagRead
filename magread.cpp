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

	settings = new QSettings;

	//Set the auto-rotation for Maemo5
#ifdef Q_WS_MAEMO_5
	if( settings->value( "autoReorient" ) == true ) {
		setAttribute( Qt::WA_Maemo5AutoOrientation, true );
	}
#endif


#ifdef Q_WS_MAEMO_5
	font.setPointSize( 32 );
#else
	font.setPointSize( 16 );
#endif

	// Start/stop/back selections
#ifdef Q_OS_SYMBIAN
	backSoftKey = new QAction( "Start", this );
	backSoftKey->setSoftKeyRole( QAction::PositiveSoftKey );
	connect( backSoftKey, SIGNAL( triggered() ), this, SLOT( toggleRead() ) );
	addAction( backSoftKey );
#else
	mainWidget = new QWidget;
	mainLayout = new QVBoxLayout( mainWidget );
	mainWidget->setLayout( mainLayout );

	mainBackBtn = new QPushButton( "Start" );
	mainLayout->addWidget( mainBackBtn, 0 );
	connect( mainBackBtn, SIGNAL( clicked() ), this, SLOT( toggleRead() ) );

	setCentralWidget( mainWidget );
#endif

	//Option and Exit selections

#ifndef Q_OS_SYMBIAN
//	Removed until settings functionality implemented
	settingsAction = new QAction( "&Settings", this );
	connect( settingsAction, SIGNAL( triggered() ), this, SLOT( settingsPage() ) );

	aboutAction = new QAction( "&About", this );
	connect( aboutAction, SIGNAL( triggered() ), this, SLOT( aboutDialogue() ) );

	exitAction = new QAction( "E&xit", this );
	connect( exitAction, SIGNAL( triggered() ), this, SLOT( close() ) );

	showDataAction = new QAction( "Show &Data", this );
	connect( showDataAction, SIGNAL( triggered() ), this, SLOT( showData() ) );
#endif

#ifdef Q_WS_MAEMO_5
	menuBar()->addAction( settingsAction );
	menuBar()->addAction( aboutAction );
	menuBar()->addAction( exitAction );
#elif !defined( Q_OS_SYMBIAN )
	fileMenu = menuBar()->addMenu( "&File" );

	fileMenu->addAction( settingsAction );
	fileMenu->addAction( aboutAction );
	fileMenu->addAction( exitAction );
#endif

	mainPage();
}

void MagRead::notice( QString msg, int timeout, mboxStatus status ) {
	qDebug() << status << timeout << msg;


	if( status == CRITICAL ) {
		QMessageBox *mbox = new QMessageBox;
		mbox->setText( msg );
		mbox->setIcon( QMessageBox::Critical );
		mbox->setStandardButtons( QMessageBox::Abort | QMessageBox::Ignore );
		int retval = mbox->exec();

		if( retval == QMessageBox::Abort )
			destroy();
	} else {
#ifdef Q_WS_MAEMO_5
		QMaemo5InformationBox infoBox;

		infoBox.information( 0, msg, timeout );
		infoBox.show();
#else
		QMessageBox *mbox = new QMessageBox;
		mbox->setText( msg );
		if( status == INFORMATION )
			mbox->setIcon( QMessageBox::Information );
		else
			mbox->setIcon( QMessageBox::Warning );

		mbox->setStandardButtons( QMessageBox::NoButton );
		mbox->show();
		QTimer::singleShot( timeout, mbox, SLOT( hide() ) );
#endif
	}


}

void MagRead::showData() {
	if( !card.charStream.isEmpty() ) {
		miscPage();
	}
}

void MagRead::cardRead( const MagCard _card ) {
	card = _card;
	cardDetect.setCard( &card );

	if( ( card.type & MagCard::CARD_CC || card.type == MagCard::CARD_AAA ) && card.accountValid ) {
		if( settings->value( "formatCredit" ) == true )
			creditPage();
		else
			miscPage();

	} else if( card.type == MagCard::CARD_AAMVA ) {
		if( settings->value( "formatAAMVA" ) == true )
			aamvaPage();
		else
			miscPage();
	} else if( card.swipeValid ) {
		miscPage();
	} else if( partialRead && !card.charStream.isEmpty() ) {
		notice( "Showing data from a partial read; may be incomplete/invalid", 750, WARNING );
		miscPage( true );
	} else {
		notice( "Swipe Failed! Please Retry", 750, WARNING );
	}
}

/* Main Page */
void MagRead::mainPage() {
	QWidget *widget = new QWidget;
	QVBoxLayout *layout = new QVBoxLayout( widget );

	widget->setLayout( layout );

	QLabel *label;
	label = new QLabel( "MagRead" );
	label->setFont( font );
	layout->addWidget( label, 1, Qt::AlignHCenter );

	QCheckBox *cbox = new QCheckBox( "Show Partial Data" );
	layout->addWidget( cbox );
	connect( cbox, SIGNAL( toggled( bool ) ), this, SLOT( togglePartialRead( bool ) ) );

	onMainPage = true;

#ifdef Q_OS_SYMBIAN
	QHBoxLayout *optionsBox = new QHBoxLayout;

//	Removed until settings functionality implemented
//	QPushButton *settingsBtn = new QPushButton( "Settings" );
//	optionsBox->addWidget( settingsBtn );

	QPushButton *aboutBtn = new QPushButton( "About" );
	optionsBox->addWidget( aboutBtn );
	connect( aboutBtn, SIGNAL( clicked() ), this, SLOT( aboutDialogue() ) );

	layout->addLayout( optionsBox );

	setCentralWidget( widget );
#else
	if( mainLayout->count() > 1 ) {
		mainLayout->itemAt( 0 )->widget()->hide();
		mainLayout->removeItem( mainLayout->itemAt( 0 ) );
	}
	mainLayout->insertWidget( 0, widget, 1 );
#endif

	removeShowData();

}

void MagRead::aboutDialogue() {
	QMessageBox *mbox = new QMessageBox;

	mbox->setText( "MagRead v" QUOTE( APP_VERSION ) "<br>\nwritten by Jeffrey Malone &lt;<a href=\"mailto:ieatlint@tehnterweb.com\">ieatlint@tehinterweb.com</a>&gt;<br>\nMore information can be found at <a href=\"http://blog.tehinterweb.com/\">http://blog.tehinterweb.com</a>" );

	mbox->exec();
}

void MagRead::toggleRead() {
	QString backStr;

	if( captureAudio ) {
		captureStop();
		if( onMainPage )
			backStr = "Start";
		else
			backStr = "Back";
		captureAudio = false;
	} else if( onMainPage ) {
		captureStart();
		backStr = "Stop";
		captureAudio = true;
	} else {
		card.clear();
		backStr = "Start";
		mainPage();
	}

#ifdef Q_OS_SYMBIAN
			backSoftKey->setText( backStr );
#else
			mainBackBtn->setText( backStr );
#endif
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

void MagRead::addShowData() {
#ifdef Q_WS_MAEMO_5
	if( !menuBar()->actions().contains( showDataAction ) )
		menuBar()->addAction( showDataAction );
#elif !defined( Q_OS_SYMBIAN )
	if( !fileMenu->actions().contains( showDataAction ) )
		fileMenu->addAction( showDataAction );
#endif
}

void MagRead::removeShowData() {
#ifdef Q_WS_MAEMO_5
	if( menuBar()->actions().contains( showDataAction ) )
		menuBar()->removeAction( showDataAction );
#elif !defined( Q_OS_SYMBIAN )
	if( fileMenu->actions().contains( showDataAction ) )
		fileMenu->removeAction( showDataAction );
#endif
}

void MagRead::settingsPage() {
	onMainPage = false;

	SettingsPage *settingsWidget = new SettingsPage;
	connect( settingsWidget, SIGNAL( autoReorientSig( bool ) ), this, SLOT( autoReorient( bool ) ) );

#ifdef Q_OS_SYMBIAN
	setCentralWidget( settingsWidget );
#else
	if( mainLayout->count() > 1 ) {
		mainLayout->itemAt( 0 )->widget()->hide();
		mainLayout->removeItem( mainLayout->itemAt( 0 ) );
	}
	mainLayout->insertWidget( 0, settingsWidget, 1 );
#endif

#ifdef Q_OS_SYMBIAN
	backSoftKey->setText( "Back" );
#else
	mainBackBtn->setText( "Back" );
#endif

	removeShowData();
}

void MagRead::autoReorient( bool enabled ) {
#ifdef Q_WS_MAEMO_5
	setAttribute( Qt::WA_Maemo5AutoOrientation, enabled );
#endif
}

/* Credit Page */
void MagRead::creditPage() {
	notice( "Successfully Read Credit Card", 750 );

	onMainPage = false;

	AccountCard *accountCard = new AccountCard( &card );

#ifdef Q_OS_SYMBIAN
	setCentralWidget( accountCard );
#else
	if( mainLayout->count() > 1 ) {
		mainLayout->itemAt( 0 )->widget()->hide();
		mainLayout->removeItem( mainLayout->itemAt( 0 ) );
	}
	mainLayout->insertWidget( 0, accountCard, 1 );
#endif

	addShowData();

}

void MagRead::aamvaPage() {
	notice( "Successfully Read AAMVA Card", 750 );

	onMainPage = false;

	AAMVACard *aamvaCard = new AAMVACard( &card );

#ifdef Q_OS_SYMBIAN
	setCentralWidget( aamvaCard );
#else
	if( mainLayout->count() > 1 ) {
		mainLayout->itemAt( 0 )->widget()->hide();
		mainLayout->removeItem( mainLayout->itemAt( 0 ) );
	}
	mainLayout->insertWidget( 0, aamvaCard, 1 );
#endif

	addShowData();
}

/* Misc Page */
void MagRead::miscPage( bool partial ) {
	if( !partial )
		notice( "Misc Card Page", 750 );

	onMainPage = false;

	QScrollArea *scroll = new QScrollArea;

	QString tmpStr = card.charStream;
	if( tmpStr.contains( '%' ) ) {
		tmpStr.replace( '^', "<br>\n[Field Separator]<br>\n" );
	} else {
		tmpStr.replace( '=', "<br>\n[Field Separator]<br>\n" );
	}
	tmpStr.replace( '|', "<font color=\"red\">|</font>" );

	tmpStr.prepend( "<div align=\"center\">" );
	tmpStr.append( "</div>" );

	QLabel *label = new QLabel( tmpStr );
	label->setFont( font );
	scroll->setWidget( label );
	scroll->setWidgetResizable( true );

	label->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

#ifdef Q_OS_SYMBIAN
	setCentralWidget( scroll );
#else
	if( mainLayout->count() > 1 ) {
		mainLayout->itemAt( 0 )->widget()->hide();
		mainLayout->removeItem( mainLayout->itemAt( 0 ) );
	}
	mainLayout->insertWidget( 0, scroll, 1 );
#endif
	
	removeShowData();

}

