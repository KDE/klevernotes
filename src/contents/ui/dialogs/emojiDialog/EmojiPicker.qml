// SPDX-FileCopyrightText: 2022 Tobias Fella <tobias.fella@kde.org>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

import org.kde.neochat

ColumnLayout {
    id: root

    readonly property var currentEmojiModel: EmojiModel.categories
    readonly property int categoryIconSize: Math.round(Kirigami.Units.gridUnit * 2.5)
    readonly property var currentCategory: currentEmojiModel[categories.currentIndex].category
    readonly property alias categoryCount: categories.count
    property int selectedType: 0

    signal chosen(string emoji)

    onActiveFocusChanged: if (activeFocus) {
        searchField.forceActiveFocus();
    }

    spacing: 0

    QQC2.ScrollView {
        Layout.fillWidth: true
        Layout.preferredHeight: root.categoryIconSize + QQC2.ScrollBar.horizontal.height
        QQC2.ScrollBar.horizontal.height: QQC2.ScrollBar.horizontal.visible ? QQC2.ScrollBar.horizontal.implicitHeight : 0

        ListView {
            id: categories
            clip: true
            focus: true
            orientation: ListView.Horizontal

            Keys.onReturnPressed: if (emojiGrid.count > 0) emojiGrid.focus = true
            Keys.onEnterPressed: if (emojiGrid.count > 0) emojiGrid.focus = true

            KeyNavigation.down: emojiGrid.count > 0 ? emojiGrid : categories
            KeyNavigation.tab: emojiGrid.count > 0 ? emojiGrid : categories

            keyNavigationEnabled: true
            keyNavigationWraps: true
            Keys.forwardTo: searchField
            interactive: width !== contentWidth

            model: root.currentEmojiModel
            Component.onCompleted: categories.forceActiveFocus()

            delegate: root.selectedType === 0 ? emojiDelegate : stickerDelegate
        }
    }

    Kirigami.Separator {
        Layout.fillWidth: true
        Layout.preferredHeight: 1
    }

    Kirigami.SearchField {
        id: searchField
        Layout.margins: Kirigami.Units.smallSpacing
        Layout.fillWidth: true
        visible: selectedType === 0

        /**
         * The focus is manged by the parent and we don't want to use the standard
         * shortcut as it could block other SearchFields from using it.
         */
        focusSequence: ""
    }

    EmojiGrid {
        id: emojiGrid
        targetIconSize: root.currentCategory === EmojiModel.Custom ? Kirigami.Units.gridUnit * 3 : root.categoryIconSize  // Custom emojis are bigger
        model: root.selectedType === 1 ? emoticonFilterModel : searchField.text.length === 0 ? EmojiModel.emojis(root.currentCategory) : EmojiModel.filterModelNoCustom(searchField.text, false)
        Layout.fillWidth: true
        Layout.fillHeight: true
        onChosen: unicode => root.chosen(unicode)
        header: categories
        Keys.forwardTo: searchField
    }

    Component {
        id: emojiDelegate
        Kirigami.NavigationTabButton {
            width: root.categoryIconSize
            height: width
            checked: categories.currentIndex === model.index
            text: modelData ? modelData.emoji : ""
            QQC2.ToolTip.text: modelData ? modelData.name : ""
            QQC2.ToolTip.delay: Kirigami.Units.toolTipDelay
            QQC2.ToolTip.visible: hovered
            onClicked: {
                categories.currentIndex = index;
                categories.focus = true;
            }
        }
    }
}
