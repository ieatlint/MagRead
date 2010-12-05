#include "settingspage.h"

SettingsPage::SettingsPage(QObject *parent) {
	widget = new QWidget;

	layout = new QVBoxLayout;
	widget->setLayout( layout );

	settings = new QSettings;

	makeGeneralBox();
	makeAudioBox();

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
		formatCredit->setCheckState( Qt::Checked );
	connect( formatAAMVA, SIGNAL( stateChanged( int ) ), this, SLOT( formatAAMVA_checked( int ) ) );

#ifndef Q_OS_SYMBIAN
	autoReorient = new QCheckBox( "Auto-reorient Screen" );//not symbian
	generalLayout->addWidget( autoReorient );
	if( settings->value( "autoReorient" ) == true )
		autoReorient->setCheckState( Qt::Checked );
	connect( autoReorient, SIGNAL( stateChanged( int ) ), this, SLOT( autoReorient_checked( int ) ) );
#endif
	timeOutLabel = new QLabel( "Timeout: 10" );
	generalLayout->addWidget( timeOutLabel );

	timeOutSlider = new QSlider( Qt::Horizontal );
	timeOutSlider->setTickInterval( 1 );
	timeOutSlider->setRange( 0, 100 );
	if( settings->contains( "timeOut" ) ) {
		timeOutLabel->setText( QString( "Timeout: %1" ).arg( settings->value( "timeOut" ).toInt() ) );
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
}

void SettingsPage::timeOutChanged( int value ) {
	timeOutLabel->setText( QString( "Timeout: %1" ).arg( value ) );
	settings->setValue( "timeOut", value );
}

void SettingsPage::makeAudioBox() {
	audioBox = new QGroupBox( "Audio Settings" );

	audioLayout = new QVBoxLayout;

	audioLayout->addWidget( new QLabel( "Audio Input:" ) );
	audioSource = new QComboBox;
	QList<QAudioDeviceInfo> inputDevices = QAudioDeviceInfo::availableDevices( QAudio::AudioInput );
	for( int i = 0; i < inputDevices.size(); i++ ) {
		//audioSource->addItem( inputDevices.at( i ).deviceName(), qVariantFromValue( inputDevices.at( i ) ) );
		audioSource->addItem( inputDevices.at( i ).deviceName() );
	}
	audioLayout->addWidget( audioSource );
	
	normCBox = new QCheckBox( "Auto-Detect Normalzation" );
	normCBox->setCheckState( Qt::Checked );
	audioLayout->addWidget( normCBox );
	connect( normCBox, SIGNAL( stateChanged( int ) ), this, SLOT( normChecked( int ) ) );

	normLabel = new QLabel( "Normalization: Auto-Detect" );
	audioLayout->addWidget( normLabel );

	normSlider = new QSlider( Qt::Horizontal );
	normSlider->setTickInterval( 10 );
	normSlider->setRange( -500, 500 );
	normSlider->setSliderPosition( 0 );
	normSlider->setEnabled( false );
	audioLayout->addWidget( normSlider );
	connect( normSlider, SIGNAL( valueChanged( int ) ), this, SLOT( normChanged( int ) ) );

	silenceLabel = new QLabel( "Silence Theshold: 300" );
	audioLayout->addWidget( silenceLabel );

	silenceSlider = new QSlider( Qt::Horizontal );
	silenceSlider->setTickInterval( 20 );
	silenceSlider->setRange( 0, 1000 );
	silenceSlider->setSliderPosition( 300 );
	audioLayout->addWidget( silenceSlider );
	connect( silenceSlider, SIGNAL( valueChanged( int ) ), this, SLOT( silenceChanged( int ) ) );

	audioBox->setLayout( audioLayout );

	layout->addWidget( audioBox );
}

void SettingsPage::normChecked( int state ) {
	if( state == Qt::Checked ) {
		normSlider->setEnabled( false );
		normLabel->setText( "Normalization: Auto-Detect" );
	} else {
		normSlider->setEnabled( true );
		normLabel->setText( QString( "Normalization: %1" ).arg( normSlider->value() ) );
	}
}

void SettingsPage::silenceChanged( int value ) {
	silenceLabel->setText( QString( "Silence Threshold: %1" ).arg( value ) );
}

void SettingsPage::normChanged( int value ) {
	normLabel->setText( QString( "Normalization: %1" ).arg( value ) );
}

