/*
 * Copyright (C) 2014-2016 Jolla Ltd.
 * Contact: Slava Monich <slava.monich@jolla.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   - Neither the name of Jolla Ltd nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
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

#ifndef QUICK_CLOCK_H
#define QUICK_CLOCK_H

#include "ClockRenderer.h"
#include "ClockSettings.h"
#include "ClockTheme.h"
#include "ClockDebug.h"

#include "HarbourSystemState.h"

#include <QQuickPaintedItem>
#include <QDateTime>
#include <QPainter>
#include <QPixmap>
#include <QList>

#define QUICK_CLOCK_MIN_UPDATE_INTERVAL_DISPLAY_ON  (15)
#define QUICK_CLOCK_MIN_UPDATE_INTERVAL_DISPLAY_OFF (200)

class QuickClockSeconds;

class QuickClock: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool invertColors READ invertColors WRITE setInvertColors NOTIFY invertColorsChanged)
    Q_PROPERTY(bool drawBackground READ drawBackground WRITE setDrawBackground NOTIFY drawBackgroundChanged)
    Q_PROPERTY(int renderType READ renderType WRITE setRenderType NOTIFY renderTypeChanged)
    Q_PROPERTY(QString style READ style WRITE setStyle NOTIFY styleChanged)

public:
    explicit QuickClock(QQuickItem* aParent = NULL);
    ~QuickClock();

    bool invertColors() const;
    void setInvertColors(bool aValue);

    bool drawBackground() const;
    void setDrawBackground(bool aValue);

    bool running() const;
    void setRunning(bool aRunning);

    int renderType() const;
    void setRenderType(int aValue);

    QString style() const;
    void setStyle(QString aValue);

    bool updatesEnabled() const;
    int minUpdateInterval() const;
    ClockRenderer* renderer() const;
    ClockTheme* theme() const;

    static QTime currentTime();

Q_SIGNALS:
    void invertColorsChanged();
    void drawBackgroundChanged();
    void renderTypeChanged();
    void styleChanged();
    void runningChanged();
    void updatesEnabledChanged();

private Q_SLOTS:
    void onWidthChanged();
    void onHeightChanged();
    void checkUpdatesEnabled();
    void onUpdated();

protected:
    virtual void paint(QPainter *painter);
    virtual void timerEvent(QTimerEvent* aEvent);

private:
    bool updateRenderingType();
    void requestUpdate(bool aFullUpdate);
    void paintOffScreenNoSec(QPainter* aPainter, const QSize& aSize,
         const QTime& aTime);
    void repaintHourMin(const QSize& aSize, const QTime& aTime);

private:
    CLOCK_PERFORMANCE_LOG_DEFINE
    QSharedPointer<HarbourSystemState> iSystemState;
    ClockSettings::RenderType iRenderType;
    bool iUpdatesEnabled;
    bool iInvertColors;
    bool iDrawBackground;
    bool iOptimized;
    bool iRunning;
    bool iRepaintAll;
    ClockTheme* iThemeDefault;
    ClockTheme* iThemeInverted;
    QList<ClockRenderer*> iRenderers;
    ClockRenderer* iRenderer;
    QuickClockSeconds* iSecLayer;
    QPixmap* iDialPlatePixmap;
    QPixmap* iHourMinPixmap;
    QTime iPaintTimeNoSec;
    QBasicTimer iRepaintTimer;
    int iPaintHour;
    int iPaintMinute;
};

QML_DECLARE_TYPE(QuickClock)

inline bool QuickClock::invertColors() const
    { return iInvertColors; }
inline bool QuickClock::drawBackground() const
    { return iDrawBackground; }
inline bool QuickClock::running() const
    { return iRunning; }
inline int QuickClock::renderType() const
    { return iRenderType; }
inline QString QuickClock::style() const
    { return iRenderer->id(); }
inline ClockRenderer* QuickClock::renderer() const
    { return iRenderer; }
inline ClockTheme* QuickClock::theme() const
    { return iInvertColors ? iThemeInverted : iThemeDefault; }

#endif // QUICK_CLOCK_H
