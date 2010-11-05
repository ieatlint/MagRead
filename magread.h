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
#ifndef MAGREAD_H
#define MAGREAD_H

#include <QtGui/QMainWindow>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QDate>
#include <QCheckBox>
#include <QScrollArea>

#include <QMetaType>

#include <QMaemo5InformationBox>

#include "carddetect.h"
#include "audioinput.h"
#include "magcard.h"


class MagRead : public QMainWindow {
	Q_OBJECT

	public:
		MagRead( QWidget *parent = 0 );
	
	private:
		MagCard card;
		CardDetect cardDetect;
		AudioInput audioInput;
		bool captureAudio;
		bool partialRead;
		bool onMainPage;

		void mainPage();
		void creditPage();
		void aamvaPage();
		void miscPage( bool partial = false );

	private slots:
		void cardRead( const MagCard _card );
		void maemoNotice( QString msg, int msec );
		void audioInputError( const QString msg );
		void toggleRead();
		void togglePartialRead( bool _partialRead );
};

#endif // MAGREAD_H
