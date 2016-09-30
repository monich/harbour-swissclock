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

import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.swissclock 1.0

Page {
    id: page
    allowedOrientations: window.allowedOrientations
    property string clockStyle: globalClockSettings.clockStyle
    property int initialIndex: 0
    property bool ready: false

    readonly property var clockModel: [
        {
            style: "SwissRailroad",
            //: Label for Swiss raiload clock style
            //% "Swiss railroad"
            title: qsTrId("swiss-clock-title")
        },{
            style: "HelsinkiMetro",
            //: Label for Helsinki metro clock style
            //% "Helsinki metro"
            title: qsTrId("helsinki-clock-title")
        },{
            style: "DeutscheBahn",
            //: Label for Deutsche Bahn clock style
            //% "Deutsche Bahn"
            title: qsTrId("deutschebahn-clock-title")
        }
    ]

    Component.onCompleted: {
        for (var i=0; i<clockModel.length; i++) {
            if (clockModel[i].style === clockStyle) {
                initialIndex += i
                break
            }
        }
        ready = true
        slideshow.model = clockModel
    }

    onClockStyleChanged: {
        if (ready) {
            for (var i=0; i<clockModel.length; i++) {
                if (clockModel[(i + initialIndex) % clockModel.length].style === clockStyle) {
                    slideshow.currentIndex = i
                    break
                }
            }
        }
    }

    SlideshowView {
        id: slideshow
        anchors.fill: parent
        delegate: Item {
            id: clockItem
            width: slideshow.itemWidth
            height: slideshow.height
            MouseArea {
                anchors.fill: parent
                onClicked: globalClockSettings.showNumbers = !globalClockSettings.showNumbers
                onPressAndHold: { / * ignore long taps * / }
            }
            Image {
                anchors.fill: parent
                source: Qt.resolvedUrl("texture.png")
                fillMode: Image.Tile
                Column {
                    id: clockColumn
                    anchors.centerIn: parent
                    spacing: Theme.paddingSmall
                    Label {
                        id: label1
                        text: "12"
                        font.bold: true
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    Row {
                        spacing: Theme.paddingSmall
                        anchors.margins: Theme.paddingSmall
                        Label {
                            id: label2
                            text: "9"
                            font.bold: true
                            height: parent.height
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        Item {
                            width: Math.ceil(clockItem.width - label2.width - label3.width - 4*Theme.paddingSmall)
                            height: width
                            Clock {
                                id: clock
                                anchors.fill: parent
                                drawBackground: true
                                style: clockModel[(index + initialIndex) % clockModel.length].style
                                invertColors: globalClockSettings.invertColors
                                renderType: globalClockSettings.renderType
                                running: (index == slideshow.currentIndex) || slideshow.moving
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: mouse.accepted = true
                                onDoubleClicked: {
                                    globalClockSettings.invertColors = !globalClockSettings.invertColors
                                    mouse.accepted = true
                                }
                            }
                        }
                        Label {
                            id: label3
                            text: "3"
                            font.bold: true
                            height: parent.height
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                    Label {
                        id: label4
                        text: "6"
                        font.bold: true
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
                Label {
                    id: title
                    text: clockModel[(index + initialIndex) % clockModel.length].title
                    font.pixelSize: Theme.fontSizeHuge
                    width: parent.width
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    anchors {
                        top: clockColumn.bottom
                        bottom: parent.bottom
                    }
                    states: [
                        State {
                            name: "showTitle"
                            when: ready && slideshow.moving
                            PropertyChanges { target: title;  opacity: 0.7 }
                        },
                        State {
                            name: "hideTitle"
                            when: ready && !slideshow.moving
                            PropertyChanges { target: title;  opacity: 0 }
                        }
                    ]
                    transitions: [
                        Transition {
                            from: "hideTitle"
                            to: "showTitle"
                            NumberAnimation {
                                properties: "opacity"
                                duration: 250
                                easing.type: Easing.InOutQuad
                            }
                        },
                        Transition {
                            from: "showTitle"
                            to: "hideTitle"
                            NumberAnimation {
                                properties: "opacity"
                                duration: 1000
                                easing.type: Easing.InOutQuad
                            }
                        }
                    ]
                }
            }
            states: [
                State {
                    name: "showNumbers"
                    when: ready && globalClockSettings.showNumbers && !slideshow.moving
                    PropertyChanges { target: label1; opacity: 1 }
                    PropertyChanges { target: label2; opacity: 1 }
                    PropertyChanges { target: label3; opacity: 1 }
                    PropertyChanges { target: label4; opacity: 1 }
                },
                State {
                    name: "hideNumbers"
                    when: ready && !globalClockSettings.showNumbers && !slideshow.moving
                    PropertyChanges { target: label1; opacity: 0 }
                    PropertyChanges { target: label2; opacity: 0 }
                    PropertyChanges { target: label3; opacity: 0 }
                    PropertyChanges { target: label4; opacity: 0 }
                },
                State {
                    name: "flicking"
                    when: ready && slideshow.moving
                    PropertyChanges { target: label1; opacity: 0 }
                    PropertyChanges { target: label2; opacity: 0 }
                    PropertyChanges { target: label3; opacity: 0 }
                    PropertyChanges { target: label4; opacity: 0 }
                }
            ]
            transitions: [
                Transition { from: "flicking"; to: "*"; animations: [ numbersAnimation ] },
                Transition { from: "hideNumbers"; to: "showNumbers"; animations: [ numbersAnimation ] },
                Transition { from: "showNumbers"; to: "hideNumbers"; animations: [ numbersAnimation ] }
            ]
            NumberAnimation {
                id: numbersAnimation
                properties: "opacity"
                easing.type: Easing.InOutQuad
            }
        }
        onCurrentIndexChanged: globalClockSettings.clockStyle = clockModel[(currentIndex + initialIndex) % clockModel.length].style
    }
}
