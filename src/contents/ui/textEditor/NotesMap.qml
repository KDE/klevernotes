// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.19 as Kirigami
import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import org.kde.kitemmodels 1.0

import "qrc:/contents/ui/textEditor/components/"

Kirigami.OverlaySheet {
    id: noteMap

    required property QtObject parser

    title: i18nc("@window:title","Linked notes map")

    background: Rectangle {
        Kirigami.Theme.colorSet: Kirigami.Theme.Window
        Kirigami.Theme.inherit: false
        color: Kirigami.Theme.backgroundColor    
    }
    
    contentItem: ColumnLayout {
        KSortFilterProxyModel {
            id: missingLinks
            sourceModel: applicationWindow().noteMapper
            
            @FILTER_NAMING@: "exists"
            filterString: "No"
            filterCaseSensitivity: Qt.CaseInsensitive
        }

        KSortFilterProxyModel {
            id: existingLinks
            sourceModel: applicationWindow().noteMapper
            
            @FILTER_NAMING@: "exists"
            filterString: "Yes"
            filterCaseSensitivity: Qt.CaseInsensitive
        }

        spacing: 0
        Layout.preferredWidth: Kirigami.Units.gridUnit * 30

        FormCard.FormHeader {
            Layout.fillWidth: true
            title: i18nc("@title, notes map section", "Existing notes")
        }

        FormCard.FormCard {
            Layout.fillWidth: true

            Repeater {
                model: existingLinks 

                delegate: NotesMapEntry {
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
            Layout.fillWidth: true
            title: i18nc("@title, notes map section", "Missing notes")
        }

        FormCard.FormCard {
            Layout.fillWidth: true

            Repeater {
                model: missingLinks 

                delegate: NotesMapEntry {
                    onClicked: showPassiveNotification(i18nc("@notification, error message %1 is a path", "%1 doesn't exists", displayedPath))
                } 
            }
        }
    }
}

