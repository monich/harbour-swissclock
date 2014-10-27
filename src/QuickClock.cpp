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

#include "QuickClock.h"
#include "ClockDebug.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QPainter>

#define SUPER QQuickPaintedItem

#define MCE_SERVICE "com.nokia.mce"

QuickClock::QuickClock(QQuickItem* aParent) :
    SUPER(aParent),
    iDrawBackground(true),
    iThemeDefault(ClockTheme::newDefault()),
    iThemeInverted(ClockTheme::newInverted()),
    iSettings(NULL),
    iDialPlate(NULL),
    iOffScreenNoSec(NULL),
    iTimerId(0)
{
    QTRACE("- created");
    iTheme = iThemeDefault;
    iRenderers.append(ClockRenderer::newSwissRailroad());
    iRenderers.append(ClockRenderer::newHelsinkiMetro());
    setStyle(DEFAULT_CLOCK_STYLE);

#if CLOCK_PERFORMANCE_LOG
    iRenderCount = 0;
    iStartTime = QDateTime::currentDateTime();
#endif // CLOCK_PERFORMANCE_LOG
    setRunning(true);

    // Listen for MCE display state change signals
    QDBusConnection systemBus(QDBusConnection::systemBus());
    systemBus.connect(MCE_SERVICE,
         "/com/nokia/mce/signal", "com.nokia.mce.signal",
         "display_status_ind", this, SLOT(onDisplayStatusChanged(QString)));

    // And query the current state although it's rather unlikely that we
    // have been started with display off
    connect(new QDBusPendingCallWatcher(systemBus.asyncCall(
        QDBusMessage::createMethodCall(MCE_SERVICE,
        "/com/nokia/mce/request", "com.nokia.mce.request",
        "get_display_status")), this), SIGNAL(finished(QDBusPendingCallWatcher*)),
        this, SLOT(onDisplayStatusQueryDone(QDBusPendingCallWatcher*)));
}

QuickClock::~QuickClock()
{
    QTRACE("- destroyed");
    if (iDialPlate) delete iDialPlate;
    if (iOffScreenNoSec) delete iOffScreenNoSec;
    delete iThemeDefault;
    delete iThemeInverted;
    while (!iRenderers.isEmpty()) delete iRenderers.takeLast();
}

void QuickClock::invalidatePixmaps()
{
    if (iDialPlate) {
        delete iDialPlate;
        iDialPlate = NULL;
    }
    if (iOffScreenNoSec) {
        delete iOffScreenNoSec;
        iOffScreenNoSec = NULL;
    }
}

void QuickClock::setDrawBackground(bool aValue)
{
    if (iDrawBackground != aValue) {
        iDrawBackground = aValue;
        invalidatePixmaps();
        drawBackgroundChanged();
        update();
    }
}

void QuickClock::setSettings(ClockSettings* aSettings)
{
    if (iSettings != aSettings) {
        if (iSettings) iSettings->disconnect(this);
        iSettings = aSettings;
        if (iSettings) {
            iTheme = iSettings->invertColors() ? iThemeDefault : iThemeInverted;
            setStyle(iSettings->clockStyle());
            connect(iSettings, SIGNAL(invertColorsChanged()),
                SLOT(onInvertColorsChanged()));
        } else {
            iTheme = iThemeDefault;
        }
        settingsChanged();
        invalidatePixmaps();
        update();
    }
}

void QuickClock::setStyle(QString aStyle)
{
    if (aStyle.isEmpty()) aStyle = DEFAULT_CLOCK_STYLE;
    for (int i=0; i<iRenderers.count(); i++) {
        ClockRenderer* renderer = iRenderers.at(i);
        if (renderer->id() == aStyle) {
            QTRACE("style = " << aStyle);
            if (iRenderer != renderer) {
                iRenderer = renderer;
                invalidatePixmaps();
                styleChanged();
            }
            return;
        }
    }
    QTRACE("unknown style" << aStyle);
    if (!iRenderer) iRenderer = iRenderers.at(0);
}

void QuickClock::onInvertColorsChanged()
{
    QASSERT(iSettings);
    QTRACE("-" << iSettings->invertColors());
    iTheme = iSettings->invertColors() ? iThemeDefault : iThemeInverted;
    invalidatePixmaps();
    update();
}

