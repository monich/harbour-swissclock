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

#ifndef CLOCK_SETTINGS_H
#define CLOCK_SETTINGS_H

#include <QObject>

#include "ClockRenderer.h"

#ifdef OPENREPOS
#  define CLOCK_APP_NAME            "openrepos-swissclock"
#else
#  define CLOCK_APP_NAME            "harbour-swissclock"
#endif

#define DEFAULT_SHOW_NUMBERS        false
#define DEFAULT_INVERT_COLORS       false
#define DEFAULT_CLOCK_STYLE         ClockRenderer::SWISS_RAILROAD
#define DEFAULT_RENDER_TYPE         ClockSettings::RenderAuto

class QQmlEngine;
class QJSEngine;
class MGConfItem;

class ClockSettings : public QObject
{
    Q_OBJECT
    Q_ENUMS(RenderType)
    Q_ENUMS(Orientation)
    Q_PROPERTY(bool showNumbers
               READ showNumbers
               WRITE setShowNumbers
               NOTIFY showNumbersChanged)
    Q_PROPERTY(bool invertColors
               READ invertColors
               WRITE setInvertColors
               NOTIFY invertColorsChanged)
    Q_PROPERTY(bool keepDisplayOn
               READ keepDisplayOn
               WRITE setKeepDisplayOn
               NOTIFY keepDisplayOnChanged)
    Q_PROPERTY(QString clockStyle
               READ clockStyle
               WRITE setClockStyle
               NOTIFY clockStyleChanged)
    Q_PROPERTY(int renderType
               READ renderType
               NOTIFY renderTypeChanged)
    Q_PROPERTY(int orientation
               READ orientation
               NOTIFY orientationChanged)

public:
    enum RenderType {
        RenderAuto,
        RenderSpeed,
        RenderQuality
    };

    enum Orientation {
        OrientationPrimary,
        OrientationPortrait,
        OrientationPortraitAny,
        OrientationLandscape,
        OrientationLandscapeAny,
        OrientationAny
    };

    explicit ClockSettings(QObject* aParent = Q_NULLPTR);
    ~ClockSettings();

    // Callback for qmlRegisterSingletonType<ClockSettings>
    static QObject* createSingleton(QQmlEngine*, QJSEngine*);

    bool showNumbers() const;
    bool invertColors() const;
    bool keepDisplayOn() const;
    QString clockStyle() const;
    RenderType renderType() const;
    Orientation orientation() const;

    void setShowNumbers(bool);
    void setInvertColors(bool);
    void setKeepDisplayOn(bool);
    void setClockStyle(QString);

signals:
    void showNumbersChanged();
    void invertColorsChanged();
    void keepDisplayOnChanged();
    void clockStyleChanged();
    void renderTypeChanged();
    void orientationChanged();

private:
    MGConfItem* iShowNumbers;
    MGConfItem* iInvertColors;
    MGConfItem* iKeepDisplayOn;
    MGConfItem* iClockStyle;
    MGConfItem* iRenderType;
    MGConfItem* iOrientation;
};

#endif // CLOCK_SETTINGS_H
