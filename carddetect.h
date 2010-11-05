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
#ifndef CARDDETECT_H
#define CARDDETECT_H

#include <QMap>
#include "magcard.h"

struct issuer {
	QString name;
	QString abbreviation;
	QString format;
};

class CardDetect {
	public:
		CardDetect( MagCard *_card = 0 );
		void setCard( MagCard *_card ); 

	private:
		MagCard *card;

		void processCard();

		void luhnCheck();
		void creditCardCheck();
		void aamvaCardCheck( QString expDate );
		void aamvaIssuerList();
		QMap<QString,struct issuer> issuerList;

};


#endif // CARDDETECT_H
