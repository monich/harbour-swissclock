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

#include "ClockRenderer.h"
#include "ClockDebug.h"

#include <QPainterPath>
#include <math.h>

const QString ClockRenderer::HELSINKI_METRO("HelsinkiMetro");

class HelsinkiMetro : public ClockRenderer
{
public:
    HelsinkiMetro();
    ~HelsinkiMetro() Q_DECL_OVERRIDE;

    void paintDialPlate(QPainter* aPainter, const QSize& aSize,
        ClockTheme* aTheme, bool aDrawBackground) Q_DECL_OVERRIDE;
    void paintHourMinHands(QPainter* aPainter, const QSize& aSize,
        const QTime& aTime, ClockTheme* aTheme) Q_DECL_OVERRIDE;
    void paintSecHand(QPainter* aPainter, const QSize& aSize,
        const QTime& aTime, ClockTheme* aTheme) Q_DECL_OVERRIDE;
    void initNode(QSGTransformNode* aTxNode, NodeType aType,
        QQuickWindow* aWindow, const QSizeF& aSize,
        ClockTheme* aTheme) Q_DECL_OVERRIDE;

    void initHour(QSGTransformNode*, const QSizeF&, ClockTheme*);
    void initMin(QSGTransformNode*, const QSizeF&, ClockTheme*);
    void initSec(QSGTransformNode*, QQuickWindow*, const QSizeF&);

    static QSGNode* handNode(const QRectF&, const QColor&);
    static void paintHand(QPainter* aPainter, const QRectF& aRect,
        qreal aAngle, const QBrush& aBrush, qreal aX = 0.0, qreal aY = 0.0);

    const QColor iSecondHandColor;
    const QBrush iSecondHandBrush;
    const QBrush iBlack;
    const QBrush iWhite;
    QPolygonF iPolygon;
    QSizeF iSize;
    QQuickWindow* iWindow;
    QPixmap* iCenterDiskPixmap;
};

ClockRenderer*
ClockRenderer::newHelsinkiMetro()
{
    return new HelsinkiMetro;
}

HelsinkiMetro::HelsinkiMetro() :
    ClockRenderer(HELSINKI_METRO),
    iSecondHandColor(255, 0, 0),
    iSecondHandBrush(iSecondHandColor, Qt::SolidPattern),
    iBlack(Qt::black),
    iWhite(Qt::white),
    iWindow(NULL),
    iCenterDiskPixmap(NULL)
{
}

HelsinkiMetro::~HelsinkiMetro()
{
    if (iCenterDiskPixmap) delete iCenterDiskPixmap;
}

void
HelsinkiMetro::paintDialPlate(
    QPainter* aPainter,
    const QSize& aSize,
    ClockTheme* aTheme,
    bool aDrawBackground)
{
    const qreal w = aSize.width();
    const qreal h = aSize.height();
    const qreal d = qMin(w, h);
    const qreal r1 = d / 2;
    const qreal r2 = r1 - qMax(qreal(1), qreal(d / 98));
    const qreal r3 = d / 16.25;

    aPainter->save();
    aPainter->translate(w/2, h/2);

    // Draw the dial plate
    QPointF center(0,0);
    if (aDrawBackground) {
        aPainter->setPen(Qt::NoPen);
        aPainter->setBrush(QBrush(aTheme->iBackgroundColor2));
        aPainter->drawEllipse(center, r1, r1);
        aPainter->setBrush(QBrush(aTheme->iBackgroundColor));
        aPainter->drawEllipse(center, r2, r2);
        aPainter->setBrush(QBrush(aTheme->iBackgroundColor1));
        aPainter->drawEllipse(center, r3, r3);
    }

    const qreal y = d / 84;
    const qreal y1 = qMax(qreal(1), qreal(d / 195));
    const qreal x1 = d / 2.71;
    const qreal x2 = d / 2.25;
    const qreal x3 = d / 2.1;
    QRectF hourMarkRect(x1, -y, (x3-x1), 2*y);
    QRectF minMarkRect(x2, -y1, (x3-x2), 2*y1);
    QBrush markBrush(aTheme->iHourMinHandColor);

    aPainter->setPen(aTheme->iHourMinHandColor);
    for (int i=0; i<60; i++) {
        if (i % 5) {
            aPainter->fillRect(minMarkRect, markBrush);
        } else {
            aPainter->fillRect(hourMarkRect, markBrush);
        }
        aPainter->rotate(6.0);
    }
    aPainter->restore();
}

