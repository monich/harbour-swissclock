TARGET = harbour-swissclock
CONFIG += sailfishapp
CONFIG += link_pkgconfig
PKGCONFIG += mlite5 sailfishapp
QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-psabi
QT += dbus

CONFIG(debug, debug|release) {
  QMAKE_CXXFLAGS += -g -O0
  DEFINES += CLOCK_DEBUG
}

SOURCES += \
    src/main.cpp \
    src/ClockRenderer.cpp \
    src/ClockRendererDeutscheBahn.cpp \
    src/ClockRendererHelsinkiMetro.cpp \
    src/ClockRendererSwissRailroad.cpp \
    src/ClockSettings.cpp \
    src/ClockTheme.cpp \
    src/QuickClock.cpp \
    src/QuickClockSeconds.cpp \
    src/SystemState.cpp

HEADERS += \
    src/ClockDebug.h \
    src/ClockRenderer.h \
    src/ClockSettings.h \
    src/ClockTheme.h \
    src/QuickClock.h \
    src/QuickClockSeconds.h \
    src/SystemState.h

OTHER_FILES += \
    qml/*.qml \
    qml/*.png \
    harbour-swissclock.desktop \
    translations/*.ts \
    rpm/harbour-swissclock.changes \
    rpm/harbour-swissclock.spec

CONFIG += sailfishapp_i18n
TRANSLATIONS += \
    translations/harbour-swissclock.ts \
    translations/harbour-swissclock-ru.ts
