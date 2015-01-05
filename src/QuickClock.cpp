/*
  Copyright (C) 2014-2015 Jolla Ltd.
  Contact: Slava Monich <slava.monich@jolla.com>
  All rights reserved.

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

#include "QuickClock.h"
#include "ClockSettings.h"
#include "ClockDebug.h"

#include <QQuickWindow>
#include <QSGSimpleTextureNode>

#define UPDATE_INTERVAL_WITH_DISPLAY_ON  (10)
#define UPDATE_INTERVAL_WITH_DISPLAY_OFF (200)

class QuickClock::Node : public QSGSimpleTextureNode
{
public:
    Node(const QRect& aRect);
    ~Node();
    void update(QQuickWindow* aWindow, QImage aImage);
private:
    QRect iRect;
    QImage iImage;
};

QuickClock::Node::Node(const QRect& aRect) : iRect(aRect)
{
    setRect(QRectF(aRect));
    setFlag(OwnedByParent);
}

void QuickClock::Node::update(QQuickWindow* aWindow, QImage aImage)
{
    delete texture();
    setTexture(aWindow->createTextureFromImage(QImage(aImage.bits() +
        iRect.x()*aImage.depth()/8 + iRect.y()*aImage.bytesPerLine(),
        iRect.width(), iRect.height(), aImage.bytesPerLine(),
        aImage.format())));
    iImage = aImage; // iImage holds the data
}

QuickClock::Node::~Node()
{
    delete texture();
}

QuickClock::QuickClock(QQuickItem* aParent) :
    QQuickItem(aParent),
    iInvertColors(DEFAULT_INVERT_COLORS),
    iDrawBackground(true),
    iDisplayOff(false),
    iDisplayLocked(false),
    iRunning(true),
    iRepaintAll(true),
    iThemeDefault(ClockTheme::newDefault()),
    iThemeInverted(ClockTheme::newInverted()),
    iDialPlatePixmap(NULL),
    iHourMinPixmap(NULL),
    iRepaintTimer(new QTimer(this))
{
    QTRACE("- created");
    iRenderers.append(ClockRenderer::newSwissRailroad());
    iRenderers.append(ClockRenderer::newHelsinkiMetro());
    iRenderers.append(ClockRenderer::newDeutscheBahn());
    setStyle(DEFAULT_CLOCK_STYLE);
    setFlags(ItemHasContents);

    iRepaintTimer->setSingleShot(true);
    iRepaintTimer->setInterval(UPDATE_INTERVAL_WITH_DISPLAY_ON);
    connect(iRepaintTimer, SIGNAL(timeout()), SLOT(onRepaintTimer()));

#if CLOCK_PERFORMANCE_LOG
    iRenderCount = 0;
    iStartTime = QDateTime::currentDateTime();
#endif // CLOCK_PERFORMANCE_LOG
    setRunning(true);
}

QuickClock::~QuickClock()
{
    QTRACE("- destroyed");
    delete iDialPlatePixmap;
    delete iHourMinPixmap;
    delete iThemeDefault;
    delete iThemeInverted;
    qDeleteAll(iRenderers);
}

QTime QuickClock::currentTime()
{
#if CLOCK_DEBUG
    static const char FIXED_TIME_UNINITIALIZED[] = "";
    static const char* fixedTimeString = FIXED_TIME_UNINITIALIZED;
    static QTime fixedTime;
    if (fixedTimeString == FIXED_TIME_UNINITIALIZED) {
        fixedTimeString = getenv("QUICK_CLOCK_TIME");
        if (fixedTimeString) {
            fixedTime = QTime::fromString(fixedTimeString, "h:mm:s");
            if (!fixedTime.isValid()) {
                fixedTime = QTime::fromString(fixedTimeString, "h:mm:s.z");
            }
            if (fixedTime.isValid()) {
                qDebug() << "Fixed" << fixedTime;
            } else {
                qWarning() << "Invalid time string " << fixedTimeString;
                fixedTimeString = NULL;
            }
        }
    }
    if (fixedTimeString) return fixedTime;
#endif
    return QTime::currentTime();
}

ClockTheme* QuickClock::theme() const
{
    return iInvertColors ? iThemeInverted : iThemeDefault;
}

void QuickClock::setInvertColors(bool aValue)
{
    if (iInvertColors != aValue) {
        iInvertColors = aValue;
        emit invertColorsChanged();
        iRepaintAll = true;
        update();
    }
}

void QuickClock::setDrawBackground(bool aValue)
{
    if (iDrawBackground != aValue) {
        iDrawBackground = aValue;
        emit drawBackgroundChanged();
        iRepaintAll = true;
        update();
    }
}

void QuickClock::setStyle(QString aValue)
{
    if (aValue.isEmpty()) aValue = DEFAULT_CLOCK_STYLE;
    for (int i=0; i<iRenderers.count(); i++) {
        ClockRenderer* renderer = iRenderers.at(i);
        if (renderer->id() == aValue) {
            QTRACE("style = " << aValue);
            if (iRenderer != renderer) {
                iRenderer = renderer;
                emit styleChanged();
                iRepaintAll = true;
                update();
            }
            return;
        }
    }
    QTRACE("unknown style" << aValue);
    if (!iRenderer) iRenderer = iRenderers.at(0);
}


bool QuickClock::updatesEnabled() const
{
    return (iRunning && (!iDisplayOff || !iDisplayLocked));
}

void QuickClock::setDisplayStatus(QString aValue)
{
    QTRACE("-" << aValue);
    if (iDisplayStatus != aValue) {
        iDisplayStatus = aValue;
        emit displayStatusChanged();
        iDisplayOff = (iDisplayStatus == "off");
        iRepaintTimer->setInterval(iDisplayOff ?
            UPDATE_INTERVAL_WITH_DISPLAY_OFF :
            UPDATE_INTERVAL_WITH_DISPLAY_ON);
        if (updatesEnabled()) {
            QTRACE("- requesting update");
            update();
        }
    }
}

void QuickClock::setLockMode(QString aValue)
{
    QTRACE("-" << aValue);
    if (iLockMode != aValue) {
        iLockMode = aValue;
        emit lockModeChanged();
        iDisplayLocked = (iLockMode == "locked");
        if (updatesEnabled()) {
            QTRACE("- requesting update");
            update();
        }
    }
}

void QuickClock::setRunning(bool aRunning)
{
    if (iRunning != aRunning) {
        iRunning = aRunning;
        emit runningChanged();
        if (updatesEnabled()) {
            QTRACE("- requesting update");
            update();
        }
    }
}

void QuickClock::onRepaintTimer()
{
    if (updatesEnabled()) {
        update();
    } else {
        QTRACE("- stopping updates");
    }
}

void QuickClock::scheduleUpdate()
{
    QMetaObject::invokeMethod(iRepaintTimer, "start", Qt::QueuedConnection);
}

void QuickClock::paintOffScreenNoSec(
    QPainter* aPainter,
    const QSize& aSize,
    const QTime& aTime)
{
    if (!iDialPlatePixmap || iDialPlatePixmap->size() != aSize) {
        delete iDialPlatePixmap;
        iDialPlatePixmap = new QPixmap(aSize);

        QDEBUG("drawing dial plate");
        QPainter painter(iDialPlatePixmap);

        QRectF rect(QPoint(0,0), aSize);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(rect, QBrush(Qt::transparent));

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        aPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
        iRenderer->paintDialPlate(&painter, aSize, theme(), iDrawBackground);
    }

    aPainter->drawPixmap(0, 0, *iDialPlatePixmap);

    aPainter->save();
    aPainter->setRenderHint(QPainter::Antialiasing);
    aPainter->setRenderHint(QPainter::HighQualityAntialiasing);
    aPainter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    iRenderer->paintHourMinHands(aPainter, aSize, aTime, theme());
    aPainter->restore();
}

void QuickClock::repaintHourMin(const QSize& aSize, const QTime& aTime)
{
    delete iHourMinPixmap;
    iHourMinPixmap = new QPixmap(aSize);
    iHourMinPixmap->fill(Qt::transparent);
    QPainter painter(iHourMinPixmap);
    if (Q_UNLIKELY(aTime.second() == 0)) {
        QTime t1 = aTime.addSecs(-1);
        QTime t2(t1.hour(), t1.minute(), 60*t1.msec()/1000);
        QTRACE("- drawing hour and minute hands (live)" <<
            qPrintable(t2.toString("hh:mm:ss.zzz")));
        paintOffScreenNoSec(&painter, aSize, t2);
    } else {
        QTime t1(aTime.hour(), aTime.minute(), 0);
        QTRACE("- drawing hour and minute hands" <<
            qPrintable(t1.toString("hh:mm:ss.zzz")));
        paintOffScreenNoSec(&painter, aSize, t1);
    }
}

QuickClock::Node* QuickClock::createSecNode(const QSize& aSize,
    const QTime& aTime)
{
    QPixmap pixmap(aSize);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.drawPixmap(0, 0, *iHourMinPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    iRenderer->paintSecHand(&painter, aSize, aTime, theme());

    Node* node = new Node(QRect(QPoint(0,0), aSize));
    node->update(window(), pixmap.toImage());
    return node;
}

QSGNode* QuickClock::updatePaintNode(QSGNode* aNode, UpdatePaintNodeData*)
{
    QTime time = currentTime();
    //QTRACE("- rendering" << qPrintable(time.toString("hh:mm:ss.zzz")));

    const int w = (int)width() & ~1;
    const int h = (int)height() & ~1;
    QRect rect(0, 0, w, h);
    QSize size(w, h);

    bool updateHourMinNode = false;
    if (iRepaintAll) {
        iRepaintAll = false;
        delete iDialPlatePixmap;
        iDialPlatePixmap = NULL;
        updateHourMinNode = true;
    }

    Node* hourMinNode = (Node*)aNode;
    if (!hourMinNode || hourMinNode->rect() != rect) {
        delete hourMinNode;
        aNode = hourMinNode = new Node(rect);
        updateHourMinNode = true;
    }

    if (!updateHourMinNode &&
       (time.second() == 0 ||
        time.minute() != iPaintTimeNoSec.minute() ||
        time.hour() != iPaintTimeNoSec.hour())) {
        updateHourMinNode = true;
    }

    if (updateHourMinNode) {
        repaintHourMin(size, time);
        hourMinNode->update(window(), iHourMinPixmap->toImage());
    }

    delete aNode->firstChild();
    hourMinNode->appendChildNode(createSecNode(size, time));

    iPaintTimeNoSec = time;
    scheduleUpdate();

#if CLOCK_PERFORMANCE_LOG
    iRenderCount++;

#  if CLOCK_DEBUG
    QDateTime now = QDateTime::currentDateTime();
    const int ms = iStartTime.msecsTo(now);
    if (ms >= 1000) {
        if (iRenderCount > 0) {
            QTRACE(iRenderCount*1000.0/ms << "frames per second");
            iRenderCount = 0;
        }
        iStartTime = now;
    }
#  endif // CLOCK_DEBUG
#endif // CLOCK_PERFORMANCE_LOG

    return aNode;
}
