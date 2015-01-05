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
    src/ClockRenderer.cpp \
    src/ClockRendererDeutscheBahn.cpp \
    src/ClockRendererHelsinkiMetro.cpp \
    src/ClockRendererSwissRailroad.cpp \
    src/ClockSettings.cpp \
    src/ClockTheme.cpp \
    src/QuickClock.cpp \
    src/SystemState.cpp

HEADERS += \
    src/ClockDebug.h \
    src/ClockRenderer.h \
    src/ClockSettings.h \
    src/ClockTheme.h \
    src/QuickClock.h \
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
