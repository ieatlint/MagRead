#-------------------------------------------------
#
# Project created by QtCreator 2010-07-21T09:30:18
#
#-------------------------------------------------

QT       += core gui

TARGET = MagRead
TEMPLATE = app

maemo5 {
        QT+= maemo5
}


SOURCES += main.cpp\
        magread.cpp \
    carddetect.cpp \
    audioinput.cpp \
    mslib.c \
    llist.c

HEADERS  += magread.h \
    carddetect.h \
    audioinput.h \
    mslib.h \
    llist.h \
    magcard.h

LIBS += -lpulse-simple -lpulse

CONFIG += link_pkconfig

symbian {
    TARGET.UID3 = 0xe2c961e1
    # TARGET.CAPABILITY += 
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}

unix {
	INSTALLS += target desktop icon48
	target.path = /usr/bin/magread
	desktop.path = /usr/share/applications/hildon
	desktop.files += maemofiles/magread.desktop
	icon48.path = /usr/share/icons/hicolor/48x48/hildon
	icon48.files += maemofiles/magread.png
}

