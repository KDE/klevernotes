// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15

import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

import "qrc:/contents/ui/sideBar"

Kirigami.ApplicationWindow {
    id: root

    readonly property NoteMapper noteMapper: noteMapper
    property string currentPageName: "Main"

    title: i18nc("@title:ApplicationWindow", "KleverNotes")

    minimumWidth: Kirigami.Settings.isMobile ? Kirigami.Units.gridUnit * 25 :  Kirigami.Units.gridUnit * 35
    minimumHeight: Kirigami.Units.gridUnit * 30

    globalDrawer: sideBar
    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn

    onCurrentPageNameChanged: {
        if (!isMainPage()) {
            sideBar.changeWidth = false
            sideBar.close()
        } else if (pageStack.depth > 1){
            if (!sideBar.modal) sideBar.open()
            pageStack.pop()
            sideBar.changeWidth = true
        }
    }
    onClosing: {
        saveState() 
    }
    onXChanged: {
        saveWindowGeometryTimer.restart()
    }
    onYChanged: {
        saveWindowGeometryTimer.restart()
    }
    onWidthChanged: {
        saveWindowGeometryTimer.restart()
    }
    onHeightChanged: {
        saveWindowGeometryTimer.restart()
    }
    Component.onCompleted: {
        App.restoreWindowGeometry(root)
        switchToPage('Main')
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

    Kirigami.PagePool { 
        id: pagePool 
    }

    Sidebar { 
        id: sideBar 
    }

    NoteMapper { 
        id: noteMapper 
    }

    function saveState() {
        App.saveWindowGeometry(root)
        const mainPage = pageStack.get(0)
        const editor = mainPage.editorView.editor
        editor.saveNote(editor.text, editor.path)
        if (Config.noteMapEnabled) noteMapper.saveMap()
    }

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

        pageStack.push(page)

        currentPageName = pageName
    }

    function isMainPage() {
        return currentPageName === "Main"
    }

    function showCheatSheet() {
        if (!isMainPage()) return

        const mainPage = pageStack.get(0)
        mainPage.cheatSheet.open() 
    }
}
