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

#ifndef CLOCK_DEBUG_H
#define CLOCK_DEBUG_H

#include "HarbourDebug.h"

#if HARBOUR_DEBUG
#  define CLOCK_PERFORMANCE_LOG_ENABLED
#endif // HARBOUR_DEBUG

#define QTRACE(x) HDEBUG(((void*)this) << x)
#define QVERBOSE(x) HVERBOSE(x)

#ifdef CLOCK_PERFORMANCE_LOG_ENABLED
#  include <QDateTime>
class ClockPerformance {
public:
    ClockPerformance() { reset(); }
    void reset() {
        iRenderCount = 0;
        iStartTime = QDateTime::currentDateTime();
    }
    void record(QObject* aOwner) {
        iRenderCount++;
        QDateTime now = QDateTime::currentDateTime();
        const int ms = iStartTime.msecsTo(now);
        if (ms >= 1000) {
            if (iRenderCount > 0) {
                HDEBUG(aOwner->metaObject()->className() << ((void*)aOwner) <<
                    iRenderCount*1000.0/ms << "frames per second");
                iRenderCount = 0;
            }
            iStartTime = now;
        }
    }
private:
    int iRenderCount;
    QDateTime iStartTime;
};
#  define CLOCK_PERFORMANCE_LOG_DEFINE  ClockPerformance iPerformanceLog;
#  define CLOCK_PERFORMANCE_LOG_RESET   iPerformanceLog.reset()
#  define CLOCK_PERFORMANCE_LOG_RECORD  iPerformanceLog.record(this)
#else
#  define CLOCK_PERFORMANCE_LOG_DEFINE
#  define CLOCK_PERFORMANCE_LOG_RESET
#  define CLOCK_PERFORMANCE_LOG_RECORD
#endif // CLOCK_PERFORMANCE_LOG_ENABLED

#endif // CLOCK_DEBUG_H
