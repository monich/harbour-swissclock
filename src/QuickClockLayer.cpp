/*
 * Copyright (C) 2016 Jolla Ltd.
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

#include "QuickClockLayer.h"
#include "ClockDebug.h"

#include <QSGGeometryNode>
#include <QSGSimpleRectNode>

#define SUPER QQuickItem

QuickClockLayer::QuickClockLayer(QuickClock* aParent) :
    SUPER(aParent),
    iClock(aParent),
    iDirty(true)
{
    setFlags(ItemHasContents);
    setAntialiasing(true);
    setSize(QSizeF(aParent->width(), aParent->height()));
    setVisible(aParent->isVisible());
    connect(aParent, SIGNAL(widthChanged()), SLOT(onWidthChanged()));
    connect(aParent, SIGNAL(heightChanged()), SLOT(onHeightChanged()));
    connect(aParent, SIGNAL(visibleChanged()), SLOT(onVisibleChanged()));
    connect(aParent, SIGNAL(updatesEnabledChanged()), SLOT(onUpdatesEnabledChanged()));
}

void
QuickClockLayer::requestUpdate(
    bool aFullUpdate)
{
    if (aFullUpdate) {
        iDirty = true;
    }
    update();
}

void
QuickClockLayer::onUpdatesEnabledChanged()
{
    if (updatesEnabled()) {
        QTRACE("- requesting update");
        requestUpdate(false);
    }
}

void
QuickClockLayer::onWidthChanged()
{
    QTRACE(iClock->width());
    setWidth(iClock->width());
    requestUpdate(true);
}

void
QuickClockLayer::onHeightChanged()
{
    QTRACE(iClock->height());
    setHeight(iClock->height());
    requestUpdate(true);
}

void
QuickClockLayer::onVisibleChanged()
{
    QTRACE(iClock->isVisible());
    setVisible(iClock->isVisible());
    if (iClock->isVisible()) {
        requestUpdate(true);
    }
}

void
QuickClockLayer::onUpdated()
{
    int msec = msecUntilNextUpdate();
    if (msec == 0) {
        requestUpdate(false);
    } else if (msec > 0) {
        const int minMSec = iClock->minUpdateInterval();
        if (msec < minMSec) {
            msec = minMSec;
        }
        iRepaintTimer.start(msec, this);
    }
}

void
QuickClockLayer::timerEvent(
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

QSGNode*
QuickClockLayer::updatePaintNode(
    QSGNode* aNode,
    QQuickItem::UpdatePaintNodeData* aData)
{
    QSize size((int)width() & ~1, (int)height() & ~1);

    if (aNode && iDirty) {
        iDirty = false;
        delete aNode;
        aNode = NULL;
    }

    if (Q_UNLIKELY(!aNode)) {
        aNode = createNode(size);
    }

    if (aNode) {
        QTime time = QuickClock::currentTime();
        updateNode(aNode, size, time);
        QMetaObject::invokeMethod(this, "onUpdated", Qt::QueuedConnection);
    }

    return aNode;
}
