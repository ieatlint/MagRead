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
			CARD_UNSET = 0, // 0000000
			CARD_UNKNOWN = 1,//0000001
			CARD_AAMVA = 2,	// 0000010
			CARD_CC = 64,	// 1000000
			CARD_AMEX = 68,	// 1000100
			CARD_DISC = 72, // 1001000
			CARD_MC = 80,	// 1010000
			CARD_VISA = 96	// 1100000
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

};

Q_DECLARE_OPERATORS_FOR_FLAGS( MagCard::Types )

#endif // MAGCARD_H
