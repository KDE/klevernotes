// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1

import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0
import org.qtproject.example 1.0

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

    function switchToPage(page){
        const pages = {
            'about':'qrc:About.qml',
            'settings':''
            }
        pageStack.layers.push(pages[page])
    }

    FolderDialog{
        id:folderDialog
        folder:StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        options: FolderDialog.ShowDirsOnly
        onAccepted: {
            textPromptDialog.folder = currentFolder
        }
    }

    Kirigami.PromptDialog {
        id: textPromptDialog
        title: "Klever Notes Storage"


        property string folder
        property string userChoice
        standardButtons: Kirigami.Dialog.None
        customFooterActions: [
            Kirigami.Action {
                text: i18n("Existing storage")
                onTriggered: {
                    textPromptDialog.userChoice = "Existing storage chosen at "
                    folderDialog.open()
                }
            },
            Kirigami.Action {
                text: i18n("Create storage")
                onTriggered: {
                    textPromptDialog.userChoice = "Storage created at "
                    folderDialog.open()
                }
            }
        ]

        onFolderChanged: {
            var folderPath = textPromptDialog.folder
            if (userChoice === "Storage created at "){
                folderPath = textPromptDialog.folder.concat("/.KleverNotes")
            }

            var pathEnd = folderPath.substring(folderPath.length,folderPath.length-13)

            if (pathEnd !== "/.KleverNotes"){
                textPromptDialog.close();
                var subtitle = i18n(
"It looks like the selected folder is not a Klever Notes storage.

Please choose a location for your future Klever Note storage or select an existing one.")
                root.checkForStorage(subtitle)
            }
            else{
                StorageHandler.makeStorage(folderPath)
                Config.path = folderPath

                var fullNotification = textPromptDialog.userChoice+KleverUtility.getPath(folderPath)

                showPassiveNotification(fullNotification);
                textPromptDialog.close();
            }
        }

        onRejected:{
            var subtitle = i18n(
"This step is mandatory, please don't skip it.

Choose a location to your future Klever Note storage or select an existing one.")
            root.checkForStorage(subtitle)
        }
    }

    function checkForStorage(subtitle){
        var actualPath = Config.path

        if (actualPath === "None"){
            textPromptDialog.subtitle = subtitle
            textPromptDialog.open()
        }
    }

    Component.onCompleted: {
        App.restoreWindowGeometry(root)
        var subtitle = i18n(
"It looks like this is your first time using this app!

Please choose a location for your future Klever Note storage or select an existing one.")
        root.checkForStorage(subtitle)
        // StorageHandler.callMe(subtitle)
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

    globalDrawer: Sidebar{}

    pageStack.initialPage: page

    Kirigami.Page {
        id: page

        Layout.fillWidth: true

        title: i18n("Main Page")

        Editor{
            id:editor
        }

    }

    footer: BottomToolBar{}
}
