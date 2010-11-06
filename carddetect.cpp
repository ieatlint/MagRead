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
#include "carddetect.h"

CardDetect::CardDetect( MagCard *_card ) {
	aamvaIssuerList();
	if( _card )
		setCard( _card );
}

void CardDetect::setCard( MagCard *_card ) {
	card = _card;
	processCard();
}

void CardDetect::processCard() {
	if( !card )
		return;
	
	card->type = MagCard::CARD_UNKNOWN;
	
	/* Fill in the fields.. */
	QString expDate;
	int acctLen = 0;
	if( card->encoding == ABA && card->charStream.startsWith( ';' ) ) {
		acctLen = card->charStream.indexOf( '=' ) - 1;
		if( acctLen > 0 ) {
			card->accountNumber = card->charStream.mid( 1, acctLen );

			expDate = card->charStream.mid( acctLen + 2, 4 );
			card->miscData = card->charStream.mid( acctLen + 6 ).remove( '?' );
		}
	} else if( card->encoding == IATA && card->charStream.startsWith( "%B" ) ) {
		acctLen = card->charStream.indexOf( '^', 1 ) - 2;
		if( acctLen > 0 ) {
			card->accountNumber = card->charStream.mid( 2, acctLen );

			card->miscData = card->charStream.section( '^', 2 ).remove( '?' );
			expDate = card->miscData.left( 4 );
			card->miscData.remove( 0, 4 );
		}

	}
	

	//if the above didn't yield us an account number, just copy the charStream */
	if( card->accountNumber.isEmpty() ) {
		card->accountNumber = card->charStream;
		card->charStream.remove( ';' );
		card->charStream.remove( '%' );
		card->charStream.remove( '?' );
	}

	if( acctLen )
		luhnCheck();
	
	if( card->accountValid )
		creditCardCheck();
	
	/* This code allows partial credit card swipes to be treated as valid, as long at least 3 digits
	 * of extra data that passes checksum are present */
	if( card->accountValid && !card->swipeValid && card->type & MagCard::CARD_CC ) {
		if( 3 > card->miscData.length() || card->miscData.left( 3 ).contains( BAD_CHAR ) ) {
			card->accountValid = false;
		}
	}

	if( card->swipeValid && card->type == MagCard::CARD_UNKNOWN )
		aamvaCardCheck( expDate );
	
	if( !expDate.isEmpty() && card->type != MagCard::CARD_AAMVA ) {
		card->expirationDate = QDate::fromString( expDate, "yyMM" );
		if( card->expirationDate.year() < 1980 )//qdate defaults to 19nn.  Adjust for this.
			card->expirationDate = card->expirationDate.addYears( 100 );

		if( card->expirationDate.isValid() ) {//Make it so it expires on the last day of the given month
			card->expirationDate = card->expirationDate.addDays( card->expirationDate.daysInMonth() - 1);
		} else {
			card->type = MagCard::CARD_UNKNOWN; // must contain a valid date to be known
		}
	}
}

