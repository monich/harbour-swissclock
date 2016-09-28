TARGET = harbour-swissclock
CONFIG += sailfishapp
CONFIG += link_pkgconfig
PKGCONFIG += mlite5 sailfishapp
QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-psabi
QT += dbus

CONFIG(debug, debug|release) {
  QMAKE_CXXFLAGS += -g -O0
  DEFINES += HARBOUR_DEBUG
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
    src/QuickClockSeconds.cpp
HEADERS += \
    src/ClockDebug.h \
    src/ClockRenderer.h \
    src/ClockSettings.h \
    src/ClockTheme.h \
    src/QuickClock.h \
    src/QuickClockSeconds.h

HARBOUR_LIB = $${_PRO_FILE_PWD_}/harbour-lib
HARBOUR_SRC = $${HARBOUR_LIB}/src
HARBOUR_INCLUDE = $${HARBOUR_LIB}/include
INCLUDEPATH += $${HARBOUR_INCLUDE}
SOURCES += \
  $${HARBOUR_SRC}/HarbourSystemState.cpp
HEADERS += \
    $${HARBOUR_INCLUDE}/HarbourDebug.h \
    $${HARBOUR_INCLUDE}/HarbourSystemState.h

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
