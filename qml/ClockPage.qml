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

Page {
    id: page

    allowedOrientations: window.allowedOrientations

    property int _initialIndex: 0
    property bool _ready: false

    readonly property var clockModel: [
        {
            style: "SwissRailroad",
            //: Label for Swiss raiload clock style
            //% "Swiss railroad"
            title: qsTrId("swissclock-title-swiss_railroad")
        },{
            style: "HelsinkiMetro",
            //: Label for Helsinki metro clock style
            //% "Helsinki metro"
            title: qsTrId("swissclock-title-helsinki_metro")
        },{
            style: "DeutscheBahn",
            //: Label for Deutsche Bahn clock style
            //% "Deutsche Bahn"
            title: qsTrId("swissclock-title-deutsche_bahn")
        }
    ]

    function clockAt(i) {
        return clockModel[(i + _initialIndex) % clockModel.length];
    }

    Component.onCompleted: {
        for (var i=0; i<clockModel.length; i++) {
            if (clockModel[i].style === ClockSettings.clockStyle) {
                _initialIndex += i
                break
            }
        }
        _ready = true
        slideshow.model = clockModel
        slideshow.forceActiveFocus()
        orientationReminder.restart()
    }

    onOrientationChanged: orientationReminder.restart()

    Timer {
        id: orientationReminder

        interval: 250
    }

    Connections {
        target: ClockSettings
        onClockStyleChanged: {
            if (_ready) {
                for (var i=0; i<clockModel.length; i++) {
                    if (clockAt(i).style === ClockSettings.clockStyle) {
                        slideshow.currentIndex = i
                        break
                    }
                }
            }
        }
    }

    SlideshowView {
        id: slideshow

        snapMode: PathView.SnapToItem
        anchors.fill: parent
        delegate: ClockDelegate {
            width: slideshow.itemWidth
            height: slideshow.height
            style: clockAt(index).style
            title: clockAt(index).title
            flicking: slideshow.moving || flickSimulationTimer.running
            selected: index === slideshow.currentIndex
            landscape: page.isLandscape
            peekNumbers: orientationReminder.running
        }
        onCurrentIndexChanged: {
            if (_ready) {
                ClockSettings.clockStyle = clockAt(currentIndex).style
            }
        }
        Keys.onSelectPressed: defaultAction()
        Keys.onReturnPressed: defaultAction()
        Keys.onLeftPressed: {
            flickSimulationTimer.restart()
            decrementCurrentIndex()
        }
        Keys.onRightPressed: {
            flickSimulationTimer.restart()
            incrementCurrentIndex()
        }

        function defaultAction() {
            if (doubleClickTimer.running) {
                doubleClickTimer.stop()     // Double click
                ClockSettings.invertColors = !ClockSettings.invertColors
            } else {
                doubleClickTimer.start()    // Don't know yet
            }
        }

        Timer {
            id: doubleClickTimer

            interval: 500
            onTriggered: ClockSettings.showNumbers = !ClockSettings.showNumbers // Single click
        }

        Timer {
            id: flickSimulationTimer

            interval: 1000
        }
    }
}
