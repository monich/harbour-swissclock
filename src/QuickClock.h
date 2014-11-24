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

#include "ClockRenderer.h"
#include "ClockTheme.h"

#include <QQuickPaintedItem>
#include <QDateTime>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include <QList>

class QuickClock: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool invertColors READ invertColors WRITE setInvertColors NOTIFY invertColorsChanged)
    Q_PROPERTY(bool drawBackground READ drawBackground WRITE setDrawBackground NOTIFY drawBackgroundChanged)
    Q_PROPERTY(QString lockMode READ lockMode WRITE setLockMode NOTIFY lockModeChanged)
    Q_PROPERTY(QString displayStatus READ displayStatus WRITE setDisplayStatus NOTIFY displayStatusChanged)
    Q_PROPERTY(QString style READ style WRITE setStyle NOTIFY styleChanged)

public:
    explicit QuickClock(QQuickItem* aParent = NULL);
    ~QuickClock();

    bool invertColors() const { return iInvertColors; }
    void setInvertColors(bool aValue);

    bool drawBackground() const { return iDrawBackground; }
    void setDrawBackground(bool aValue);

    bool running() const { return iRunning; }
    void setRunning(bool aRunning);

    QString lockMode() const { return iLockMode; }
    void setLockMode(QString aValue);

    QString displayStatus() const { return iDisplayStatus; }
    void setDisplayStatus(QString aValue);

    QString style() const { return iRenderer->id(); }
    void setStyle(QString aValue);

    static QTime currentTime();

signals:
    void invertColorsChanged();
    void drawBackgroundChanged();
    void displayStatusChanged();
    void lockModeChanged();
    void styleChanged();
    void runningChanged();

protected:
    virtual void paint(QPainter* aPainter);

private:
    ClockTheme* theme() const;
    void invalidatePixmaps();
    bool updatesEnabled() const;
    void scheduleUpdate();
    void paintOffScreenNoSec(QPainter* aPainter, const QSize& aSize,
         const QTime& aTime);

private slots:
    void onRepaintTimer();

private:
    bool iInvertColors;
    bool iDrawBackground;
    bool iDisplayOff;
    bool iDisplayLocked;
    bool iRunning;
    QString iLockMode;
    QString iDisplayStatus;
    ClockTheme* iThemeDefault;
    ClockTheme* iThemeInverted;
    QList<ClockRenderer*> iRenderers;
    ClockRenderer* iRenderer;
    QPixmap* iDialPlate;
    QPixmap* iOffScreenNoSec;   // Everything except seconds
    QTime iPaintTimeNoSec;      // When iOffScreenNoSec was painted
    QTimer* iRepaintTimer;

#define CLOCK_PERFORMANCE_LOG 0
#if CLOCK_PERFORMANCE_LOG
    QDateTime iStartTime;
    int iRenderCount;
#endif // CLOCK_PERFORMANCE_LOG
};

QML_DECLARE_TYPE(QuickClock)

#endif // QUICK_CLOCK_H
