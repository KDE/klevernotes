// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0
import "qrc:/contents/ui/sideBar"

Kirigami.ApplicationWindow {
    id: root

    title: i18nc("@title:ApplicationWindow", "KleverNotes")

    minimumWidth: Kirigami.Units.gridUnit * 25
    minimumHeight: Kirigami.Units.gridUnit * 30

    onClosing: {
        App.saveWindowGeometry(root) ;
        const mainPage = pageStack.get(0)
        const editor = mainPage.editorView.editor
        editor.saveNote(editor.text, editor.path)
    }

    onWidthChanged: saveWindowGeometryTimer.restart()
    onHeightChanged: saveWindowGeometryTimer.restart()
    onXChanged: saveWindowGeometryTimer.restart()
    onYChanged: saveWindowGeometryTimer.restart()

    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn

    Kirigami.PagePool {id: pagePool}

    globalDrawer: Sidebar{}

    function getPage(name) {
        switch (name) {
            case "Main": return pagePool.loadPage("qrc:contents/ui/pages/MainPage.qml");
            case "Settings": return pagePool.loadPage("qrc:contents/ui/pages/SettingsPage.qml");
            case "Painting": return pagePool.loadPage("qrc:contents/ui/pages/PaintingPage.qml");
            case "Printing": return pagePool.loadPage("qrc:contents/ui/pages/PrintingPage.qml");
            case "About": return pagePool.loadPage("qrc:contents/ui/pages/AboutPage.qml");
        }
    }

    function switchToPage(pageName) {
        const page = getPage(pageName)

        pageStack.push(page);
    }

    pageStack.onCurrentItemChanged: if (isMainPage() && pageStack.depth > 1) pageStack.pop()

    function isMainPage(){
        return pageStack.currentItem == getPage("Main")
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
    }
}
