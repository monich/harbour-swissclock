/*
  Copyright (C) 2014-2015 Jolla Ltd.
  Contact: Slava Monich <slava.monich@jolla.com>

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS
  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
  THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ClockRenderer.h"

class DeutscheBahn : public ClockRenderer
{
public:
    virtual void paintDialPlate(QPainter* aPainter, const QSize& aSize,
        ClockTheme* aTheme, bool aDrawBackground);
    virtual void paintHourMinHands(QPainter* aPainter, const QSize& aSize,
        const QTime& aTime, ClockTheme* aTheme);
    virtual void paintSecHand(QPainter* aPainter, const QSize& aSize,
        const QTime& aTime, ClockTheme* aTheme);
    static void paintHand(QPainter* aPainter, const QRectF& aRect,
        qreal aAngle, const QBrush& aBrush,
        qreal aX = 0.0, qreal aY = 0.0);

    DeutscheBahn() : ClockRenderer("DeutscheBahn"),
        iSecondHandColor(192,48,48) {}
private:
    QColor iSecondHandColor;
};

ClockRenderer*
ClockRenderer::newDeutscheBahn()
{
    return new DeutscheBahn;
}

void
DeutscheBahn::paintDialPlate(
    QPainter* aPainter,
    const QSize& aSize,
    ClockTheme* aTheme,
    bool aDrawBackground)
{
    const qreal w = aSize.width();
    const qreal h = aSize.height();
    const qreal d = qMin(w, h);

    aPainter->save();
    aPainter->translate(w/2, h/2);

    // Draw the dial plate
    QPointF center(0,0);
    if (aDrawBackground) {
        aPainter->setPen(Qt::NoPen);
        aPainter->setBrush(QBrush(aTheme->iBackgroundColor));
        aPainter->drawEllipse(center, d/2, d/2);
    }

    const qreal y = qMax(d * qreal(0.02), qreal(1));
    const qreal y1 = qMax(d * qreal(0.008), qreal(1));
    const qreal x1 = d * 0.34;
    const qreal x2 = d * 0.364;
    const qreal x3 = d * 0.445;
    const qreal x4 = d * 0.48;
    QRectF minMarkRect(x3, -y1, (x4-x3), 2*y1);
    QRectF hourMarkRect1(x2, -y, (x4-x2), 2*y);
    QRectF hourMarkRect2(x1, -y, (x4-x1), 2*y);
    QBrush markBrush(aTheme->iHourMinHandColor);

    aPainter->setPen(aTheme->iHourMinHandColor);
    for (int i=0; i<60; i++) {
        if (i % 5) {
            aPainter->fillRect(minMarkRect, markBrush);
        } else if (i % 15) {
            aPainter->fillRect(hourMarkRect1, markBrush);
        } else {
            aPainter->fillRect(hourMarkRect2, markBrush);
        }
        aPainter->rotate(6.0);
    }
    aPainter->restore();
}

void
DeutscheBahn::paintHand(
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

void
DeutscheBahn::paintHourMinHands(
    QPainter* aPainter,
    const QSize& aSize,
    const QTime& aTime,
    ClockTheme* aTheme)
{
    // Draw hands
    const qreal d = qMin(aSize.width(),aSize.height());
    const qreal ym = qMax(d * qreal(0.018), qreal(1));
    const qreal yh = qMax(d * qreal(0.028), qreal(2));
    const qreal xm = d * 0.466;
    const qreal xh = d * 0.3;
    QRectF hourHandRect(0, -yh, xh, 2*yh);
    QRectF minHandRect(0, -ym, xm, 2*ym);
    QBrush handBrush(aTheme->iHourMinHandColor);
    QBrush shadowBrush1(aTheme->iHandShadowColor1);
    QBrush shadowBrush2(aTheme->iHandShadowColor2);

    qreal hourAngle = 30.0 * (aTime.hour() +
        (aTime.minute() + aTime.second()/60.0)/60.0) - 90;
    qreal minAngle= 6.0 * (aTime.minute() +
        (aTime.second() + aTime.msec()/1000.0)/60.0) - 90;

    QRectF rect(QPoint(0,0), aSize);
    aPainter->save();
    aPainter->translate(rect.center());
    aPainter->setPen(aTheme->iHourMinHandColor);
    paintHand(aPainter, hourHandRect, hourAngle, handBrush);
    paintHand(aPainter, minHandRect, minAngle, shadowBrush2, -1, -1);
    paintHand(aPainter, minHandRect, minAngle, shadowBrush2, 2, 2);
    paintHand(aPainter, minHandRect, minAngle, shadowBrush1, 1, 1);
    paintHand(aPainter, minHandRect, minAngle, handBrush);
    aPainter->restore();
}

void
DeutscheBahn::paintSecHand(
    QPainter* aPainter,
    const QSize& aSize,
    const QTime& aTime,
    ClockTheme* aTheme)
{
    const qreal w = aSize.width();
    const qreal h = aSize.height();
    const qreal d = qMin(w, h);

    QPointF center(0,0);

    // Draw the second hand
    const qreal x1 = d * 0.227;
    const qreal x2 = d * 0.28;
    const qreal x3 = d * 0.332;
    const qreal x4 = d * 0.486;
    const qreal y1 = qMax(d * qreal(0.011), qreal(2));
    const qreal y2 = qMax(d * qreal(0.009), qreal(1));
    const qreal y3 = qMax(d * qreal(0.008), qreal(1));
    const qreal y4 = qMax(d * qreal(0.0065), qreal(1));
    const qreal r = qMax(d * qreal(0.052), qreal(2));
    const qreal dr = qMax(d * qreal(0.015), qreal(1));

    QPainterPath p1;
    p1.moveTo(0, y1);
    p1.lineTo(x1, y2);
    p1.lineTo(x1, -y2);
    p1.lineTo(0, -y1);
    p1.closeSubpath();

    QPainterPath p2;
    p2.moveTo(x3, y3);
    p2.lineTo(x4, y4);
    p2.lineTo(x4, -y4);
    p2.lineTo(x3, -y3);
    p2.closeSubpath();

    aPainter->save();
    aPainter->setPen(Qt::NoPen);
    aPainter->translate(w/2, h/2);
    aPainter->rotate(6.0 * (aTime.second() + aTime.msec()/1000.0) - 90);

    QBrush shadowBrush(aTheme->iDialShadowColor);
    QPen shadowPen(shadowBrush, dr);
    aPainter->setPen(Qt::NoPen);
    aPainter->setBrush(shadowBrush);
    aPainter->translate(1, 1);
    aPainter->drawPath(p1);
    aPainter->drawPath(p2);
    aPainter->setBrush(Qt::NoBrush);
    aPainter->setPen(shadowPen);
    aPainter->drawEllipse(QPointF(x2,0), r, r);

    QBrush secBrush(iSecondHandColor);
    QPen secPen(secBrush, dr);
    aPainter->setPen(Qt::NoPen);
    aPainter->setBrush(secBrush);
    aPainter->translate(-1, -1);
    aPainter->drawPath(p1);
    aPainter->drawPath(p2);
    aPainter->setBrush(Qt::NoBrush);
    aPainter->setPen(secPen);
    aPainter->drawEllipse(QPointF(x2,0), r, r);

    aPainter->setPen(Qt::NoPen);
    aPainter->setBrush(QBrush(aTheme->iHandShadowColor1));
    aPainter->setPen(aTheme->iHandShadowColor1);
    aPainter->drawEllipse(center, r+1, r+1);

    aPainter->setBrush(QBrush(aTheme->iHourMinHandColor));
    aPainter->setPen(aTheme->iHourMinHandColor);
    aPainter->drawEllipse(center, r, r);

    aPainter->restore();
}
