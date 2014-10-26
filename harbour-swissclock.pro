TARGET = harbour-swissclock
CONFIG += sailfishapp
CONFIG += link_pkgconfig
PKGCONFIG += mlite5 sailfishapp
QMAKE_CXXFLAGS += -Wno-unused-parameter
QT += dbus

CONFIG(debug, debug|release) {
  DEFINES += CLOCK_DEBUG=1
}

SOURCES += \
    src/main.cpp \
    src/ClockSettings.cpp \
    src/QuickClock.cpp

HEADERS += \
    src/ClockDebug.h \
    src/ClockSettings.h \
    src/QuickClock.h

OTHER_FILES += \
    qml/main.qml \
    qml/cover/*.qml \
    qml/pages/*.qml \
    qml/pages/*.png \
    harbour-swissclock.desktop \
    rpm/harbour-swissclock.changes \
    rpm/harbour-swissclock.spec
