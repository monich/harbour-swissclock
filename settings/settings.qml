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

import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.configuration 1.0

Page {
    readonly property string rootPath: "/apps/" + appName() + "/"

    // Deduce package name from the path
    function appName() {
        var parts = Qt.resolvedUrl("dummy").split('/')
        if (parts.length > 2) {
            var name = parts[parts.length-3]
            if (name.indexOf("swissclock") >= 0) {
                return name
            }
        }
        return "harbour-swissclock"
    }

    Component.onCompleted: {
        orientation.updateControls()
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content
            width: parent.width

            PageHeader {
                //: Settings page header
                //% "Swiss Clock"
                title: qsTrId("swissclock-settings-page_header")
            }

            ComboBox {
                id: orientationComboBox
                //: Combo box label
                //% "Orientation"
                label: qsTrId("swissclock-settings-orientation_label")
                value: currentItem ? currentItem.text : ""
                menu: ContextMenu {
                    id: orientationMenu
                    readonly property int defaultIndex: 0
                    MenuItem {
                        //: Combo box value for primary orientation
                        //% "Primary"
                        text: qsTrId("swissclock-settings-orientation-primary")
                        readonly property int primary: 0
                        readonly property int inverted: primary
                        readonly property bool canInvert: false
                    }
                    MenuItem {
                        //: Combo box value for portrait orientation
                        //% "Portrait"
                        text: qsTrId("swissclock-settings-orientation-portrait")
                        readonly property int primary: 1
                        readonly property int inverted: 2
                        readonly property bool canInvert: true
                    }
                    MenuItem {
                        //: Combo box value for landscape orientation
                        //% "Landscape"
                        text: qsTrId("swissclock-settings-orientation-landscape")
                        readonly property int primary: 3
                        readonly property int inverted: 4
                        readonly property bool canInvert: true
                    }
                    MenuItem {
                        //: Combo box value for dynamic orientation
                        //% "Dynamic"
                        text: qsTrId("swissclock-settings-orientation-dynamic")
                        readonly property int primary: 5
                        readonly property int inverted: primary
                        readonly property bool canInvert: true
                    }
                }
                onCurrentItemChanged: orientation.updateValue()
            }
            TextSwitch {
                id: orientationInvertSwitch
                //: Text switch label
                //% "Allow inverted orientation"
                text: qsTrId("swissclock-settings-orientation-allow_inverted")
                //: Text switch description
                //% "If enabled, allows both primary and inverted landscape or portrait oprientations"
                description: qsTrId("swissclock-settings-orientation-allow_inverted-description")
                onCheckedChanged: orientation.updateValue()
            }
            ConfigurationValue {
                id: orientation
                key: rootPath + "orientation"
                defaultValue: 0
                onValueChanged: updateControls()
                function updateValue() {
                    var item = orientationComboBox.currentItem
                    if (item) {
                        value = orientationInvertSwitch.checked ? item.inverted : item.primary
                    }
                }
                function updateControls() {
                    var n = orientationMenu.children.length
                    var index = orientationMenu.defaultIndex
                    for (var i=0; i<n; i++) {
                        var item = orientationMenu.children[i]
                        if (value === item.primary || value === item.inverted) {
                            index = i
                            break
                        }
                    }
                    orientationComboBox.currentIndex = index
                    var currentItem = orientationComboBox.currentItem
                    if (currentItem) {
                        if (currentItem.primary === currentItem.inverted) {
                            orientationInvertSwitch.checked = item.canInvert
                            orientationInvertSwitch.enabled = false
                        } else {
                            orientationInvertSwitch.checked = (value === currentItem.inverted)
                            orientationInvertSwitch.enabled = true
                        }
                    }
                }
            }
        }
    }
}
