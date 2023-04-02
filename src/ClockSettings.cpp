/*
 * Copyright (C) 2014-2023 Slava Monich <slava@monich.com>
 * Copyright (C) 2014-2016 Jolla Ltd.
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3. Neither the names of the copyright holders nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ClockSettings.h"
#include "ClockDebug.h"

#include <MGConfItem>
#include <QSettings>

#define SETTINGS_GROUP          "Configuration/"
#define DCONF_PATH              "/apps/" CLOCK_APP_NAME "/"
#define DCONF_(x)               DCONF_PATH x

#define KEY_SHOW_NUMBERS        "showNumbers"
#define KEY_INVERT_COLORS       "invertColors"
#define KEY_KEEP_DISPLAY_ON     "keepDisplayOn"
#define KEY_CLOCK_STYLE         "clockStyle"
#define KEY_RENDER_TYPE         "renderType"
#define KEY_ORIENTATION         "orientation"

#define DEFAULT_KEEP_DISPLAY_ON false
#define DEFAULT_ORIENTATION     ClockSettings::OrientationPrimary

#define SETTINGS_SHOW_NUMBERS   SETTINGS_GROUP KEY_SHOW_NUMBERS
#define SETTINGS_INVERT_COLORS  SETTINGS_GROUP KEY_INVERT_COLORS

ClockSettings::ClockSettings(QObject* aParent) :
    QObject(aParent),
    iShowNumbers(new MGConfItem(DCONF_(KEY_SHOW_NUMBERS), this)),
    iInvertColors(new MGConfItem(DCONF_(KEY_INVERT_COLORS), this)),
    iKeepDisplayOn(new MGConfItem(DCONF_(KEY_KEEP_DISPLAY_ON), this)),
    iClockStyle(new MGConfItem(DCONF_(KEY_CLOCK_STYLE), this)),
    iRenderType(new MGConfItem(DCONF_(KEY_RENDER_TYPE), this)),
    iOrientation(new MGConfItem(DCONF_(KEY_ORIENTATION), this))
{
    QTRACE("- created");

    // Pull in settings from the .ini file
    QSettings settings;

    if (settings.contains(SETTINGS_SHOW_NUMBERS)) {
        bool value = settings.value(SETTINGS_SHOW_NUMBERS).toBool();
        QTRACE("- importing " SETTINGS_SHOW_NUMBERS ":" << value);
        iShowNumbers->set(value);
    }
    if (settings.contains(SETTINGS_INVERT_COLORS)) {
        bool value = settings.value(SETTINGS_INVERT_COLORS).toBool();
        QTRACE("- importing " SETTINGS_INVERT_COLORS ":" << value);
        iInvertColors->set(value);
    }
    settings.remove("");
    settings.sync();

    connect(iShowNumbers, SIGNAL(valueChanged()), SIGNAL(showNumbersChanged()));
    connect(iInvertColors, SIGNAL(valueChanged()), SIGNAL(invertColorsChanged()));
    connect(iKeepDisplayOn, SIGNAL(valueChanged()), SIGNAL(keepDisplayOnChanged()));
    connect(iClockStyle, SIGNAL(valueChanged()), SIGNAL(clockStyleChanged()));
    connect(iRenderType, SIGNAL(valueChanged()), SIGNAL(renderTypeChanged()));
    connect(iOrientation, SIGNAL(valueChanged()), SIGNAL(orientationChanged()));
}

ClockSettings::~ClockSettings()
{
    QTRACE("- destroyed");
}

// Callback for qmlRegisterSingletonType<ClockSettings>
QObject*
ClockSettings::createSingleton(
    QQmlEngine*,
    QJSEngine*)
{
    return new ClockSettings;
}

bool
ClockSettings::showNumbers() const
{
    return iShowNumbers->value(DEFAULT_SHOW_NUMBERS).toBool();
}

bool
ClockSettings::invertColors() const
{
    return iInvertColors->value(DEFAULT_INVERT_COLORS).toBool();
}

bool
ClockSettings::keepDisplayOn() const
{
    return iKeepDisplayOn->value(DEFAULT_KEEP_DISPLAY_ON).toBool();
}

QString
ClockSettings::clockStyle() const
{
    return iClockStyle->value(DEFAULT_CLOCK_STYLE).toString();
}

ClockSettings::RenderType
ClockSettings::renderType() const
{
    // Need to cast int to enum right away to force "enumeration value not
    // handled in switch" warning if we miss one of the values:
    ClockSettings::RenderType value = (ClockSettings::RenderType)
        iRenderType->value(DEFAULT_RENDER_TYPE).toInt();
    switch (value) {
    case RenderAuto:
    case RenderSpeed:
    case RenderQuality:
        return value;
    }
    return DEFAULT_RENDER_TYPE;
}

ClockSettings::Orientation
ClockSettings::orientation() const
{
    // Need to cast int to enum right away to force "enumeration value not
    // handled in switch" warning if we miss one of the Orientation:
    ClockSettings::Orientation value = (ClockSettings::Orientation)
        iOrientation->value(DEFAULT_ORIENTATION).toInt();
    switch (value) {
    case OrientationPrimary:
    case OrientationPortrait:
    case OrientationPortraitAny:
    case OrientationLandscape:
    case OrientationLandscapeAny:
    case OrientationAny:
        return value;
    }
    return DEFAULT_ORIENTATION;
}

void
ClockSettings::setShowNumbers(
    bool aValue)
{
    QTRACE("-" << KEY_SHOW_NUMBERS << "=" << aValue);
    iShowNumbers->set(aValue);
}

void
ClockSettings::setInvertColors(
    bool aValue)
{
    QTRACE("-" << KEY_INVERT_COLORS << "=" << aValue);
    iInvertColors->set(aValue);
}

void
ClockSettings::setKeepDisplayOn(
    bool aValue)
{
    QTRACE("-" << KEY_KEEP_DISPLAY_ON << "=" << aValue);
    iKeepDisplayOn->set(aValue);
}

void
ClockSettings::setClockStyle(
    QString aValue)
{
    QTRACE("-" << KEY_CLOCK_STYLE << "=" << aValue);
    iClockStyle->set(aValue);
}
