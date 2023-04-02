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

import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.swissclock 1.0

ApplicationWindow {
    id: window
    allowedOrientations: {
        switch (ClockSettings.orientation) {
        case ClockSettings.OrientationPortrait: return Orientation.Portrait
        case ClockSettings.OrientationPortraitAny: return Orientation.PortraitMask
        case ClockSettings.OrientationLandscape: return Orientation.Landscape
        case ClockSettings.OrientationLandscapeAny: return Orientation.LandscapeMask
        case ClockSettings.OrientationAny: return Orientation.All
        default: return (Screen.sizeCategory !== undefined && Screen.sizeCategory >= Screen.Large) ?
                    Orientation.Landscape : Orientation.Portrait
        }
    }
    initialPage: Component { ClockPage { allowedOrientations: window.allowedOrientations } }
    cover: Component { ClockCover {} }
    HarbourDisplayBlanking {
        pauseRequested: Qt.application.active && ClockSettings.keepDisplayOn &&
            (HarbourBattery.batteryState === HarbourBattery.BatteryStateCharging ||
             HarbourBattery.batteryLevel === 0 || // Zero if unknown (not reported by mce)
             HarbourBattery.batteryLevel >= 20)
    }
}
