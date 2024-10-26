// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

import QtQuick
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

Kirigami.AbstractCard {
    id: card

    property int ogIndex: model.ogIndex
    property bool isVisible

    signal moveItem(int indexTo)
    signal changeList

    verticalPadding: 0
    horizontalPadding: 0

    contentItem: RowLayout {
        spacing: 0
        ClickableArrow {
            id: downArrow

            arrowDirection: Qt.DownArrow
            visible: isVisible

            Layout.fillHeight: true
            Layout.preferredWidth: Kirigami.Units.gridUnit * 2

            onClicked: {
                moveItem(index + 1)
            }
        }
        
        ClickableArrow {
            id: upArrow

            arrowDirection: Qt.UpArrow
            visible: isVisible

            Layout.fillHeight: true
            Layout.preferredWidth: Kirigami.Units.gridUnit * 2

            onClicked: {
                moveItem(index - 1)
            }    
        }

        Kirigami.Heading {
            text: model.index + 1
            level: 3
            visible: isVisible
            Layout.preferredWidth: Kirigami.Units.gridUnit + Kirigami.Units.largeSpacing
            Layout.leftMargin: Kirigami.Units.largeSpacing
        }

        Kirigami.Separator {
            Layout.fillHeight: true
            Layout.margins: Kirigami.Units.largeSpacing
            visible: isVisible
        }

        Kirigami.Icon {
            source: model.iconName
            visible: source.length !== 0
            Layout.margins: Kirigami.Units.largeSpacing
            Layout.rightMargin: 0
            Layout.fillHeight: true
            Layout.preferredWidth: Kirigami.Units.gridUnit
        }

        Kirigami.Heading {
            text: model.description
            Layout.leftMargin: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
        }

        ClickableIcon {
            icon.name: card.isVisible ? "quickview-symbolic" : "view-hidden-symbolic"
            Layout.fillHeight: true
            Layout.preferredWidth: height

            onClicked: {
                changeList()
            }
        }
    }
}