void CardDetect::aamvaIssuerList() {
	issuerList[ "636026" ] =  (struct issuer) { "Arizona", "AZ", "L" };
	issuerList[ "0636021" ] = (struct issuer) { "Arkansas", "AR", "" };
	issuerList[ "636014" ] =  (struct issuer) { "California", "CA", "L" };
	issuerList[ "636020" ] =  (struct issuer) { "Colorado", "CO", "NN-NNN-NNNN" };
	issuerList[ "636010" ] =  (struct issuer) { "Florida", "FL", "LNNN-NNN-NN-NNN-N" };
	issuerList[ "636018" ] =  (struct issuer) { "Iowa", "IA", "NNNLLNNNN" };
	issuerList[ "636022" ] =  (struct issuer) { "Kansas", "KS", "KNN-NN-NNNN" };
	issuerList[ "636007" ] =  (struct issuer) { "Louisiana", "LA", "" };
	issuerList[ "636003" ] =  (struct issuer) { "Maryland", "MD", "L-NNN-NNN-NNN-NNN" };
	issuerList[ "636032" ] =  (struct issuer) { "Michigan", "MI", "L NNN NNN NNN NNN" };
	issuerList[ "636038" ] =  (struct issuer) { "Minnesota", "MN", "L" };
	issuerList[ "636030" ] =  (struct issuer) { "Missouri", "MO", "L" };
	issuerList[ "636039" ] =  (struct issuer) { "New Hampshire", "NH", "NNLLLNNNN" };
	issuerList[ "636009" ] =  (struct issuer) { "New Mexico", "NM", "" };
	issuerList[ "636023" ] =  (struct issuer) { "Ohio", "OH", "LLNNNNNN" };
	issuerList[ "636025" ] =  (struct issuer) { "Pennsylvania", "PA", "NN NNN NNN" };
	issuerList[ "636005" ] =  (struct issuer) { "South Carolina", "SC", "" };
	issuerList[ "636015" ] =  (struct issuer) { "Texas", "TX", "" };
	issuerList[ "636024" ] =  (struct issuer) { "Vermont", "VT", "NNNNNNNL" };
	issuerList[ "636031" ] =  (struct issuer) { "Wisconsin", "WI", "LNNN-NNNN-NNNN-NN" };
	issuerList[ "636027" ] =  (struct issuer) { "State Dept (USA)", "US-DoS", "" };
	
	/* Exceptions:
	 * Arkansas --	Inexplicably, they put a leading 0 on their IIN.  This
	 * 		is handled below in the lookup.
	 * Vermont -- 	I'm told they have additional format to the one listed
	 * 		above that is 8 numbers (no letter).  As the two
	 * 		formats have different field widths, I automatically
	 * 		handle this in the formatting algorithm.
	 */

	//Formatting information is not available for Canada right now
	issuerList[ "636028" ] =  (struct issuer) { "British Columbia", "BC", "" };
	issuerList[ "636017" ] =  (struct issuer) { "New Brunswick", "NB", "" };
	issuerList[ "636016" ] =  (struct issuer) { "Newfoundland", "NL", "" };
	issuerList[ "636013" ] =  (struct issuer) { "Nova Scotia", "NS", "" };
	issuerList[ "636012" ] =  (struct issuer) { "Ontario", "ON", "" };
	issuerList[ "636044" ] =  (struct issuer) { "Saskatchewan", "SK", "" };

	//These may or may not have magstripes.  I'm including them in case they do
	issuerList[ "604427" ] =  (struct issuer) { "American Samoa", "AS", "" };
	issuerList[ "636019" ] =  (struct issuer) { "Guam", "GU", "" };
	issuerList[ "636062" ] =  (struct issuer) { "US Virgin Islands", "US-VI", "" };
	issuerList[ "636056" ] =  (struct issuer) { "Coahuila", "MX-COA", "" };
	issuerList[ "636057" ] =  (struct issuer) { "Hidalgo", "MX-HID", "" };

}
void CardDetect::aamvaCardCheck( QString expDate ) {
	if( card->encoding == IATA )
		return; //we're only going to support ABA for now
	struct issuer issuerInfo;

	QString iin = card->accountNumber.left( 6 );

	issuerInfo = issuerList.value( iin );
	if( issuerInfo.name.isEmpty() ) {
		iin = card->accountNumber.mid( 1, 6 );
		issuerInfo = issuerList.value( iin );
		if( issuerInfo.name.isEmpty() )
			return; // this is not a known AAMVA card, abort
	}

	card->type = MagCard::CARD_AAMVA;

	card->aamvaIssuer = iin;
	card->aamvaIssuerName = issuerInfo.name;
	card->aamvaIssuerAbr = issuerInfo.abbreviation;

	card->accountNumber.remove( iin );
	if( card->miscData.length() > 8 )
		card->accountNumber.append( card->miscData.mid( 8 ) );

	//format the id number if applicable
	if( !issuerInfo.format.isEmpty() ) {
		for( int i = 0; i < issuerInfo.format.length(); i++ ) {
			if( issuerInfo.format.at( i ) == 'L' ) {
				if( card->accountNumber.length() - i < 2 )
					continue;
				QChar letter = card->accountNumber.mid( i, 2 ).toInt() + 64;
				if( letter.isLetter() )
					card->accountNumber.replace( i, 2, letter );
			} else if( issuerInfo.format.at( i ) != 'N' ) {
				card->accountNumber.insert( i, issuerInfo.format.at( i ) );
			}
		}
	}

	//set the birthday
	QString bday = card->miscData.left( 8 );
	if( bday.mid( 4, 2 ) > "12" ) { //some (Calif) violate AAMVA standard and switch the exp and bday month values
		QString exp = bday.mid( 4, 2 );
		bday.replace( 4, 2, expDate.right( 2 ) );
		expDate.replace( 2, 2, exp );
	}
	card->aamvaBirthday = QDate::fromString( bday, "yyyyMMdd" );

	//set the age
	card->aamvaAge = QDate::currentDate().year() - card->aamvaBirthday.year();
	QDate curBday;
	curBday.setDate( QDate::currentDate().year(), card->aamvaBirthday.month(), card->aamvaBirthday.day() );
	if( curBday > QDate::currentDate() )
		card->aamvaAge--;

	//set the expiration date
	if( expDate.endsWith( "99" ) ) { // expires on the birth day and month in the given year
		expDate.replace( 2, 4, bday.mid( 4, 4 ) );
		expDate.prepend( "20" );	

		card->expirationDate = QDate::fromString( expDate, "yyyyMMdd" );
	} else if( !expDate.endsWith( "77" ) ) {
		if( expDate.endsWith( "88" ) )// expires on last day of the month of birth in given year
			expDate.replace( 2, 2, bday.mid( 4, 2 ) );

		expDate.prepend( "20" );
		card->expirationDate = QDate::fromString( expDate, "yyyyMM" );
		card->expirationDate.addDays( card->expirationDate.daysInMonth() - 1 );
	}

}

