#ifndef ACCOUNTCARD_H
#define ACCOUNTCARD_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QEvent>

#include "magcard.h"

class AccountCard : public QWidget {
	Q_OBJECT
	public:
		explicit AccountCard( MagCard *_card = 0 );
		void setCard( MagCard *_card );

	private:
		void showData();

		void reorient();

		enum {
			LANDSCAPE,
			PORTRAIT
		} orientation;

		MagCard *card;

		QVBoxLayout layout;

		QLabel accountNumber;
		QLabel accountHolder;
		
		QWidget vFlexWidget;
		QVBoxLayout vFlexBox;

		QWidget vWidgetAccountIssuer;
		QWidget vWidgetExpirationDate;

		QWidget hFlexWidget;
		QHBoxLayout hFlexBox;

		QVBoxLayout vboxExpirationDate;
		QLabel expirationDate;

		QVBoxLayout vboxAccountIssuer;
		QLabel accountIssuer;

		QFont smallFont;
		QFont medFont;
		QFont accountNumberFont;

	protected:
		bool eventFilter( QObject *obj, QEvent *event );
};

#endif // ACCOUNTCARD_H
