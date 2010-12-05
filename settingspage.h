#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QScrollArea>
#include <QGroupBox>
#include <QSlider>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QAudioDeviceInfo>
#include <QSettings>

class SettingsPage : public QScrollArea {
	Q_OBJECT
	public:
		explicit SettingsPage(QObject *parent = 0);

	private:
		QSettings *settings;

		QWidget *widget;
		QVBoxLayout *layout;

		/* General Settings */
		void makeGeneralBox();
		QGroupBox *generalBox;
		QVBoxLayout *generalLayout;
		QCheckBox *formatCredit;
		QCheckBox *formatAAMVA;
		QCheckBox *autoReorient;
		QLabel *timeOutLabel;
		QSlider *timeOutSlider;

		/* Audio Box */
		void makeAudioBox();
		QGroupBox *audioBox;
		QVBoxLayout *audioLayout;
		QComboBox *audioSource;
		QCheckBox *normCBox;
		QLabel *normLabel;
		QSlider *normSlider;
		QLabel *silenceLabel;
		QSlider *silenceSlider;

	signals:

	private slots:
		void formatCredit_checked( int state );
		void formatAAMVA_checked( int state );
		void autoReorient_checked( int state );

		void timeOutChanged( int value );
		void normChanged( int value );
		void silenceChanged( int value );
		void normChecked( int state );

};

#endif // SETTINGSPAGE_H
