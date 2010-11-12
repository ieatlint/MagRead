#include "magdecode.h"

#include <QDebug>

MagDecode::MagDecode(QObject *parent) : QIODevice(parent) {
	silenceCount = 0;
	noiseDetected = false;
	captureAudio = true;

	silenceThreshold = 300;
	normOffset = 0;
}

void MagDecode::start() {
	open( QIODevice::WriteOnly );
}

void MagDecode::stop() {
	close();
}

qint64 MagDecode::writeData( const char *data, qint64 dataLen ) {
	if( !captureAudio )
		return dataLen;
	
	const qint16 *pcmDataBlock = reinterpret_cast<const qint16 *>( data );
	int blockLen = dataLen / sizeof( qint16 );

	/* Roughly estimates what true 0 amplitude is */
	if( normOffset == 0 ) {
		for( int i = 0; i < blockLen; i++ ) {
			normOffset += pcmDataBlock[ i ];
		}

		normOffset /= blockLen;

		return dataLen;
	}

	for( int i = 0; i < blockLen; i++ ) {
		if( qAbs( pcmDataBlock[ i ] ) - normOffset > silenceThreshold ) {
			noiseDetected = true;
			silenceCount = 0;
		} else if( noiseDetected ) {
			silenceCount++;
		}
	}

	pcmData.append( pcmDataBlock, blockLen );
	if( noiseDetected ) {
		if( silenceCount > 200 ) {
			captureAudio = false;
			processSwipe();
		} else if( pcmData.count() > 20000 ) {
			pcmData.clear();
			silenceCount = 0;
			noiseDetected = 0;
		}
	} else
		pcmData.clear();

	return dataLen;
}

void MagDecode::processSwipe() {
	bool valid;

	//Normalize the audio based on calculated 0 level
	for( int i = 0; i < pcmData.count(); i++ )
		pcmData[ i ] -= normOffset;
	
	msData *ms = ms_create( pcmData.data(), pcmData.count() );

	ms_set_peakThreshold( ms, silenceThreshold );
	
	ms_peaks_find_walk( ms );
	ms_peaks_filter_group( ms );

	ms_save( ms, "/home/ieatlint/code/SWipe/tests/x" );

	ms_decode_peaks( ms  );

	valid = ( ms_decode_bits( ms ) == 0 );
	
	MagCard card;
	card.charStream = ms_get_charStream( ms );
	card.bitStream = ms_get_bitStream( ms );
	card.encoding = ms->dataType;
	card.swipeValid = valid;

	emit cardRead( card );
	noiseDetected = false;
	silenceCount = 0;
	captureAudio = true;
	
//	qDebug() << "Card Validity:" << valid;
//	qDebug() << "Chars:" << ms_get_charStream( ms );
//	qDebug() << "Bits:" << ms_get_bitStream( ms );
	
	ms = ms_free( ms );

	pcmData.clear();
}

qint64 MagDecode::readData( char *data, qint64 len ) {
	Q_UNUSED( data );
	Q_UNUSED( len );

	return 0;
}

void MagDecode::setThreshold( int threshold ) {
	silenceThreshold = threshold;
}

