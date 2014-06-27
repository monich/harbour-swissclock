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

#ifndef QUICK_CLOCK_H
#define QUICK_CLOCK_H

#include "ClockSettings.h"

#include <QQuickPaintedItem>
#include <QPixmap>
#include <QDateTime>

class QuickClock: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(bool drawBackground READ drawBackground WRITE setDrawBackground NOTIFY drawBackgroundChanged)
    Q_PROPERTY(ClockSettings* settings READ settings WRITE setSettings NOTIFY settingsChanged)

public:
    explicit QuickClock(QQuickItem* aParent = NULL);
    ~QuickClock();

    ClockSettings* settings() const { return iSettings; }
    void setSettings(ClockSettings* aSettings);

    bool drawBackground() const { return iDrawBackground; }
    void setDrawBackground(bool aValue);

signals:
    void drawBackgroundChanged(bool aValue);
    void settingsChanged(ClockSettings* aValue);

protected:
    virtual void paint(QPainter* aPainter);
    virtual void timerEvent(QTimerEvent* aEvent);

private:
    void invalidPixmaps();
    void updateColors();
    void setRunning(bool aRunning);
    void paintDialPlate(const QSize& aSize);
    void paintOffScreenNoSec(QPainter* aPainter, const QSize& aSize,
         const QTime& aTime);
    qreal dialPlateSize() const { return qMin(width(),height()); }
    static void paintSimpleHand(QPainter* aPainter, const QRectF& aRect,
        qreal aAngle, const QBrush& aBrush,
        qreal aX = 0.0, qreal aY = 0.0);

private slots:
    void onDisplayStatusChanged(QString);
    void onInvertColorsChanged(bool);

private:
    QColor iBackgroundColor;
    QColor iHourMinArmColor;
    QColor iArmShadowColor1;
    QColor iArmShadowColor2;
    bool iDrawBackground;
    ClockSettings* iSettings;
    QPixmap* iDialPlate;
    QPixmap* iOffScreenNoSec;   // Everything except seconds
    QTime iPaintTimeNoSec;      // When iOffScreenNoSec was painted
    int iTimerId;

#define CLOCK_PERFORMANCE_LOG 0
#if CLOCK_PERFORMANCE_LOG
    QDateTime iStartTime;
    int iRenderCount;
#endif // CLOCK_PERFORMANCE_LOG
};

QML_DECLARE_TYPE(QuickClock)

#endif // QUICK_CLOCK_H
