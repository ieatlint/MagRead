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
#include "settingspage.h"
#include <QDebug>

SettingsPage::SettingsPage() {
	widget = new QWidget;

	layout = new QVBoxLayout;
	widget->setLayout( layout );

	settings = new QSettings;
	if( !settings->contains( "formatCredit" ) )
		resetAll( false );

	makeGeneralBox();
	makeAudioBox();

	resetButton = new QPushButton( "Reset to defaults" );
	layout->addWidget( resetButton );
	connect( resetButton, SIGNAL( clicked() ), this, SLOT( resetAll() ) );

	setWidget( widget );
	setWidgetResizable( true );
}

void SettingsPage::makeGeneralBox() {
	generalBox = new QGroupBox( "General Settings" );
	
	generalLayout = new QVBoxLayout;

	formatCredit = new QCheckBox( "Format Bank Cards" );
	generalLayout->addWidget( formatCredit );
	if( settings->value( "formatCredit" ) == true )
		formatCredit->setCheckState( Qt::Checked );
	connect( formatCredit, SIGNAL( stateChanged( int ) ), this, SLOT( formatCredit_checked( int ) ) );

	formatAAMVA = new QCheckBox( "Format AAMVA Cards" );
	generalLayout->addWidget( formatAAMVA );
	if( settings->value( "formatAAMVA" ) == true )
		formatAAMVA->setCheckState( Qt::Checked );
	connect( formatAAMVA, SIGNAL( stateChanged( int ) ), this, SLOT( formatAAMVA_checked( int ) ) );

#ifdef Q_WS_MAEMO_5
	autoReorient = new QCheckBox( "Auto-reorient Screen" );//not symbian
	generalLayout->addWidget( autoReorient );
	if( settings->value( "autoReorient" ) == true )
		autoReorient->setCheckState( Qt::Checked );
	connect( autoReorient, SIGNAL( stateChanged( int ) ), this, SLOT( autoReorient_checked( int ) ) );
#endif
	timeOutLabel = new QLabel( "Swipe Timeout: 10" );
	generalLayout->addWidget( timeOutLabel );

	timeOutSlider = new QSlider( Qt::Horizontal );
	timeOutSlider->setTickInterval( 1 );
	timeOutSlider->setRange( 0, 100 );
	if( settings->contains( "timeOut" ) ) {
		timeOutLabel->setText( QString( "Swipe Timeout: %1" ).arg( settings->value( "timeOut" ).toInt() ) );
		timeOutSlider->setSliderPosition( settings->value( "timeOut" ).toInt() );
	} else {
		timeOutSlider->setSliderPosition( 10 );
	}

	connect( timeOutSlider, SIGNAL( valueChanged( int ) ), this, SLOT( timeOutChanged( int ) ) );

	generalLayout->addWidget( timeOutSlider );

	generalBox->setLayout( generalLayout );

	layout->addWidget( generalBox );
}

void SettingsPage::formatCredit_checked( int state ) {
	if( state == Qt::Checked )
		settings->setValue( "formatCredit", true );
	else
		settings->setValue( "formatCredit", false );
}

void SettingsPage::formatAAMVA_checked( int state ) {
	if( state == Qt::Checked )
		settings->setValue( "formatAAMVA", true );
	else
		settings->setValue( "formatAAMVA", false );
}

void SettingsPage::autoReorient_checked( int state ) {
	if( state == Qt::Checked )
		settings->setValue( "autoReorient", true );
	else
		settings->setValue( "autoReorient", false );	
#ifdef Q_WS_MAEMO_5
	if( settings->value( "autoReorient" ) == true ) {
		emit autoReorientSig( true );
	} else {
		emit autoReorientSig( false );
	}
#endif
}

void SettingsPage::timeOutChanged( int value ) {
	timeOutLabel->setText( QString( "Swipe Timeout: %1" ).arg( value ) );
	settings->setValue( "timeOut", value );
}