void CardDetect::creditCardCheck() {
	int acctLen = card->accountNumber.length();
	if( acctLen == 16 ) {
		if( card->accountNumber.startsWith( '4' ) ) {
			card->type = MagCard::CARD_VISA;
			card->accountIssuer = "Visa";
		} else if( card->accountNumber.left( 2 ) >= "51" && card->accountNumber.left( 2 ) <= "55" ) {
			card->type = MagCard::CARD_MC;
			card->accountIssuer = "MasterCard";
		} else if( card->accountNumber.startsWith( "6011" ) || card->accountNumber.startsWith( "65" ) ) {
			card->type = MagCard::CARD_DISC;
			card->accountIssuer = "Discover";
		}
	} else if( acctLen == 15 ) {
		if( card->accountNumber.startsWith( "34" ) || card->accountNumber.startsWith( "37" ) ) {
			card->type = MagCard::CARD_CC | MagCard::CARD_AMEX;
			card->accountIssuer = "American Express";
		}
	}

	if( card->encoding == IATA && card->type & MagCard::CARD_CC ) {
		card->accountHolder = card->charStream.section( '^', 1, 1 ).trimmed();
		if( card->accountHolder.contains( '/' ) ) {  // fix the formatting from "LAST/FIRST" on some cards
			card->accountHolder = card->accountHolder.section( '/', 1, 1 ) + ' ' + card->accountHolder.section( '/', 0, 0 );
		}
	}
}

/* verifies the card->accountNumber against the luhn algorithm and sets card->accountValid */
void CardDetect::luhnCheck() {
	int total = 0;
	for( int i = 0; i < card->accountNumber.length(); i++ ) {
		if( i & 1 ) { // if odd
			total += card->accountNumber.at( i ).digitValue();
		} else {
			int x = card->accountNumber.at( i ).digitValue() * 2;
			total += x % 10;
			if( x > 9 )
				total++;
		}
	}

	card->accountValid = !( total % 10 );
}
