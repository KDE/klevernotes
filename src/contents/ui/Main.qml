// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>

import QtQuick

import org.kde.kirigami as Kirigami

import org.kde.klevernotes

Kirigami.ApplicationWindow {
    id: root

    Item {
        id: windowItem
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        property color windowBackground: Kirigami.Theme.backgroundColor
    }
    Item {
        id: viewItem
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        property color viewBackground: Kirigami.Theme.backgroundColor
    }
    readonly property color sideBarColor: Kirigami.ColorUtils.linearInterpolation(windowItem.windowBackground, viewItem.viewBackground, 0.6)
    property string currentPageName: "Main"

    title: i18nc("@title:ApplicationWindow", "KleverNotes")

    minimumWidth: Kirigami.Settings.isMobile ? Kirigami.Units.gridUnit * 25 :  Kirigami.Units.gridUnit * 35
    minimumHeight: Kirigami.Units.gridUnit * 30

    globalDrawer: sideBar
    pageStack.initialPage: getPage("Main")
    pageStack.columnView.columnResizeMode: Kirigami.ColumnView.SingleColumn
    
    onCurrentPageNameChanged: {
        if (currentPageName !== "Main") {
            sideBar.changeWidth = false
            sideBar.close()
        } else {
            pageStack.layers.pop()
            StyleHandler.inMain = true
        }
        pageTransitionTimer.start()
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

    Sidebar { 
        id: sideBar 

        backgroundColor: root.sideBarColor
    }

    Timer {
        id: pageTransitionTimer

        repeat: false
        interval: Kirigami.Units.longDuration
        onTriggered: {
            if (currentPageName !== "Main") {
                const page = getPage(currentPageName)
                pageStack.layers.push(page)
            } else {
                sideBar.changeWidth = true
                if (!sideBar.modal) {
                    sideBar.open()
                    openDialogTimer.start()
                }
            }
        }
    }

    Timer {
        id: openDialogTimer

        property var dialog: null
        repeat: false
        interval: Kirigami.Units.longDuration
        onTriggered: if (dialog) {
            dialog.open()
            dialog = null
        }
    }
    Connections{
        target : KleverConfig

        function onSortByLastModifiedChanged(){
            saveState()
            NoteTreeModel.initModel()
        }
    }

    function saveState() {
        App.saveWindowGeometry(root)
        const mainPage = pageStack.get(0)
        const editorView = mainPage.editorView
        if (editorView) {
            const editor = editorView.editor
            editor.saveNote(editor.text, editor.path)
        }
        NoteTreeModel.saveMetaData()
        if (KleverConfig.noteMapEnabled) NoteMapper.saveMap()
    }

    function getPage(name) {
        switch (name) {
            case "Main": return Qt.createComponent("org.kde.klevernotes", "MainPage");
            case "Settings": return Qt.createComponent("org.kde.klevernotes", "SettingsPage");
            case "Painting": return Qt.createComponent("org.kde.klevernotes", "PaintingPage");
            case "Printing": return Qt.createComponent("org.kde.klevernotes", "PrintingPage");
            case "About": return Qt.createComponent("org.kde.klevernotes", "AboutPage");
        }
    }

    function switchToPage(pageName, dialogToOpen) {
        currentPageName = pageName
        openDialogTimer.dialog = dialogToOpen
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
