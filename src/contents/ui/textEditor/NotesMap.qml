// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as Controls

import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.kitemmodels

import "qrc:/contents/ui/textEditor/components/"

Kirigami.Dialog {
    id: noteMap

    required property QtObject parser

    title: i18nc("@window:title","Linked notes map")

    background: Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.backgroundColor    
    }

    width: Kirigami.Units.gridUnit * 30
    height: layout.height + header.height + footer.height + verticalPadding * 2
    verticalPadding: Kirigami.Units.largeSpacing

    standardButtons: Controls.Dialog.NoButton

    ColumnLayout {
        id: layout

        spacing: 0

        FormCard.FormHeader {
            title: i18nc("@title, notes map section", "Existing notes")
            Layout.fillWidth: true
        }

        FormCard.FormCard {
            Layout.fillWidth: true

            Repeater {
                model: existingLinks 

                delegate: NotesMapEntry {
                    height: Kirigami.Units.gridUnit * 3
                    onClicked: {
                        if (headerExists) {
                            parser.headerInfo = [header, headerLevel.toString()]
                        }
                        const sidebar = applicationWindow().globalDrawer
                        const noteModelIndex = sidebar.treeModel.getNoteModelIndex(notePath)
                        sidebar.askForFocus(noteModelIndex)
                        noteMap.close()
                    }
                }
            }
        }

        FormCard.FormHeader {
            title: i18nc("@title, notes map section", "Missing notes")
            Layout.fillWidth: true
        }

        FormCard.FormCard {
            Layout.fillWidth: true

            Repeater {
                model: missingLinks 

                delegate: NotesMapEntry {
                    height: Kirigami.Units.gridUnit * 3
                    onClicked: showPassiveNotification(i18nc("@notification, error message %1 is a path", "%1 doesn't exists", displayedPath))
                } 
            }
        }

        KSortFilterProxyModel {
            id: missingLinks

            sourceModel: applicationWindow().noteMapper
            filterRoleName: "exists"
            filterString: "No"
            filterCaseSensitivity: Qt.CaseInsensitive
        }

        KSortFilterProxyModel {
            id: existingLinks

            sourceModel: applicationWindow().noteMapper
            filterRoleName: "exists"
            filterString: "Yes"
            filterCaseSensitivity: Qt.CaseInsensitive
        }
    }
}

