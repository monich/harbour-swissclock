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

#ifndef CLOCK_RENDERER_H
#define CLOCK_RENDERER_H

#include "ClockTheme.h"

#include <QSize>
#include <QColor>
#include <QImage>
#include <QPainter>
#include <QDateTime>
#include <QQuickWindow>
#include <QMatrix4x4>
#include <QSGGeometryNode>
#include <QSGSimpleTextureNode>

class ClockRenderer
{
public:
    class ImageNode: public QSGSimpleTextureNode {
    public:
        ImageNode(QQuickWindow* aWindow, qreal aX, qreal aY, QImage aImage);
        ~ImageNode();
    };

public:
    virtual ~ClockRenderer();
    virtual void paintDialPlate(QPainter* aPainter, const QSize& aSize,
        ClockTheme* aTheme, bool aDrawBackground) = 0;
    virtual void paintHourMinHands(QPainter* aPainter, const QSize& aSize,
        const QTime& aTime, ClockTheme* aTheme) = 0;
    virtual void paintSecHand(QPainter* aPainter, const QSize& aSize,
        const QTime& aTime, ClockTheme* aTheme) = 0;

    virtual void initSecNode(QSGTransformNode* aTxNode, const QSizeF& aSize,
        QQuickWindow* aWindow, ClockTheme* aTheme) = 0;
    virtual QMatrix4x4 secNodeMatrix(const QSize& aSize, const QTime& aTime);

    QString id() const { return iId; }

    // Utilities
    static QSGGeometry* circleGeometry(const QPointF& aCenter, qreal aRadius);
    static QSGGeometry* ringGeometry(const QPointF& aCenter, qreal aRadius,
        qreal aThickness);
    static QSGNode* geometryNode(QSGGeometry* aGeometry, const QColor& color);
    static QSGNode* circleNode(const QPointF& aCenter, qreal aRadius,
        const QColor& aColor);
    static QSGNode* ringNode(const QPointF& aCenter, qreal aRadius,
        qreal aThickness, const QColor& aColor);
    static QSGNode* centerDiskNode(QQuickWindow* aWindow,
        const QPointF& aCenter, qreal aRadius, int aCenterRadius,
        const QColor& aColor);
    static QSGNode* centerNode(QQuickWindow* aWindow, const QPointF& aCenter,
        int aRadius);

public:
    static const QString SWISS_RAILROAD;
    static const QString HELSINKI_METRO;
    static const QString DEUTSCHE_BAHN;

    static ClockRenderer* newSwissRailroad();
    static ClockRenderer* newHelsinkiMetro();
    static ClockRenderer* newDeutscheBahn();

protected:
    ClockRenderer(QString aId) : iId(aId) {}

private:
    QString iId;
};

#endif // CLOCK_RENDERER_H
