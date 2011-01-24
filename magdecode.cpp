#include "magdecode.h"

#include <QDebug>

MagDecode::MagDecode(QObject *parent) : QIODevice(parent) {
	silenceCount = 0;
	noiseDetected = false;
	captureAudio = true;

	silenceThreshold = 300;

	normOffsetFound = false;
	normOffset = 0;

	defaultTimeOut = 10; //default to 10
	timeOut = defaultTimeOut;

	algorithm = "walk";
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
	
	if( timeOut > 0 ) {
		timeOut--;
		return dataLen;
	}

	const qint16 *pcmDataBlock = reinterpret_cast<const qint16 *>( data );
	int blockLen = dataLen / sizeof( qint16 );

	/* Roughly estimates what true 0 amplitude is */
	if( !normOffsetFound ) {
		for( int i = 0; i < blockLen; i++ ) {
			normOffset += pcmDataBlock[ i ];
		}

		normOffset /= blockLen;
		normOffsetFound = true;
		qDebug() << "Norm offset is" << normOffset;
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

	timeOut = defaultTimeOut;

	//Normalize the audio based on calculated 0 level
	for( int i = 0; i < pcmData.count(); i++ )
		pcmData[ i ] -= normOffset;
	
	msData *ms = ms_create( pcmData.data(), pcmData.count() );

	ms_set_peakThreshold( ms, silenceThreshold );
	
	if( algorithm == "intersect" ) {
		ms_peaks_find( ms );
		qDebug() << "Intersection algorithm used";
	} else {
		ms_peaks_find_walk( ms );
		qDebug() << "Walk algorithm used";
	}

	ms_peaks_filter_group( ms );

//	ms_save( ms, "/tmp/swipe" );

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

void MagDecode::setTimeOut( int _timeOut ) {
	defaultTimeOut = _timeOut;
}

void MagDecode::setNorm( int _normOffset ) {
	normOffsetFound = true;
	normOffset = _normOffset;
}

void MagDecode::setAlgorithm( QString _algorithm ) {
	qDebug() << "Settings algorithm to" << _algorithm;
	if( _algorithm == "intersect" )
		algorithm = _algorithm;
	else
		algorithm = "walk";
}
