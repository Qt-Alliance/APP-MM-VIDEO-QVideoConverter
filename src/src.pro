SOURCES += main.cpp \
 mainwindow.cpp \
 tools.cpp
TEMPLATE = app
CONFIG += warn_on \
	  thread \
          qt \
 x11

TARGET = qvideoconverter

DESTDIR = ../bin/

HEADERS += mainwindow.h \
 tools.h
QMAKE_CXXFLAGS_DEBUG += -DDEBUG

TRANSLATIONS += lang_de.ts

RESOURCES += languages.qrc \
icons.qrc
