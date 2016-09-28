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

#include <QPainterPath>

const QString ClockRenderer::HELSINKI_METRO("HelsinkiMetro");

class HelsinkiMetro : public ClockRenderer
{
public:
    HelsinkiMetro();
    virtual ~HelsinkiMetro();
    virtual void paintDialPlate(QPainter* aPainter, const QSize& aSize,
        ClockTheme* aTheme, bool aDrawBackground);
    virtual void paintHourMinHands(QPainter* aPainter, const QSize& aSize,
        const QTime& aTime, ClockTheme* aTheme);
    virtual void paintSecHand(QPainter* aPainter, const QSize& aSize,
        const QTime& aTime, ClockTheme* aTheme);
    virtual void initSecNode(QSGTransformNode* aTxNode, const QSizeF& aSize,
        QQuickWindow* aWindow, ClockTheme* aTheme);

    bool dropCachedItems(const QSizeF& aSize, ClockTheme* aTheme);
    static void paintHand(QPainter* aPainter, const QRectF& aRect,
        qreal aAngle, const QBrush& aBrush,
        qreal aX = 0.0, qreal aY = 0.0);

    QColor iSecondHandColor;
    QBrush iSecondHandBrush;
    QBrush iBlack;
    QBrush iWhite;
    QPolygonF iPolygon;
    QSizeF iSize;
    QQuickWindow* iWindow;
    ClockTheme* iTheme;
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
    iTheme(NULL),
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

    dropCachedItems(aSize, aTheme);

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
        diskPainter.setBrush(QBrush(Qt::white));
        diskPainter.drawEllipse(center, 2, 2);
        diskPainter.setBrush(QBrush(Qt::black));
        diskPainter.drawEllipse(center, 1, 1);
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

bool
HelsinkiMetro::dropCachedItems(
    const QSizeF& aSize,
    ClockTheme* aTheme)
{
    bool sizeChanged;
    if (iSize != aSize) {
        iSize = aSize;
        sizeChanged = true;
    } else {
        sizeChanged = false;
    }

    bool themeChanged;
    if (iTheme != aTheme) {
        iTheme = aTheme;
        themeChanged = true;
    } else {
        themeChanged = false;
    }

    return (sizeChanged || themeChanged);
}

void
HelsinkiMetro::initSecNode(
    QSGTransformNode* aTxNode,
    const QSizeF& aSize,
    QQuickWindow* aWindow,
    ClockTheme* aTheme)
{
    const qreal w = aSize.width();
    const qreal h = aSize.height();
    const qreal d = qMin(w, h);
    const qreal x0 = w/2;
    const qreal y0 = h/2;
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

    QPointF center(x0,y0);
    QSGNode* rootNode = aTxNode->parent();
    rootNode->appendChildNode(circleNode(center, r, iSecondHandColor));
    rootNode->appendChildNode(new CenterNode(aWindow, center));
}
