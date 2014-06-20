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

#include <QDBusConnection>
#include <QPainter>

#define SUPER QQuickPaintedItem

#define SETTING_INVERT_COLORS "invertColors"
#define SETTING_DRAW_BACKGROUND "drawBackground"

QuickClock::QuickClock(QQuickItem* aParent) :
    SUPER(),
    iDrawBackground(true),
    iSettings(NULL),
    iDialPlate(NULL),
    iOffScreenNoSec(NULL),
    iTimerId(0)
{
    QTRACE("- created");
    updateColors();
#if CLOCK_PERFORMANCE_LOG
    iRenderCount = 0;
    iStartTime = QDateTime::currentDateTime();
#endif // CLOCK_PERFORMANCE_LOG
    setRunning(true);
    QDBusConnection::systemBus().connect("com.nokia.mce",
         "/com/nokia/mce/signal", "com.nokia.mce.signal",
         "display_status_ind", this, SLOT(onDisplayStatusChanged(QString)));
}

QuickClock::~QuickClock()
{
    QTRACE("- destroyed");
    if (iDialPlate) delete iDialPlate;
    if (iOffScreenNoSec) delete iOffScreenNoSec;
}

void QuickClock::invalidPixmaps()
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
        invalidPixmaps();
        drawBackgroundChanged(aValue);
        update();
    }
}

void QuickClock::setSettings(ClockSettings* aSettings)
{
    if (iSettings != aSettings) {
        if (iSettings) iSettings->disconnect(this);
        iSettings = aSettings;
        if (iSettings) {
            connect(iSettings,SIGNAL(invertColorsChanged(bool)),
                SLOT(onInvertColorsChanged(bool)));
        }
        settingsChanged(iSettings);
        invalidPixmaps();
        updateColors();
        update();
    }
}

void QuickClock::updateColors()
{
    if (iSettings && iSettings->invertColors()) {
        iBackgroundColor.setRgb(228,228,228);
        iHourMinArmColor.setRgb(43,43,43);
        iArmShadowColor1.setRgb(228,228,228,0x40);
        iArmShadowColor2.setRgb(228,228,228,0x80);
    } else {
        iBackgroundColor.setRgb(43,43,43);
        iHourMinArmColor.setRgb(228,228,228);
        iArmShadowColor1.setRgb(43,43,43,0x80);
        iArmShadowColor2.setRgb(43,43,43,0x40);
    }
}

void QuickClock::onInvertColorsChanged(bool aValue)
{
    invalidPixmaps();
    updateColors();
    update();
}

void QuickClock::onDisplayStatusChanged(QString aStatus)
{
    QTRACE("-" << aStatus);
    setRunning(aStatus != "off");
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

void QuickClock::paintDialPlate(const QSize& aSize)
{
    if (!iDialPlate || iDialPlate->size() != aSize) {
        if (iDialPlate) delete iDialPlate;
        iDialPlate = new QPixmap(aSize);

        QDEBUG("drawing dial plate");
        QRectF rect(QPoint(0,0), aSize);
        QPainter painter(iDialPlate);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.translate(rect.center());

        QPointF center(0,0);

        // Draw the dial plate
        const qreal d = dialPlateSize();
        if (iDrawBackground) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(iBackgroundColor));
            painter.drawEllipse(center, d/2, d/2);
        }

#if 0
        // Flag
        painter.save();
        const qreal f = d/50;
        const qreal wf = 7*f;
        const qreal hf = 5*f;
        painter.translate(-wf/2, -d/4 + hf/2);
        QColor flagColor1(145,60,53);
        QColor flagColor2(166,166,166);
        painter.setPen(Qt::NoPen);
        painter.fillRect(QRectF(0,0,7*f,5*f), QBrush(flagColor1));
        painter.fillRect(QRectF(2*f,2*f,3*f,f), QBrush(flagColor2));
        painter.fillRect(QRectF(3*f,f,f,3*f), QBrush(flagColor2));
        painter.restore();
#endif

        const qreal y = d / 50;
        const qreal y1 = qMax((qreal)1, d / 158);
        const qreal x1 = d * 10 / 27;
        const qreal x2 = d * 10 / 21;
        const qreal x = (x1+x2)/2;
        QRectF hourMarkRect(x1, -y, (x2-x1), 2*y);
        QRectF minMarkRect(x, -y1, (x2-x), 2*y1);
        QBrush markBrush(iHourMinArmColor);

        painter.save();
        painter.setPen(iHourMinArmColor);
        for (int i=0; i<60; i++) {
            if (i % 5) {
                painter.fillRect(minMarkRect, markBrush);
            } else {
                painter.fillRect(hourMarkRect, markBrush);
            }
            painter.rotate(6.0);
        }
        painter.restore();
    }
}

void QuickClock::paintSimpleHand(
    QPainter* aPainter,
    const QRectF& aRect,
    qreal aAngle,
    const QBrush& aBrush,
    qreal aX,
    qreal aY)
{
    aPainter->save();
    aPainter->translate(aX,aY);
    aPainter->rotate(aAngle);
    aPainter->fillRect(aRect, aBrush);
    aPainter->restore();
}

