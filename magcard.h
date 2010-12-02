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
#ifndef MAGCARD_H
#define MAGCARD_H

#include <QDate>

#include "mslib.h"

class MagCard {
	public:
		QString charStream;
		QString bitStream;
		ms_dataType encoding;
		bool swipeValid;
		enum Type {
			CARD_UNSET = 0, // 00000000
			CARD_UNKNOWN = 1,//00000001
			CARD_AAMVA = 2,	// 00000010
			CARD_AAA = 4,	// 00000100
			CARD_CC = 128,	// 10000000
			CARD_AMEX = 136,// 10001000
			CARD_DISC = 144, // 10010000
			CARD_MC = 160,	// 10100000
			CARD_VISA = 192	// 11000000
		};
		Q_DECLARE_FLAGS( Types, Type );
		Types type;

		QString accountNumber;
		QString accountHolder;
		QString accountIssuer;
		QDate expirationDate;
		QString miscData;


		bool accountValid;

		QString aamvaIssuer;
		QString aamvaIssuerName;
		QString aamvaIssuerAbr;
		QDate aamvaBirthday;
		int aamvaAge;

		MagCard() {
			encoding = UNSET;
			type = 0;
			accountValid = false;
			swipeValid = false;
		}

		void clear() {
			type = CARD_UNSET;
			charStream.clear();
			bitStream.clear();
			accountNumber.clear();
		}

};

Q_DECLARE_OPERATORS_FOR_FLAGS( MagCard::Types )

#endif // MAGCARD_H
