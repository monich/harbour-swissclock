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

#include "QuickClock.h"
#include "QuickClockSeconds.h"
#include "ClockSettings.h"
#include "ClockDebug.h"

#include <QQuickWindow>
#include <QSGSimpleTextureNode>

#define AUTO_OPTIMIZE_SIZE (300)
//#define AUTO_OPTIMIZE_SIZE (550)

#define SUPER QQuickPaintedItem

QuickClock::QuickClock(QQuickItem* aParent) :
    SUPER(aParent),
    iSystemState(HarbourSystemState::sharedInstance()),
    iRenderType(DEFAULT_RENDER_TYPE),
    iInvertColors(DEFAULT_INVERT_COLORS),
    iDrawBackground(true),
    iOptimized(false),
    iRunning(true),
    iRepaintAll(true),
    iThemeDefault(ClockTheme::newDefault()),
    iThemeInverted(ClockTheme::newInverted()),
    iRenderer(NULL),
    iSecLayer(NULL),
    iDialPlatePixmap(NULL),
    iHourMinPixmap(NULL),
    iPaintHour(0),
    iPaintMinute(0)
{
    QTRACE("- created");
    setFlags(ItemHasContents);

    iRenderers.append(ClockRenderer::newSwissRailroad());
    iRenderers.append(ClockRenderer::newHelsinkiMetro());
    iRenderers.append(ClockRenderer::newDeutscheBahn());
    setStyle(DEFAULT_CLOCK_STYLE);

    iUpdatesEnabled = updatesEnabled();
    connect(iSystemState.data(), SIGNAL(lockModeChanged()), SLOT(checkUpdatesEnabled()));
    connect(iSystemState.data(), SIGNAL(displayStatusChanged()), SLOT(checkUpdatesEnabled()));
    connect(this, SIGNAL(widthChanged()), SLOT(onWidthChanged()));
    connect(this, SIGNAL(heightChanged()), SLOT(onHeightChanged()));
    updateRenderingType();
    setRunning(true);
}

QuickClock::~QuickClock()
{
    QTRACE("- destroyed");
    delete iDialPlatePixmap;
    delete iHourMinPixmap;
    delete iThemeDefault;
    delete iThemeInverted;
    qDeleteAll(iRenderers);
}

QTime
QuickClock::currentTime()
{
#if HARBOUR_DEBUG
    static const char FIXED_TIME_UNINITIALIZED[] = "";
    static const char* fixedTimeString = FIXED_TIME_UNINITIALIZED;
    static QTime fixedTime;
    if (fixedTimeString == FIXED_TIME_UNINITIALIZED) {
        fixedTimeString = getenv("QUICK_CLOCK_TIME");
        if (fixedTimeString) {
            fixedTime = QTime::fromString(fixedTimeString, "h:mm:s");
            if (!fixedTime.isValid()) {
                fixedTime = QTime::fromString(fixedTimeString, "h:mm:s.z");
            }
            if (fixedTime.isValid()) {
                HDEBUG("Fixed" << fixedTime);
            } else {
                HWARN("Invalid time string " << fixedTimeString);
                fixedTimeString = NULL;
            }
        }
    }
    if (fixedTimeString) return fixedTime;
#endif
    return QTime::currentTime();
}

void
QuickClock::setInvertColors(
    bool aValue)
{
    if (iInvertColors != aValue) {
        iInvertColors = aValue;
        Q_EMIT invertColorsChanged();
        QTRACE("- requesting update");
        requestUpdate(true);
    }
}

void
QuickClock::setDrawBackground(
    bool aValue)
{
    if (iDrawBackground != aValue) {
        iDrawBackground = aValue;
        Q_EMIT drawBackgroundChanged();
        QTRACE("- requesting update");
        requestUpdate(true);
    }
}

void
QuickClock::setStyle(
    QString aValue)
{
    if (aValue.isEmpty()) aValue = DEFAULT_CLOCK_STYLE;
    for (int i=0; i<iRenderers.count(); i++) {
        ClockRenderer* renderer = iRenderers.at(i);
        if (renderer->id() == aValue) {
            QTRACE("style = " << aValue);
            if (iRenderer != renderer) {
                iRenderer = renderer;
                Q_EMIT styleChanged();
                iRepaintAll = true;
                QTRACE("- requesting update");
                requestUpdate(true);
            }
            return;
        }
    }
    QTRACE("unknown style" << aValue);
    if (!iRenderer) iRenderer = iRenderers.at(0);
}


bool
QuickClock::updatesEnabled() const
{
    return iRunning && (!iSystemState->displayOff() || !iSystemState->locked());
}

void
QuickClock::checkUpdatesEnabled()
{
    if (updatesEnabled()) {
        if (!iUpdatesEnabled) {
            iUpdatesEnabled = true;
            QTRACE("- requesting update");
            requestUpdate(false);
            Q_EMIT updatesEnabledChanged();
        }
    } else if (iUpdatesEnabled) {
        iUpdatesEnabled = false;
        Q_EMIT updatesEnabledChanged();
    }
}

void
QuickClock::setRenderType(
    int aValue)
{
    ClockSettings::RenderType renderType = DEFAULT_RENDER_TYPE;
    switch ((ClockSettings::RenderType)aValue) {
    case ClockSettings::RenderAuto:
    case ClockSettings::RenderSpeed:
    case ClockSettings::RenderQuality:
        renderType = (ClockSettings::RenderType)aValue;
        break;
    }

    QTRACE("-" << aValue);
    if (iRenderType != renderType) {
        iRenderType = renderType;
        updateRenderingType();
        Q_EMIT renderTypeChanged();
    }
}

void
QuickClock::setRunning(
    bool aRunning)
{
    QTRACE("-" << aRunning);
    if (iRunning != aRunning) {
        iRunning = aRunning;
        Q_EMIT runningChanged();
        checkUpdatesEnabled();
    }
}