void
HelsinkiMetro::paintHand(
    QPainter* aPainter,
    const QRectF& aRect,
    qreal aAngle,
    const QBrush& aBrush,
    qreal aX,
    qreal aY)
{
    const qreal x = aRect.right() - aRect.height() / 3.0;

    QPainterPath path;
    path.moveTo(aRect.left(), aRect.bottom());
    path.lineTo(aRect.left(), aRect.top());
    path.lineTo(x, aRect.top());
    path.lineTo(aRect.right(), (aRect.top() + aRect.bottom())/2.0);
    path.lineTo(x, aRect.bottom());
    path.closeSubpath();

    aPainter->save();
    aPainter->translate(aX,aY);
    aPainter->rotate(aAngle);
    aPainter->setPen(Qt::NoPen);
    aPainter->setBrush(aBrush);
    aPainter->drawPath(path);
    aPainter->restore();
}

void
HelsinkiMetro::paintHourMinHands(
    QPainter* aPainter,
    const QSize& aSize,
    const QTime& aTime,
    ClockTheme* aTheme)
{
    // Draw hands
    const qreal d = qMin(aSize.width(),aSize.height());
    const qreal x = d / 2 - qMax(qreal(1), qreal(d / 98));
    const qreal yh = d / 31;
    const qreal ym = d / 50;
    const qreal xh1 = -(d / 8.6);
    const qreal xh2 = d / 3.4;
    const qreal xm1 = xh1;
    const qreal xm2 = qMin(qreal(d / 2.03), x - 2);
    QRectF hourHandRect(xh1, -yh, (xh2-xh1), 2*yh);
    QRectF minHandRect(xm1, -ym, (xm2-xm1), 2*ym);
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
HelsinkiMetro::paintSecHand(
    QPainter* aPainter,
    const QSize& aSize,
    const QTime& aTime,
    ClockTheme* aTheme)
{
    const qreal w = aSize.width();
    const qreal h = aSize.height();
    const qreal d = qMin(w, h);
    const int r = qMax((int)(d / 23), 3);

    if (iSize != aSize) {
        iSize = aSize;
        iPolygon.clear();
    }

    if (iPolygon.isEmpty()) {
        const qreal y = qMax(qreal(1), qreal(d / 195));
        const qreal x1 = -(d / 6.97);
        const qreal x2 = d / 2;

        QTRACE("- new path");
        iSize = aSize;
        QPointF origin(x1, 0);
        iPolygon.clear();
        iPolygon.append(origin);
        iPolygon.append(QPointF(x1+y, -y));
        iPolygon.append(QPointF(x2-y, -y));
        iPolygon.append(QPointF(x2, 0));
        iPolygon.append(QPointF(x2-y, y));
        iPolygon.append(QPointF(x1+y, y));
        iPolygon.append(origin);
    }

    if (!iCenterDiskPixmap) {
        QPointF center(0,0);
        if (iCenterDiskPixmap) delete iCenterDiskPixmap;
        iCenterDiskPixmap = new QPixmap(2*(r+1), 2*(r+1));
        QPainter diskPainter(iCenterDiskPixmap);
        diskPainter.setPen(Qt::NoPen);
        diskPainter.setBrush(iSecondHandColor);
        diskPainter.setRenderHint(QPainter::Antialiasing);
        diskPainter.setRenderHint(QPainter::HighQualityAntialiasing);
        diskPainter.setCompositionMode(QPainter::CompositionMode_Source);
        diskPainter.fillRect(QRectF(0, 0, 2*(r+1), 2*(r+1)), Qt::transparent);
        diskPainter.translate(r+1, r+1);
        diskPainter.drawEllipse(center, r, r);

        const int rw = qMax((int)d/200, 2);
        const int rb = qMax((rw/2) & ~1, 1);
        diskPainter.setBrush(QBrush(Qt::white));
        diskPainter.drawEllipse(center, rw, rw);
        diskPainter.setBrush(QBrush(Qt::black));
        diskPainter.drawEllipse(center, rb, rb);
    }

    aPainter->save();
    aPainter->setPen(Qt::NoPen);
    aPainter->setBrush(iSecondHandBrush);
    aPainter->translate(w/2, h/2);
    aPainter->save();
    aPainter->rotate(6.0 * (aTime.second() + aTime.msec()/1000.0) - 90);
    aPainter->drawPolygon(iPolygon);
    aPainter->restore();
    aPainter->drawPixmap(-(r+1), -(r+1), *iCenterDiskPixmap);
    aPainter->restore();
}

QSGNode*
HelsinkiMetro::handNode(
    const QRectF& aRect,
    const QColor& aColor)
{
    const qreal x = aRect.right() - aRect.height() / 3.0;
    QSGGeometry* g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 5);
    QSGGeometry::Point2D* v = g->vertexDataAsPoint2D();
    g->setDrawingMode(GL_TRIANGLE_FAN);
    v[0].x = aRect.right(); v[0].y = aRect.top() + aRect.height()/2;
    v[1].x = x;             v[1].y = aRect.top();
    v[2].x = aRect.left();  v[2].y = v[1].y;
    v[3].x = v[2].x;        v[3].y = aRect.bottom();
    v[4].x = x;             v[4].y = v[3].y;
    return geometryNode(g, aColor);
}

