#-------------------------------------------------
#
# Project created by QtCreator 2010-07-21T09:30:18
#
#-------------------------------------------------

QT       += core gui multimedia

maemo5 {
	QT += maemo5
}

TARGET = MagRead
TEMPLATE = app

VERSION = 0.1.5
DEFINES += APP_VERSION=$$VERSION

SOURCES += main.cpp\
        magread.cpp \
    carddetect.cpp \
    magdecode.cpp \
    mslib.c \
    llist.c \
    accountcard.cpp \
    aamvacard.cpp \
    settingspage.cpp

HEADERS  += magread.h \
    carddetect.h \
    magdecode.h \
    mslib.h \
    llist.h \
    magcard.h \
    accountcard.h \
    aamvacard.h \
    settingspage.h

symbian {
    TARGET.UID3 = 0xe2c961e1
    TARGET.CAPABILITY += "UserEnvironment"
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
    ICON = "magread.svg"
}

unix {
	INSTALLS += target desktop icon48
	target.path = /usr/bin/magread
	desktop.path = /usr/share/applications/hildon
	desktop.files += maemofiles/magread.desktop
	icon48.path = /usr/share/icons/hicolor/48x48/hildon
	icon48.files += maemofiles/magread.png
}