void SettingsPage::makeAudioBox() {
	audioBox = new QGroupBox( "Audio Settings" );

	audioLayout = new QVBoxLayout;

	audioLayout->addWidget( new QLabel( "Audio Input:" ) );
	audioSource = new QComboBox;
	QList<QAudioDeviceInfo> inputDevices = QAudioDeviceInfo::availableDevices( QAudio::AudioInput );
	for( int i = 0; i < inputDevices.size(); i++ ) {
		audioSource->addItem( inputDevices.at( i ).deviceName() );
	}
	audioLayout->addWidget( audioSource );
	connect( audioSource, SIGNAL( currentIndexChanged( QString ) ), this, SLOT( audioDeviceChanged( QString ) ) );
	
	normCBox = new QCheckBox( "Auto-Detect Normalzation" );
	audioLayout->addWidget( normCBox );
	connect( normCBox, SIGNAL( stateChanged( int ) ), this, SLOT( normChecked( int ) ) );

	normLabel = new QLabel;
	audioLayout->addWidget( normLabel );

	normSlider = new QSlider( Qt::Horizontal );
	normSlider->setTickInterval( 10 );
	normSlider->setRange( -500, 500 );
	audioLayout->addWidget( normSlider );
	connect( normSlider, SIGNAL( valueChanged( int ) ), this, SLOT( normChanged( int ) ) );

	if( settings->contains( "norm" ) )
		normSlider->setSliderPosition( settings->value( "norm" ).toInt() );
	else
		normSlider->setSliderPosition( 0 );
	
	if( !settings->contains( "normAuto" ) || settings->value( "normAuto" ) == true ) {
		normCBox->setCheckState( Qt::Checked );
		normSlider->setEnabled( false );
		normLabel->setText( QString( "Normalization: Auto-Detect" ) );
	} else {
		normLabel->setText( QString( "Normalization: %1" ).arg( normSlider->value() ) );
	}

	silenceLabel = new QLabel( "Silence Theshold: 300" );
	audioLayout->addWidget( silenceLabel );

	silenceSlider = new QSlider( Qt::Horizontal );
	silenceSlider->setTickInterval( 20 );
	silenceSlider->setRange( 0, 1000 );
	silenceSlider->setSliderPosition( 300 );
	audioLayout->addWidget( silenceSlider );
	connect( silenceSlider, SIGNAL( valueChanged( int ) ), this, SLOT( silenceChanged( int ) ) );

	if( settings->contains( "silenceThreshold" ) ) {
		silenceSlider->setSliderPosition( settings->value( "silenceThreshold" ).toInt() );
	} else {
		silenceSlider->setSliderPosition( 300 );
	}
	
	silenceLabel->setText( QString( "Silence Threshold: %1" ).arg( silenceSlider->value() ) );

	audioBox->setLayout( audioLayout );

	layout->addWidget( audioBox );
}

void SettingsPage::audioDeviceChanged( QString audioDevice ) {
	settings->setValue( "audioDevice", audioDevice );
}

void SettingsPage::normChecked( int state ) {
	if( state == Qt::Checked ) {
		normSlider->setEnabled( false );
		normLabel->setText( "Normalization: Auto-Detect" );
		settings->setValue( "normAuto", true );
	} else {
		normSlider->setEnabled( true );
		normLabel->setText( QString( "Normalization: %1" ).arg( normSlider->value() ) );
		settings->setValue( "normAuto", false );
	}
}

void SettingsPage::silenceChanged( int value ) {
	silenceLabel->setText( QString( "Silence Threshold: %1" ).arg( value ) );
	settings->setValue( "silenceThreshold", value );
}

void SettingsPage::normChanged( int value ) {
	normLabel->setText( QString( "Normalization: %1" ).arg( value ) );
	settings->setValue( "norm", value );
}



void SettingsPage::resetAll( bool updateGui ) {
	settings->setValue( "formatCredit", true );
	settings->setValue( "formatAAMVA", true );
	settings->setValue( "autoReorient", true );
	settings->setValue( "timeOut", 10 );

	settings->setValue( "normAuto", true );
	settings->setValue( "norm", 0 );
	settings->setValue( "silenceThreshold", 300 );

	if( updateGui ) {
		formatCredit->setCheckState( Qt::Checked );
		formatAAMVA->setCheckState( Qt::Checked );
#ifdef Q_WS_MAEMO5
		autoReorient->setCheckState( Qt::Checked );
#endif
		timeOutChanged( 10 );

		normCBox->setCheckState( Qt::Checked );
		normSlider->setSliderPosition( 0 );
		normLabel->setText( "Normalization: Auto-Detect" );

		silenceChanged( 300 );
	}
}
