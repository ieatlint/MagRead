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
#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <QThread>
#include <QVarLengthArray>
#include <QDateTime>
#include <pulse/pulseaudio.h>
#include <pulse/simple.h>

#include "mslib.h"
#include "magcard.h"

class AudioInput : public QThread {
	Q_OBJECT
	public:
		explicit AudioInput( QObject *parent = 0 );
		~AudioInput();

		void run();
		void stop();
		void processSwipe();

	signals:
		void cardRead( const MagCard& );
		void error( const QString& );

	private:
		pa_simple *paServer;
		pa_sample_spec paSpec;
		int paError;

		bool captureAudio;
		int silenceThresh;
		QVarLengthArray<qint16, 20000> pcmData;

		MagCard card;

		msData *ms;
};

#endif // AUDIOINPUT_H
