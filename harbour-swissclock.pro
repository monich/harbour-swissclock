openrepos {
    PREFIX = openrepos
    DEFINES += OPENREPOS
} else {
    PREFIX = harbour
}

NAME = swissclock
TARGET = $${PREFIX}-$${NAME}
CONFIG += sailfishapp
CONFIG += link_pkgconfig
PKGCONFIG += mlite5 sailfishapp
QMAKE_CXXFLAGS += -Wno-unused-parameter -Wno-psabi
QT += dbus

app_settings {
    # This path is hardcoded in jolla-settings
    TRANSLATIONS_PATH = /usr/share/translations
} else {
    TRANSLATIONS_PATH = /usr/share/$${TARGET}/translations
}

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
    src/QuickClockLayer.cpp

HEADERS += \
    src/ClockDebug.h \
    src/ClockRenderer.h \
    src/ClockSettings.h \
    src/ClockTheme.h \
    src/QuickClock.h \
    src/QuickClockLayer.h

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
    settings/*.qml \
    settings/harbour-$${NAME}.json \
    icons/harbour-$${NAME}.svg \
    harbour-$${NAME}.desktop \
    translations/*.ts \
    rpm/*-$${NAME}.spec

# Icons
ICON_SIZES = 86 108 128 256
for(s, ICON_SIZES) {
    icon_target = icon$${s}
    icon_dir = icons/$${s}x$${s}
    $${icon_target}.files = $${icon_dir}/$${TARGET}.png
    $${icon_target}.path = /usr/share/icons/hicolor/$${s}x$${s}/apps
    equals(PREFIX, "openrepos") {
        $${icon_target}.extra = cp $${icon_dir}/harbour-$${NAME}.png $$eval($${icon_target}.files)
        $${icon_target}.CONFIG += no_check_exist
    }
    INSTALLS += $${icon_target}
}

# Settings
app_settings {
    settings_json.files = settings/$${TARGET}.json
    settings_json.path = /usr/share/jolla-settings/entries/
    equals(PREFIX, "openrepos") {
        settings_json.extra = sed s/harbour/openrepos/g settings/harbour-$${NAME}.json > $$eval(settings_json.files)
        settings_json.CONFIG += no_check_exist
    }
    settings_qml.files = settings/*.qml
    settings_qml.path = /usr/share/$${TARGET}/settings/
    INSTALLS += settings_qml settings_json
}

# Desktop file
equals(PREFIX, "openrepos") {
    desktop.extra = sed s/harbour/openrepos/g harbour-$${NAME}.desktop > $${TARGET}.desktop
    desktop.CONFIG += no_check_exist
}

# Translations
TRANSLATION_SOURCES = \
  $${_PRO_FILE_PWD_}/qml \
  $${_PRO_FILE_PWD_}/settings

defineTest(addTrFile) {
    in = $${_PRO_FILE_PWD_}/translations/harbour-$$1
    out = $${OUT_PWD}/translations/$${PREFIX}-$$1

    s = $$replace(1,-,_)
    lupdate_target = lupdate_$$s
    lrelease_target = lrelease_$$s

    $${lupdate_target}.commands = lupdate -noobsolete -locations none $${TRANSLATION_SOURCES} -ts \"$${in}.ts\" && \
        mkdir -p \"$${OUT_PWD}/translations\" &&  [ \"$${in}.ts\" != \"$${out}.ts\" ] && \
        cp -af \"$${in}.ts\" \"$${out}.ts\" || :

    $${lrelease_target}.target = $${out}.qm
    $${lrelease_target}.depends = $${lupdate_target}
    $${lrelease_target}.commands = lrelease -idbased \"$${out}.ts\"

    QMAKE_EXTRA_TARGETS += $${lrelease_target} $${lupdate_target}
    PRE_TARGETDEPS += $${out}.qm
    qm.files += $${out}.qm

    export($${lupdate_target}.commands)
    export($${lrelease_target}.target)
    export($${lrelease_target}.depends)
    export($${lrelease_target}.commands)
    export(QMAKE_EXTRA_TARGETS)
    export(PRE_TARGETDEPS)
    export(qm.files)
}

LANGUAGES = hu nl ru

addTrFile($${NAME})
for(l, LANGUAGES) {
    addTrFile($${NAME}-$$l)
}

qm.path = $$TRANSLATIONS_PATH
qm.CONFIG += no_check_exist
INSTALLS += qm
