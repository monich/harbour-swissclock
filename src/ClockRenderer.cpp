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

#include <qmath.h>
#include <QSGFlatColorMaterial>

ClockRenderer::ImageNode::ImageNode(
    QQuickWindow* aWindow,
    qreal aX,
    qreal aY,
    QImage aImage)
{
    setRect(aX, aY, aImage.width(), aImage.height());
    setTexture(aWindow->createTextureFromImage(aImage));
}

ClockRenderer::ImageNode::~ImageNode()
{
    delete texture();
}

ClockRenderer::~ClockRenderer()
{
}

int
ClockRenderer::msecUntilNextUpdate(
    NodeType aType,
    const QTime& aTime)
{
    if (aType == NodeSec || aTime.second() == 0) {
        return QUICK_CLOCK_MIN_UPDATE_INTERVAL;
    } else {
        // 59 seconds out of 60 hour and minute handls are not changing
        // positions. Make this timer fire one second eariler so that we
        // could aim better.
        const int msec = ((58 - aTime.second()) * 1000) + 1000 - aTime.msec();
        return qMax(msec, QUICK_CLOCK_MIN_UPDATE_INTERVAL);
    }
}

QMatrix4x4
ClockRenderer::nodeMatrix(
    NodeType aType,
    const QSize& aSize,
    const QTime& aTime)
{
    qreal a;

    if (aType == NodeSec) {
        if (!ClockRenderer::SWISS_RAILROAD.isNull()) {
            qreal hilfiker = 58.5;
            if (aTime.second() + aTime.msec()/1000.0 >= hilfiker ) {
                a = - 90; // wait at 0 a.k.a. 60 a.k.a. 90
            } else { // speed up seconds hand
                a = 6 * (aTime.second() + aTime.msec()/1000.0)*60/hilfiker - 90;
            }
        } else {
            a = 6 * (aTime.second() + aTime.msec()/1000.0) - 90;
        }
    } else {
        QTime t;
        if (aTime.second() == 0) {
            QTime t1 = aTime.addSecs(-1);
            t = QTime(t1.hour(), t1.minute(), 60*t1.msec()/1000);
        } else {
            t = QTime(aTime.hour(), aTime.minute(), 0);
        }
        if (aType == NodeHour) {
            a = 30*(t.hour() + (t.minute() + t.second()/60.0)/60) - 90;
        } else {
            a = 6*(t.minute() + (t.second() + t.msec()/1000.0)/60) - 90;
        }
    }

    qreal dx = aSize.width()/2;
    qreal dy = aSize.height()/2;
    return QMatrix4x4(QTransform::fromTranslate(dx, dy).
        rotate(a).translate(-dx, -dy));
}

QSGGeometry*
ClockRenderer::rectGeometry(
    const QRectF& aRect)
{
    QSGGeometry* g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 4);
    QSGGeometry::Point2D* v = g->vertexDataAsPoint2D();
    g->setDrawingMode(GL_TRIANGLE_FAN);
    v[0].x = aRect.left();  v[0].y = aRect.top();
    v[1].x = v[0].x;        v[1].y = aRect.bottom();
    v[2].x = aRect.right(); v[2].y = v[1].y;
    v[3].x = v[2].x;        v[3].y = v[0].y;
    return g;
}

QSGGeometry*
ClockRenderer::circleGeometry(
    const QPointF& aCenter,
    qreal aRadius)
{
    const int n = 8*aRadius;
    const float x0 = aCenter.x();
    const float y0 = aCenter.y();
    QSGGeometry* g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), n+2);
    QSGGeometry::Point2D* v = g->vertexDataAsPoint2D();
    g->setDrawingMode(GL_TRIANGLE_FAN);
    v[0].x = x0;
    v[0].y = y0;
    for (int i=0; i<n; i++) {
        const float theta = i*2*M_PI/n;
        v[i+1].x = x0 + aRadius*cos(theta);
        v[i+1].y = y0 + aRadius*sin(theta);
    }
    v[n+1] = v[1];
    return g;
}

QSGGeometry*
ClockRenderer::ringGeometry(
    const QPointF& aCenter,
    qreal aRadius,
    qreal aThickness)
{
    const int n = 8*aRadius;
    const float x0 = aCenter.x();
    const float y0 = aCenter.y();
    const float innerRadius = aRadius - qMin(aRadius, aThickness);
    QSGGeometry* g = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 2*(n+1));
    QSGGeometry::Point2D* v = g->vertexDataAsPoint2D();
    g->setDrawingMode(GL_TRIANGLE_STRIP);
    for (int i=0; i<n; i++) {
        const float theta = i*2*M_PI/n;
        v[2*i].x = x0 + innerRadius*cos(theta);
        v[2*i].y = y0 + innerRadius*sin(theta);
        v[2*i+1].x = x0 + aRadius*cos(theta);
        v[2*i+1].y = y0 + aRadius*sin(theta);
    }
    v[2*n] = v[0];
    v[2*n+1] = v[1];
    return g;
}

QSGNode*
ClockRenderer::geometryNode(
    QSGGeometry* aGeometry,
    const QColor& aColor)
{
    QSGFlatColorMaterial* m = new QSGFlatColorMaterial;
    m->setColor(aColor);
    QSGGeometryNode* node = new QSGGeometryNode;
    node->setGeometry(aGeometry);
    node->setMaterial(m);
    node->setFlag(QSGNode::OwnsGeometry);
    node->setFlag(QSGNode::OwnsMaterial);
    return node;
}

QSGNode*
ClockRenderer::centerDiskNode(
    QQuickWindow* aWindow,
    const QPointF& aCenter,
    qreal aRadius,
    int aCenterRadius,
    const QColor& aColor)
{
    QSGNode* disk = circleNode(aCenter, aRadius, aColor);
    disk->appendChildNode(centerNode(aWindow, aCenter, aCenterRadius));
    return disk;
}

QSGNode*
ClockRenderer::centerNode(
    QQuickWindow* aWindow,
    const QPointF& aCenter,
    int aRadius)
{
    const int r = qMax(aRadius & ~1, 2);
    const int r2 = qMax((aRadius/2) & ~1, 1);

    if (r2 == 1) {
        const int d = 2*r;
        QPointF center(0,0);
        QPixmap pixmap(d, d);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.setPen(Qt::NoPen);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.translate(r, r);
        painter.setBrush(QBrush(Qt::white));
        painter.drawEllipse(center, r, r);
        painter.setBrush(QBrush(Qt::black));
        painter.drawEllipse(center, r2, r2);
        return new ImageNode(aWindow, aCenter.x()-r, aCenter.y()-r,
            pixmap.toImage());
    } else {
        QSGNode* node = circleNode(aCenter, r, Qt::white);
        node->appendChildNode(circleNode(aCenter, r2, Qt::black));
        return node;
    }
}
