import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.1
import org.kde.kirigami 2.19 as Kirigami
import QtGraphicalEffects 1.12

Kirigami.NavigationTabBar {
    id: tool
    visible: !applicationWindow().wideScreen

    actions: [
        Kirigami.Action {
            iconName: "clock"
            text: i18n("Note")
            // checked: getTimePage() === pageStack.currentItem
            // onTriggered: {
            //     if (getTimePage()  !== pageStack.currentItem) {
            //         applicationWindow().switchToPage(getTimePage(), 0);
            //     }
            // }
        },
        Kirigami.Action {
            iconName: "player-time"
            text: i18n("TODO")
            // checked: getTimersPage() === pageStack.currentItem
            // onTriggered: {
            //     if (getTimersPage() !== pageStack.currentItem) {
            //         applicationWindow().switchToPage(getTimersPage(), 0);
            //     }
            // }
        },
        Kirigami.Action {
            iconName: "chronometer"
            text: i18n("Documentation")
            // checked: getStopwatchPage() === pageStack.currentItem
            // onTriggered: {
            //     if (getStopwatchPage() !== pageStack.currentItem) {
            //         applicationWindow().switchToPage(getStopwatchPage(), 0);
            //     }
            // }
        }
    ]
}