void QuickClock::paintOffScreenNoSec(
    const QSize& aSize,
    const QTime& aTime)
{
    QTRACE("- drawing hour and minute arms");

    if (!iOffScreenNoSec || iOffScreenNoSec->size() != aSize) {
        if (iOffScreenNoSec) delete iOffScreenNoSec;
        iOffScreenNoSec = new QPixmap(aSize);
    }

    QPainter painter(iOffScreenNoSec);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    paintOffScreenNoSec(&painter, aSize, aTime);
    iPaintTimeNoSec = aTime;
}

void QuickClock::paintOffScreenNoSec(
    QPainter* aPainter,
    const QSize& aSize,
    const QTime& aTime)
{
    // Draw arms
    const qreal d = dialPlateSize();
    const qreal y = d / 50;
    const qreal x1 = d * 10 / 27;
    const qreal x2 = d * 10 / 21;
    const qreal xh1 = -(d * 10 / 74);
    const qreal xh2 = (d * 10 / 36);
    const qreal xm1 = xh1;
    const qreal xm2 = (x1+x2)/2;
    QRectF hourArmRect(xh1, -y, (xh2-xh1), 2*y);
    QRectF minArmRect(xm1, -y, (xm2-xm1), 2*y);
    QBrush armBrush(iHourMinArmColor);
    QBrush shadowBrush1(iArmShadowColor1);
    QBrush shadowBrush2(iArmShadowColor2);

    qreal hourAngle = 30.0 * ((aTime.hour() + aTime.minute()/60.0)) - 90;
    qreal minAngle;
    if (aTime.second()) {
        minAngle = aTime.minute();
    } else {
        minAngle = aTime.minute() - 1 + aTime.msec()/1000.0;
    }
    minAngle = 6.0 * minAngle - 90;

    QRectF rect(QPoint(0,0), aSize);
    paintDialPlate(aSize);
    aPainter->save();
    aPainter->setCompositionMode(QPainter::CompositionMode_Source);
    aPainter->fillRect(rect, QBrush(Qt::transparent));
    aPainter->drawPixmap(0, 0, *iDialPlate);
    aPainter->translate(rect.center());
    aPainter->setPen(iHourMinArmColor);
    paintSimpleHand(aPainter, hourArmRect, hourAngle, armBrush);
    paintSimpleHand(aPainter, minArmRect, minAngle, shadowBrush2, -1, -1);
    paintSimpleHand(aPainter, minArmRect, minAngle, shadowBrush2, 2, 2);
    paintSimpleHand(aPainter, minArmRect, minAngle, shadowBrush1, 1, 1);
    paintSimpleHand(aPainter, minArmRect, minAngle, armBrush);
    aPainter->restore();
}

void QuickClock::paint(QPainter* aPainter)
{
    QDateTime current = QDateTime::currentDateTime();
    QTime time = current.time();
    //QTRACE("- rendering" << qPrintable(time.toString("hh:mm:ss.zzz")));

    QRect backRect(0, 0, width(), height());
    QSize size = backRect.size();

    aPainter->setRenderHint(QPainter::Antialiasing);
    aPainter->setRenderHint(QPainter::HighQualityAntialiasing);

    if (time.second() == 0) {
        paintOffScreenNoSec(aPainter, size, time);
    } else {
        if (!iOffScreenNoSec || iOffScreenNoSec->size() != size ||
            time.minute() != iPaintTimeNoSec.minute() ||
            time.hour() != iPaintTimeNoSec.hour()) {
            paintOffScreenNoSec(size, time);
        }
        aPainter->drawPixmap(0, 0, *iOffScreenNoSec);
    }

    aPainter->translate(backRect.center());

    // Seconds arm colors don't depend on the theme
    QColor red(Qt::red);
    QColor black(Qt::black);
    QColor white(Qt::white);
    QPointF center(0,0);

    // Draw arms
    const qreal d = dialPlateSize();
    const qreal y = qMax(d / 50, (qreal)5);
    const qreal x1 = d * 10 / 27;
    const qreal xh1 = -(d * 10 / 74);

    const qreal rs1 = y-1;
    const qreal rs2 = d/26;
    const qreal ds = rs1-2;
    const qreal xs1 = xh1 + d / 140;
    const qreal xs2 = x1 - d / 50 - rs2;
    QRectF secArmRect(xs1, -ds/2.0, (xs2-xs1), ds);
    QBrush secBrush(red);

    aPainter->save();
    aPainter->setPen(Qt::NoPen);
    aPainter->setBrush(secBrush);
    aPainter->rotate(6.0 * (time.second() + time.msec()/1000.0) - 90);
    aPainter->fillRect(secArmRect, secBrush);
    aPainter->drawEllipse(center, rs1, rs1);
    aPainter->drawEllipse(QPointF(xs2,0), rs2, rs2);
    aPainter->setBrush(QBrush(white));
    aPainter->drawEllipse(center, 2, 2);
    aPainter->setBrush(QBrush(black));
    aPainter->drawEllipse(center, 1, 1);
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
