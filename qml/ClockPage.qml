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

import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.swissclock 1.0

Page {
    id: page
    allowedOrientations: window.allowedOrientations
    property string clockStyle: clockSettings.clockStyle
    property int initialIndex: 0
    property bool ready: false

    function getClockTitle(index) {
        if (getClockTitle["text"] === undefined) {
            getClockTitle.text = [
                qsTr("Swiss railroad"),
                qsTr("Helsinki metro")
            ]
        }
        return getClockTitle.text[(index + initialIndex) % clockModel.count]
    }

    function getClockStyle(index) {
        if (getClockStyle["text"] === undefined) {
            getClockStyle.text = [
                "SwissRailroad",
                "HelsinkiMetro"
            ]
        }
        return getClockStyle.text[(index + initialIndex) % clockModel.count]
    }

    ListModel {
         id: clockModel
         ListElement { i: 0 }
         ListElement { i: 1 }
    }

    Component.onCompleted: {
        var n = clockModel.count
        for (var i=0; i<n; i++) {
            if (getClockStyle(i) === clockStyle) {
                initialIndex += i
                break
            }
        }
        ready = true
        slideshow.model = clockModel
    }

    onClockStyleChanged: {
        if (ready) {
            var n = clockModel.count
            for (var i=0; i<n; i++) {
                if (getClockStyle(i) === clockStyle) {
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
                onClicked: clockSettings.showNumbers = !clockSettings.showNumbers
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
                            width: clockItem.width - label2.width - label3.width - 4*Theme.paddingSmall
                            height: width
                            Clock {
                                id: clock
                                anchors.fill: parent
                                settings: clockSettings
                                style: getClockStyle(index)
                            }
                            MouseArea {
                                anchors.fill: parent
                                onClicked: mouse.accepted = true
                                onDoubleClicked: {
                                    clockSettings.invertColors = !clockSettings.invertColors
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
                    text: getClockTitle(index)
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
                    when: ready && clockSettings.showNumbers && !slideshow.moving
                    PropertyChanges { target: label1; opacity: 1 }
                    PropertyChanges { target: label2; opacity: 1 }
                    PropertyChanges { target: label3; opacity: 1 }
                    PropertyChanges { target: label4; opacity: 1 }
                },
                State {
                    name: "hideNumbers"
                    when: ready && !clockSettings.showNumbers && !slideshow.moving
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
        onCurrentIndexChanged: clockSettings.clockStyle = getClockStyle(currentIndex)
    }
}
