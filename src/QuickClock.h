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

#ifndef QUICK_CLOCK_H
#define QUICK_CLOCK_H

#include "ClockRenderer.h"
#include "ClockSettings.h"
#include "ClockTheme.h"
#include "ClockDebug.h"

#include "HarbourSystemState.h"

#include <QBasicTimer>
#include <QQuickPaintedItem>
#include <QDateTime>
#include <QPainter>
#include <QPixmap>
#include <QList>

class QuickClockLayer;

class QuickClock: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool invertColors READ invertColors WRITE setInvertColors NOTIFY invertColorsChanged)
    Q_PROPERTY(bool drawBackground READ drawBackground WRITE setDrawBackground NOTIFY drawBackgroundChanged)
    Q_PROPERTY(int renderType READ renderType WRITE setRenderType NOTIFY renderTypeChanged)
    Q_PROPERTY(QString style READ style WRITE setStyle NOTIFY styleChanged)

public:
    explicit QuickClock(QQuickItem* aParent = Q_NULLPTR);
    ~QuickClock();

    bool invertColors() const;
    void setInvertColors(bool);

    bool drawBackground() const;
    void setDrawBackground(bool);

    bool running() const;
    void setRunning(bool);

    int renderType() const;
    void setRenderType(int);

    QString style() const;
    void setStyle(QString);

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
    void fullUpdateRequested();

private Q_SLOTS:
    void onWidthChanged();
    void onHeightChanged();
    void checkUpdatesEnabled();
    void onUpdated();

protected:
    virtual void paint(QPainter*);
    virtual void timerEvent(QTimerEvent*);

private:
    bool updateRenderingType();
    void requestUpdate(bool);
    void paintDialPlatePixmap(const QSize&);
    void paintOffScreenNoSec(QPainter*, const QSize&, const QTime&);
    void repaintHourMin(const QSize&, const QTime&);

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
    QuickClockLayer* iLayers;
    QPixmap* iDialPlatePixmap;
    QPixmap* iHourMinPixmap;
    QTime iPaintTimeNoSec;
    QBasicTimer iRepaintTimer;
    int iPaintHour;
    int iPaintMinute;
};

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
