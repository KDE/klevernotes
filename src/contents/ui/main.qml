// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

import org.qtproject.example 1.0
import "qrc:/contents/ui/sideBar"
import "qrc:/contents/ui/dialogs"

import org.kde.Klever 1.0

Kirigami.ApplicationWindow {
    id: root

    title: i18n("Klever")

    minimumWidth: Kirigami.Units.gridUnit * 20
    minimumHeight: Kirigami.Units.gridUnit * 20

    onClosing: App.saveWindowGeometry(root)

    onWidthChanged: saveWindowGeometryTimer.restart()
    onHeightChanged: saveWindowGeometryTimer.restart()
    onXChanged: saveWindowGeometryTimer.restart()
    onYChanged: saveWindowGeometryTimer.restart()

    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn

    Kirigami.PagePool {
        id: pagePool
    }

    globalDrawer: Sidebar{}

    function getPage(name) {
        switch (name) {
            case "Main": return pagePool.loadPage("qrc:contents/ui/pages/MainPage.qml");
            // case "Settings": return pagePool.loadPage("qrc:/qml/settings/SettingsPage.qml");
            case "About": return pagePool.loadPage("qrc:contents/ui/pages/AboutPage.qml");
        }
    }

    function switchToPage(pageName) {
        const page = getPage(pageName)

        pageStack.push(page);
    }

    function isMainPage(){
        return pageStack.currentItem == getPage("Main")
    }

    function checkForStorage(subtitle){
        var actualPath = Config.path

        if (actualPath === "None"){
            let component = Qt.createComponent("qrc:/contents/ui/dialogs/StorageDialog.qml")

            if (component.status == Component.Ready) {
                var dialog = component.createObject(root);

                if (subtitle !=="") dialog.subtitle = subtitle
                dialog.open()
            }
        }
    }


    // This timer allows to batch update the window size change to reduce
    // the io load and also work around the fact that x/y/width/height are
    // changed when loading the page and overwrite the saved geometry from
    // the previous session.
    Timer {
        id: saveWindowGeometryTimer
        interval: 1000
        onTriggered: App.saveWindowGeometry(root)
    }

    Component.onCompleted: {
        App.restoreWindowGeometry(root)
        switchToPage('Main')
        checkForStorage("")
    }
}