void QuickClock::onDisplayStatusChanged(QString aStatus)
{
    QTRACE("-" << aStatus);
    setDisplayStatus(aStatus);
}

void QuickClock::onDisplayStatusQueryDone(QDBusPendingCallWatcher* aWatcher)
{
    QDBusPendingReply<QString> reply(*aWatcher);
    QTRACE("-" << reply);
    if (reply.isValid() && !reply.isError()) {
        setDisplayStatus(reply.value());
    }
    aWatcher->deleteLater();
}

void QuickClock::timerEvent(QTimerEvent* aEvent)
{
    if (aEvent->timerId() == iTimerId) {
        update();
    }
}

void QuickClock::setRunning(bool aRunning)
{
    if (aRunning && !iTimerId) {
        update();
        iTimerId = startTimer(10);
        QTRACE("timer id" << iTimerId);
    } else if (!aRunning && iTimerId) {
        killTimer(iTimerId);
        iTimerId = 0;
        QTRACE("timer off");
    }
}

void QuickClock::paintOffScreenNoSec(
    QPainter* aPainter,
    const QSize& aSize,
    const QTime& aTime)
{
    if (!iDialPlate || iDialPlate->size() != aSize) {
        if (iDialPlate) delete iDialPlate;
        iDialPlate = new QPixmap(aSize);

        QDEBUG("drawing dial plate");
        QRectF rect(QPoint(0,0), aSize);
        QPainter painter(iDialPlate);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(rect, QBrush(Qt::transparent));
        aPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);

        iRenderer->paintDialPlate(&painter, aSize, iTheme, iDrawBackground);
    }

    aPainter->drawPixmap(0, 0, *iDialPlate);

    aPainter->save();
    aPainter->setRenderHint(QPainter::Antialiasing);
    aPainter->setRenderHint(QPainter::HighQualityAntialiasing);
    aPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    iRenderer->paintHourMinHands(aPainter, aSize, aTime, iTheme);
    aPainter->restore();
}

void QuickClock::paint(QPainter* aPainter)
{
    QDateTime current = QDateTime::currentDateTime();
    QTime time = current.time();
    //QTRACE("- rendering" << qPrintable(time.toString("hh:mm:ss.zzz")));

    QRect backRect(0, 0, width(), height());
    QSize size = backRect.size();

    aPainter->save();

    if (Q_UNLIKELY(time.second() == 0)) {
        QTime t1 = time.addSecs(-1);
        QTime t2(t1.hour(), t1.minute(), 60*t1.msec()/1000);
        QTRACE("- drawing hour and minute hands (live)" <<
            qPrintable(t2.toString("hh:mm:ss.zzz")));
        paintOffScreenNoSec(aPainter, size, t2);
    } else {
        if (iOffScreenNoSec == NULL ||
            iOffScreenNoSec->size() != size ||
            time.minute() != iPaintTimeNoSec.minute() ||
            time.hour() != iPaintTimeNoSec.hour()) {

            if (iOffScreenNoSec) delete iOffScreenNoSec;
            iOffScreenNoSec = new QPixmap(size);

            QPainter painter(iOffScreenNoSec);
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            painter.fillRect(backRect, QBrush(Qt::transparent));
            QTime t1(time.hour(), time.minute(), 0);
            QTRACE("- drawing hour and minute hands" <<
                qPrintable(t1.toString("hh:mm:ss.zzz")));
            paintOffScreenNoSec(&painter, size, t1);

            iPaintTimeNoSec = time;
        }
        aPainter->drawPixmap(0, 0, *iOffScreenNoSec);
    }

    aPainter->setRenderHint(QPainter::Antialiasing);
    aPainter->setRenderHint(QPainter::HighQualityAntialiasing);
    aPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    iRenderer->paintSecHand(aPainter, size, time, iTheme);
    aPainter->restore();

#if CLOCK_PERFORMANCE_LOG
    iRenderCount++;

#  if CLOCK_DEBUG
    const int ms = iStartTime.msecsTo(current);
    if (ms >= 1000) {
        if (iRenderCount > 0) {
            QTRACE(iRenderCount*1000.0/ms << "frames per second");
            iRenderCount = 0;
        }
        iStartTime = current;
    }
#  endif // CLOCK_DEBUG
#endif // CLOCK_PERFORMANCE_LOG
}
