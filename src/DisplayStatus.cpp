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

#include "DisplayStatus.h"
#include "ClockDebug.h"

#include <QDBusMessage>
#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>

#define MCE_SERVICE "com.nokia.mce"

DisplayStatus::DisplayStatus(QObject* aParent) :
    QObject(aParent)
{
    QTRACE("- created");

    // Listen for MCE display state change signals
    QDBusConnection systemBus(QDBusConnection::systemBus());
    systemBus.connect(MCE_SERVICE,
         "/com/nokia/mce/signal", "com.nokia.mce.signal",
         "display_status_ind", this, SLOT(onStatusChanged(QString)));

    // And query the current state although it's rather unlikely that we
    // have been started with display off
    connect(new QDBusPendingCallWatcher(systemBus.asyncCall(
        QDBusMessage::createMethodCall(MCE_SERVICE,
        "/com/nokia/mce/request", "com.nokia.mce.request",
        "get_display_status")), this), SIGNAL(finished(QDBusPendingCallWatcher*)),
        this, SLOT(onStatusQueryDone(QDBusPendingCallWatcher*)));
}

DisplayStatus::~DisplayStatus()
{
    QTRACE("- destroyed");
}

void DisplayStatus::setStatus(QString aStatus)
{
    if (iStatus != aStatus) {
        iStatus = aStatus;
        emit statusChanged();
    }
}

void DisplayStatus::onStatusChanged(QString aStatus)
{
    QTRACE("-" << aStatus);
    setStatus(aStatus);
}

void DisplayStatus::onStatusQueryDone(QDBusPendingCallWatcher* aWatcher)
{
    QDBusPendingReply<QString> reply(*aWatcher);
    QTRACE("-" << reply);
    if (reply.isValid() && !reply.isError()) {
        setStatus(reply.value());
    }
    aWatcher->deleteLater();
}