void
HelsinkiMetro::initHour(
    QSGTransformNode* aTxNode,
    const QSizeF& aSize,
    ClockTheme* aTheme)
{
    const qreal w = aSize.width();
    const qreal h = aSize.height();
    const qreal d = qMin(w, h);
    const qreal yh = d / 31;
    const qreal xh1 = -(d / 8.6);
    const qreal xh2 = d / 3.4;
    QRectF hourHandRect(round(h/2)+xh1, round(h/2)-yh, (xh2-xh1), 2*yh);
    aTxNode->appendChildNode(handNode(hourHandRect, aTheme->iHourMinHandColor));
}

void
HelsinkiMetro::initMin(
    QSGTransformNode* aTxNode,
    const QSizeF& aSize,
    ClockTheme* aTheme)
{
    const qreal w = aSize.width();
    const qreal h = aSize.height();
    const qreal d = qMin(w, h);
    const qreal x = d / 2 - qMax(qreal(1), qreal(d / 98));
    const qreal y1 = d / 50;
    const qreal x1 = -(d / 8.6);
    const qreal x2 = qMin(qreal(d / 2.03), x - 3);
    const qreal dx = (x2-x1);
    const qreal dy = 2*y1;
    const qreal x0 = round(w/2)+x1;
    const qreal y0 = round(h/2)-y1;

    aTxNode->appendChildNode(handNode(QRectF(x0-2, y0-2, dx+4, dy+4),
        aTheme->iHandShadowColor2));
    aTxNode->appendChildNode(handNode(QRectF(x0-1, y0-1, dx+2, dy+2),
        aTheme->iHandShadowColor1));
    aTxNode->appendChildNode(handNode(QRectF(x0, y0, dx, dy),
        aTheme->iHourMinHandColor));
}

void
HelsinkiMetro::initSec(
    QSGTransformNode* aTxNode,
    QQuickWindow* aWindow,
    const QSizeF& aSize)
{
    const qreal w = aSize.width();
    const qreal h = aSize.height();
    const qreal d = qMin(w, h);
    const qreal rc = qMax((int)d/200, 2);
    const qreal x0 = round(w/2);
    const qreal y0 = round(h/2);
    const qreal r = qMax((int)(d / 23), 3);
    const qreal y = qMax(qreal(1), qreal(d / 195));
    const qreal x1 = x0 - (d/6.97);
    const qreal x2 = x0 + d/2;

    HDEBUG("initializing" << qPrintable(id()) << "node");
    QSGGeometry* g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(),6);
    QSGGeometry::Point2D* v = g->vertexDataAsPoint2D();
    g->setDrawingMode(GL_TRIANGLE_FAN);
    v[0].x = x1;   v[0].y = y0;
    v[1].x = x1+y; v[1].y = y0-y;
    v[2].x = x2-y; v[2].y = y0-y;
    v[3].x = x2;   v[3].y = y0;
    v[4].x = x2-y; v[4].y = y0+y;
    v[5].x = x1+y; v[5].y = y0+y;
    aTxNode->appendChildNode(geometryNode(g, iSecondHandColor));
    aTxNode->parent()->appendChildNode(centerDiskNode(aWindow,
        QPointF(x0,y0), r, rc, iSecondHandColor));
}

void
HelsinkiMetro::initNode(
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
        initSec(aTxNode, aWindow, aSize);
        break;
    default:
        HWARN("Unsupported node type" << aType);
        break;
    }
}