bool
QuickClock::updateRenderingType()
{
    bool optimized = iOptimized;
    switch (iRenderType) {
    case ClockSettings::RenderAuto:
        optimized = width() > AUTO_OPTIMIZE_SIZE ||
                    height() > AUTO_OPTIMIZE_SIZE;
        break;
    case ClockSettings::RenderSpeed:
        optimized = true;
        break;
    case ClockSettings::RenderQuality:
        optimized = false;
        break;
    }
    if (iOptimized != optimized) {
        iOptimized = optimized;
        QTRACE("- switched to" << (optimized ? "optimized" : "non-optimized"));
        if (iOptimized) {
            iSecLayer = new QuickClockSeconds(this);
        } else {
            delete iSecLayer;
            iSecLayer = NULL;
        }
        requestUpdate(true);
        return true;
    }
    return false;
}

void
QuickClock::onWidthChanged()
{
    QTRACE(width());
    if (!updateRenderingType()) {
        requestUpdate(true);
    }
}

void
QuickClock::onHeightChanged()
{
    QTRACE(height());
    if (!updateRenderingType()) {
        requestUpdate(true);
    }
}

void
QuickClock::timerEvent(
    QTimerEvent* aEvent)
{
    if (aEvent->timerId() == iRepaintTimer.timerId()) {
        if (updatesEnabled()) {
            requestUpdate(false);
        } else {
            QTRACE("- stopping updates");
            iRepaintTimer.stop();
        }
    } else {
        SUPER::timerEvent(aEvent);
    }
}

void
QuickClock::requestUpdate(
    bool aFullUpdate)
{
    if (aFullUpdate) {
        iRepaintAll = true;
    }
    update();
}

void
QuickClock::paintOffScreenNoSec(
    QPainter* aPainter,
    const QSize& aSize,
    const QTime& aTime)
{
    if (!iDialPlatePixmap || iDialPlatePixmap->size() != aSize) {
        delete iDialPlatePixmap;
        iDialPlatePixmap = new QPixmap(aSize);

        HDEBUG("drawing dial plate" << aSize.width() << "x" << aSize.height());
        QPainter painter(iDialPlatePixmap);
        QRectF rect(QPoint(0,0), aSize);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(rect, QBrush(Qt::transparent));
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        aPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        iRenderer->paintDialPlate(&painter, aSize, theme(), iDrawBackground);
    }

    aPainter->drawPixmap(0, 0, *iDialPlatePixmap);

    aPainter->save();
    aPainter->setRenderHint(QPainter::Antialiasing);
    aPainter->setRenderHint(QPainter::HighQualityAntialiasing);
    aPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    iRenderer->paintHourMinHands(aPainter, aSize, aTime, theme());
    aPainter->restore();
}

void
QuickClock::repaintHourMin(
    const QSize& aSize,
    const QTime& aTime)
{
    delete iHourMinPixmap;
    iHourMinPixmap = new QPixmap(aSize);
    QPainter painter(iHourMinPixmap);
    QRectF rect(QPoint(0,0), aSize);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(rect, QBrush(Qt::transparent));
    if (Q_UNLIKELY(aTime.second() == 0)) {
        QTime t1 = aTime.addSecs(-1);
        QTime t2(t1.hour(), t1.minute(), 60*t1.msec()/1000);
        QTRACE("- drawing hour and minute hands (live)" <<
            qPrintable(t2.toString("hh:mm:ss.zzz")));
        paintOffScreenNoSec(&painter, aSize, t2);
    } else {
        QTime t1(aTime.hour(), aTime.minute(), 0);
        QVERBOSE("- drawing hour and minute hands" <<
            qPrintable(t1.toString("hh:mm:ss.zzz")));
        paintOffScreenNoSec(&painter, aSize, t1);
    }
}

int
QuickClock::minUpdateInterval() const
{
    return iSystemState->displayOff() ?
        QUICK_CLOCK_MIN_UPDATE_INTERVAL_DISPLAY_OFF:
        QUICK_CLOCK_MIN_UPDATE_INTERVAL_DISPLAY_ON;
}

void
QuickClock::onUpdated()
{
    int msec = 0;
    if (iOptimized) {
        QTime time = currentTime();
        const int minMSec = minUpdateInterval();
        msec = time.second() ? (1000 - time.msec()) : 0;
        if (msec < minMSec) {
            msec = minMSec;
        }
    } else {
        msec = minUpdateInterval();
    }
    QVERBOSE("- update in" << msec << "msec");
    iRepaintTimer.start(msec, this);
}

void
QuickClock::paint(
    QPainter *painter)
{
    QTime time = currentTime();
    QVERBOSE("- rendering" << qPrintable(time.toString("hh:mm:ss.zzz")));

    const int w = (int)width() & ~1;
    const int h = (int)height() & ~1;
    QSize size(w, h);

    if (iRepaintAll) {
        iRepaintAll = false;
        delete iDialPlatePixmap;
        iDialPlatePixmap = NULL;
    }

    if (!iDialPlatePixmap ||
       (time.second() == 0 ||
        time.minute() != iPaintTimeNoSec.minute() ||
        time.hour() != iPaintTimeNoSec.hour())) {
        repaintHourMin(size, time);
    }

    painter->drawPixmap(0, 0, *iHourMinPixmap);

    if (!iOptimized) {
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setRenderHint(QPainter::HighQualityAntialiasing);
        painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        iRenderer->paintSecHand(painter, size, time, theme());
        painter->restore();
        CLOCK_PERFORMANCE_LOG_RECORD;
    }

    QMetaObject::invokeMethod(this, "onUpdated", Qt::QueuedConnection);
}
