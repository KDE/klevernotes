// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15 as Controls

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard
import org.kde.kitemmodels 1.0

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

