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

#include "ClockRenderer.h"
#include "ClockDebug.h"

#include <math.h>

const QString ClockRenderer::DEUTSCHE_BAHN("DeutscheBahn");

class DeutscheBahn : public ClockRenderer
{
public:
    virtual void paintDialPlate(QPainter* aPainter, const QSize& aSize,
        ClockTheme* aTheme, bool aDrawBackground);
    virtual void paintHourMinHands(QPainter* aPainter, const QSize& aSize,
        const QTime& aTime, ClockTheme* aTheme);
    virtual void paintSecHand(QPainter* aPainter, const QSize& aSize,
        const QTime& aTime, ClockTheme* aTheme);
    virtual void initNode(QSGTransformNode* aTxNode, NodeType aType,
        QQuickWindow* aWindow, const QSizeF& aSize, ClockTheme* aTheme);

    void initHour(QSGTransformNode* aTxNode, const QSizeF& aSize,
        ClockTheme* aTheme);
    void initMin(QSGTransformNode* aTxNode, const QSizeF& aSize,
        ClockTheme* aTheme);
    void initSec(QSGTransformNode* aTxNode, const QSizeF& aSize,
        ClockTheme* aTheme);
    static void paintHand(QPainter* aPainter, const QRectF& aRect,
        qreal aAngle, const QBrush& aBrush, qreal aX = 0.0, qreal aY = 0.0);

    DeutscheBahn() : ClockRenderer(DEUTSCHE_BAHN),
        iSecondHandColor(255,32,32) {}

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
    const qreal yh = qMax(d * qreal(0.024), qreal(2));
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

    QBrush secBrush(iSecondHandColor);
    QPen secPen(secBrush, dr);
    aPainter->setPen(Qt::NoPen);
    aPainter->setBrush(secBrush);
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

void
DeutscheBahn::initHour(
    QSGTransformNode* aTxNode,
    const QSizeF& aSize,
    ClockTheme* aTheme)
{
    const qreal w = aSize.width();
    const qreal h = aSize.height();
    const qreal d = qMin(w, h);
    const qreal yh = round(qMax(d * qreal(0.024), qreal(2)));
    const qreal xh = round(d * 0.3);
    QRectF rect(round(w/2), round(h/2)-yh, xh, 2*yh);
    aTxNode->appendChildNode(rectNode(rect, aTheme->iHourMinHandColor));
}

void
DeutscheBahn::initMin(
    QSGTransformNode* aTxNode,
    const QSizeF& aSize,
    ClockTheme* aTheme)
{
    const qreal w = aSize.width();
    const qreal h = aSize.height();
    const qreal d = qMin(w, h);
    const qreal ym = round(qMax(d * qreal(0.018), qreal(1)));
    const qreal xm = round(d * 0.466);
    const qreal x1 = round(w/2);
    const qreal y1 = round(h/2)-ym;
    aTxNode->appendChildNode(rectNode(QRectF(x1, y1-2, xm+2, 2*ym+4),
        aTheme->iHandShadowColor2));
    aTxNode->appendChildNode(rectNode(QRectF(x1, y1-1, xm+1, 2*ym+2),
        aTheme->iHandShadowColor1));
    aTxNode->appendChildNode(rectNode(QRectF(x1, y1, xm, 2*ym),
        aTheme->iHourMinHandColor));
}

void
DeutscheBahn::initSec(
    QSGTransformNode* aTxNode,
    const QSizeF& aSize,
    ClockTheme* aTheme)
{
    const qreal w = aSize.width();
    const qreal h = aSize.height();
    const qreal d = qMin(w, h);
    const qreal x0 = round(w/2);
    const qreal y0 = round(h/2);

    HDEBUG("initializing" << qPrintable(id()) << "node");

    // Draw the second hand
    const qreal x1 = d * 0.23;
    const qreal x2 = d * 0.28;
    const qreal x3 = d * 0.33;
    const qreal x4 = d * 0.486;
    const qreal y1 = qMax(d * qreal(0.011), qreal(2));
    const qreal y2 = qMax(d * qreal(0.009), qreal(1));
    const qreal y3 = qMax(d * qreal(0.008), qreal(1));
    const qreal y4 = qMax(d * qreal(0.0065), qreal(1));
    const qreal r1 = qMax(d * qreal(0.052), qreal(2));
    const qreal dr = qMax(d * qreal(0.015), qreal(1));

    QSGGeometry* g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(),4);
    QSGGeometry::Point2D* v = g->vertexDataAsPoint2D();
    g->setDrawingMode(GL_TRIANGLE_FAN);
    v[0].x = x0;     v[0].y = y0+y1;
    v[1].x = x0+x1;  v[1].y = y0+y2;
    v[2].x = v[1].x; v[2].y = y0-y2;
    v[3].x = v[0].x; v[3].y = y0-y1;
    aTxNode->appendChildNode(geometryNode(g, iSecondHandColor));

    g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(),4);
    v = g->vertexDataAsPoint2D();
    g->setDrawingMode(GL_TRIANGLE_FAN);
    v[0].x = x0+x3;  v[0].y = y0+y3;
    v[1].x = x0+x4;  v[1].y = y0+y4;
    v[2].x = v[1].x; v[2].y = y0-y4;
    v[3].x = v[0].x; v[3].y = y0-y3;
    aTxNode->appendChildNode(geometryNode(g, iSecondHandColor));
    aTxNode->appendChildNode(ringNode(QPointF(x0+x2,y0), r1+dr/2, dr, iSecondHandColor));

    QPointF center(x0,y0);
    QSGNode* rootNode = aTxNode->parent();
    rootNode->appendChildNode(circleNode(center, r1+2, aTheme->iHandShadowColor2));
    rootNode->appendChildNode(circleNode(center, r1+1, aTheme->iHandShadowColor1));
    rootNode->appendChildNode(circleNode(center, r1, aTheme->iHourMinHandColor));
}

void
DeutscheBahn::initNode(
    QSGTransformNode* aTxNode,
    NodeType aType,
    QQuickWindow* aWindow,
    const QSizeF& aSize,
    ClockTheme* aTheme)
{
    switch (aType) {
    case NodeHour:
        initHour(aTxNode, aSize, aTheme);
        break;
    case NodeMin:
        initMin(aTxNode, aSize, aTheme);
        break;
    case NodeSec:
        initSec(aTxNode, aSize, aTheme);
        break;
    default:
        HWARN("Unsupported node type" << aType);
        break;
    }
}
