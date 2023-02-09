// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0

Kirigami.ScrollablePage {
    title: i18nc("@title:window", "Settings")

    Kirigami.FormLayout {
        Row {
            Kirigami.FormData.label: "Storage path:"
            Controls.TextField {
                text: Config.storagePath
                readOnly: true
            }
            Controls.Button{
                icon.name: "document-open-folder"
            }
        }
        Item {
            // Kirigami.FormData.label: "Section Title"
            Kirigami.FormData.isSection: true
        }
        Controls.TextField {
            Kirigami.FormData.label: "New Category name:"
            text: Config.defaultCategoryName
        }
        Controls.TextField {
            Kirigami.FormData.label: "New Group name:"
            text: Config.defaultGroupName
        }
        Controls.TextField {
            Kirigami.FormData.label: "New Note name:"
            text: Config.defaultNoteName
        }

    }
}


