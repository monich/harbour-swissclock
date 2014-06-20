/*
  Copyright (C) 2014 Jolla Ltd.
  Contact: Slava Monich <slava.monich@jolla.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ClockSettings.h"
#include "ClockDebug.h"

#define SETTINGS_GROUP "Configuration"
#define SETTING_KEY_SHOW_NUMBERS "showNumbers"
#define SETTING_KEY_INVERT_COLORS "invertColors"

ClockSettings::ClockSettings(QObject* aParent) :
    QObject(aParent),
    iShowNumbers(false),
    iInvertColors(false)
{
    QTRACE("- created");
    QSettings settings;
    settings.beginGroup(SETTINGS_GROUP);
    queryBool(&settings, SETTING_KEY_SHOW_NUMBERS, &iShowNumbers);
    queryBool(&settings, SETTING_KEY_INVERT_COLORS, &iInvertColors);
}

ClockSettings::~ClockSettings()
{
    QTRACE("- destroyed");
}

bool ClockSettings::queryBool(QSettings* aSettings, QString aKey, bool* aValue)
{
    if (aSettings->contains(aKey)) {
        QVariant value = aSettings->value(aKey);
        QTRACE("-" << aKey << "=" << value.toBool());
        if (aValue) *aValue = value.toBool();
        return true;
    }
    return false;
}

void ClockSettings::setShowNumbers(bool aValue)
{
    if (iShowNumbers != aValue) {
        iShowNumbers = aValue;
        QTRACE("-" << SETTING_KEY_SHOW_NUMBERS << "=" << aValue);
        showNumbersChanged(aValue);
        QSettings settings;
        settings.beginGroup(SETTINGS_GROUP);
        settings.setValue(SETTING_KEY_SHOW_NUMBERS, aValue);
    }
}

void ClockSettings::setInvertColors(bool aValue)
{
    if (iInvertColors != aValue) {
        iInvertColors = aValue;
        QTRACE("-" << SETTING_KEY_INVERT_COLORS << "=" << aValue);
        invertColorsChanged(aValue);
        QSettings settings;
        settings.beginGroup(SETTINGS_GROUP);
        settings.setValue(SETTING_KEY_INVERT_COLORS, aValue);
    }
}
