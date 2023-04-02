/*
 * Copyright (C) 2016-2023 Slava Monich <slava@monich.com>
 * Copyright (C) 2016-2019 Jolla Ltd.
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
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
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
import org.nemomobile.configuration 1.0

Page {
    readonly property string rootPath: "/apps/" + appName() + "/"

    // jolla-settings expects these properties:
    property var applicationName
    property var applicationIcon

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
        invertColors.updateControls()
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content
            width: parent.width

            PageHeader {
                id: pageHeader

                rightMargin: Theme.horizontalPageMargin + (appIcon.visible ? (height - appIcon.padding) : 0)
                title: applicationName ? applicationName : "Swiss Clock"
                description: applicationName ?
                    //: Settings page header description (app version)
                    //% "Version %1"
                    qsTrId("swissclock-settings-version").arg("1.0.9") : ""

                Image {
                    id: appIcon

                    readonly property int padding: Theme.paddingLarge
                    readonly property int size: pageHeader.height - 2 * padding
                    x: pageHeader.width - width - Theme.horizontalPageMargin
                    y: padding
                    width: size
                    height: size
                    sourceSize: Qt.size(size,size)
                    source: applicationIcon ? applicationIcon : ""
                    visible: appIcon.status === Image.Ready
                }
            }

            ComboBox {
                id: invertColorsComboBox
                //: Combo box label
                //% "Dial plate"
                label: qsTrId("swissclock-settings-dial_plate")
                //: Combo box description
                //% "You can also switch between light and dark backgrounds by double-tapping the dial plate"
                description: qsTrId("swissclock-settings-dial_plate-description")
                menu: ContextMenu {
                    id: invertColorsMenu
                    readonly property int defaultIndex: 0
                    MenuItem {
                        //: Combo box value for the normal (light) dial plate
                        //% "Light"
                        text: qsTrId("swissclock-settings-dial_plate-value-light")
                        readonly property int index: 0
                        readonly property bool value: false
                        readonly property bool isMenuItem: true
                    }
                    MenuItem {
                        //: Combo box value for inverted (dark) dial plate
                        //% "Dark"
                        text: qsTrId("swissclock-settings-dial_plate-value-dark")
                        readonly property int index: 1
                        readonly property bool value: true
                        readonly property bool isMenuItem: true
                    }
                }
                onCurrentItemChanged: invertColors.updateValue()
                ConfigurationValue {
                    id: invertColors
                    key: rootPath + "invertColors"
                    defaultValue: false
                    onValueChanged: updateControls()
                    function updateValue() {
                        var item = invertColorsComboBox.currentItem
                        if (item) value = item.value
                    }
                    function updateControls() {
                        var n = invertColorsMenu.children.length
                        var index = invertColorsMenu.defaultIndex
                        for (var i=0; i<n; i++) {
                            var item = invertColorsMenu.children[i]
                            if (item.isMenuItem && value == item.value) {
                                index = item.index
                                break
                            }
                        }
                        invertColorsComboBox.currentIndex = index
                    }
                }
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
                        readonly property int index: 0
                        readonly property int primary: 0
                        readonly property int inverted: primary
                        readonly property bool canInvert: false
                        readonly property bool isMenuItem: true
                    }
                    MenuItem {
                        //: Combo box value for portrait orientation
                        //% "Portrait"
                        text: qsTrId("swissclock-settings-orientation-portrait")
                        readonly property int index: 1
                        readonly property int primary: 1
                        readonly property int inverted: 2
                        readonly property bool canInvert: true
                        readonly property bool isMenuItem: true
                    }
                    MenuItem {
                        //: Combo box value for landscape orientation
                        //% "Landscape"
                        text: qsTrId("swissclock-settings-orientation-landscape")
                        readonly property int index: 2
                        readonly property int primary: 3
                        readonly property int inverted: 4
                        readonly property bool canInvert: true
                        readonly property bool isMenuItem: true
                    }
                    MenuItem {
                        //: Combo box value for dynamic orientation
                        //% "Dynamic"
                        text: qsTrId("swissclock-settings-orientation-dynamic")
                        readonly property int index: 3
                        readonly property int primary: 5
                        readonly property int inverted: primary
                        readonly property bool canInvert: true
                        readonly property bool isMenuItem: true
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
                        if (item.isMenuItem && (value === item.primary || value === item.inverted)) {
                            index = item.index
                            break
                        }
                    }
                    orientationComboBox.currentIndex = index
                    var currentItem = orientationComboBox.currentItem
                    if (currentItem) {
                        if (currentItem.primary === currentItem.inverted) {
                            orientationInvertSwitch.checked = currentItem.canInvert
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
