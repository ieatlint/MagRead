#-------------------------------------------------
#
# Project created by QtCreator 2010-07-21T09:30:18
#
#-------------------------------------------------

QT       += core gui multimedia

TARGET = MagRead
TEMPLATE = app

SOURCES += main.cpp\
        magread.cpp \
    carddetect.cpp \
    magdecode.cpp \
    mslib.c \
    llist.c \
    accountcard.cpp

HEADERS  += magread.h \
    carddetect.h \
    magdecode.h \
    mslib.h \
    llist.h \
    magcard.h \
    accountcard.h

symbian {
    TARGET.UID3 = 0xe2c961e1
    TARGET.CAPABILITY += "UserEnvironment"
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x800000
}
