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
#include "audioinput.h"

#include <QDebug>

#define BUF_SIZE 1024
//#define DEBUG

AudioInput::AudioInput(QObject *parent) : QThread(parent) {
	paSpec.format = PA_SAMPLE_S16LE;
	paSpec.channels = 1;
	paSpec.rate = 48000;

	paServer = NULL;
	captureAudio = false;
	silenceThresh = 750;

	ms = 0;
}

AudioInput::~AudioInput() {
	if( paServer )
		pa_simple_free( paServer );
}

void AudioInput::run() {
	#ifdef DEBUG
	qDebug() << "AudioInput::run() start";
	#endif

	paServer = pa_simple_new( NULL, "MagRead", PA_STREAM_RECORD, "source.hw0", "Recording", &paSpec, NULL, NULL, &paError );
	if( !paServer ) {
		QString err = "Failed to open PulseAudio server:\n";
		err.append( pa_strerror( paError ) );
		emit error( err );
		return;
	}

	captureAudio = true;


	qint16 pcmDataBlock[ BUF_SIZE ];
	int i, silence = 0;
	bool noise = false;
	pcmData.clear();
	while(  captureAudio ) {
		if( pa_simple_read( paServer, pcmDataBlock, ( sizeof( qint16 ) * BUF_SIZE ), &paError ) ) {
			QString err = "Failed to read a block of data:\n";
			err.append( pa_strerror( paError ) );
			emit error( err );
			captureAudio = false;
			noise = false;
		}

		for( i = 0; i < BUF_SIZE; i++ ) {
			if( qAbs( pcmDataBlock[ i ] ) > silenceThresh ) {
				noise = true;
				silence = 0;
			} else {
				silence++;
			}
		}

		if( noise ) {
			pcmData.append( pcmDataBlock, BUF_SIZE );
		}

		if( noise && silence > 200 ) {
			captureAudio = false;
		} else if( noise && ( pcmData.count() / BUF_SIZE ) > 30 ) {
			/* Stream is too fucking long, we're clearing and restarting */
			pcmData.clear();
			silence = 0;
			noise = false;
		}
	}

	pa_simple_free( paServer );
	paServer = NULL;

	if( noise ) {
		processSwipe();
	}
	#ifdef DEBUG
	qDebug() << "AudioInput::run() stop";
	#endif
}

void AudioInput::stop() {
	#ifdef DEBUG
	qDebug() << "AudioInput::stop() ";
	#endif
	captureAudio = false;
}

void AudioInput::processSwipe() {
	int retval;

	#ifdef DEBUG
	qDebug() << "AudioInput::processSwipe() start";
	#endif

	if( ms )
		ms_free( ms );

	ms = ms_create( pcmData.data(), pcmData.count() );
	#ifdef DEBUG
	qDebug() << "AudioInput::processSwipe() 1";
	#endif

	//ms_peaks_find( ms );
	ms_peaks_find_walk( ms );
	ms_peaks_filter_group( ms );
	#ifdef DEBUG
	qDebug() << "AudioInput::processSwipe() 2";
	#endif

	ms_decode_peaks( ms );
	#ifdef DEBUG
	qDebug() << "AudioInput::processSwipe() 3";
	#endif

	retval = ( ms_decode_bits( ms ) == 0 ) ;

	card.charStream = ms_get_charStream( ms );
	if( !card.charStream.isEmpty() ) {
		card.encoding = ms->dataType;
		card.swipeValid = retval;

		emit cardRead( card );
	}

	run();
}

