TEMPLATE = lib
TARGET = hotkey
DEPENDPATH += . ../../include src
INCLUDEPATH += . ../../include
DEFINES += ONERING_LIBRARY
CONFIG += shared

HEADERS = \
	../../include/onering.h \
	src/hotkey.h \
	src/qxtglobalshortcut.h \
	src/qxtglobal.h \
	src/qxtglobalshortcut_p.h \
	src/json.h \
	src/app.h

SOURCES = \
	src/hotkey.cpp \
	src/qxtglobalshortcut.cpp \
	src/json.cpp \
	src/app.cpp

macx:SOURCES += src/qxtglobalshortcut_mac.cpp
win32:SOURCES += src/qxtglobalshortcut_win.cpp
unix:!macx:SOURCES += src/qxtglobalshortcut_x11.cpp
macx:LIBS += -framework Carbon
win32 {
	win32-g++:LIBS += -l user32
	!win32-g++:LIBS += user32.lib
}
