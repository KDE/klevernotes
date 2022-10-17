import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami


Rectangle {
    id:root
    width: parent.width
    color: Kirigami.Theme.backgroundColor
    property alias model: columnRepeater.model
    Column{
        width: parent.width
        Repeater {
            id: columnRepeater
            delegate: accordion
        }

        Component {
            id: accordion

            Column {
                id:column
                width: parent.width

                Item {
                    id: infoRow

                    width: parent.width
                    height: childrenRect.height
                    property bool expanded: false

                    MouseArea {
                        anchors.fill: parent
                        id:controlRoot
                        hoverEnabled: true
                        property bool hovered : false
                        onEntered: hovered = true
                        onExited: hovered = false
                        onClicked: if(!modelData.isNote) infoRow.expanded = !infoRow.expanded
                        enabled: true
                    }

                    Rectangle{
                        Kirigami.Icon {
                            id: carot
                            x: 20 * modelData.lvl
                            width: Kirigami.Units.iconSizes.small
                            height: text_display.height
                            source: infoRow.expanded ? "go-down-symbolic" : "go-next-symbolic"
                            isMask: true
                            color: controlRoot.hovered
                                    ? Kirigami.Theme.activeTextColor
                                    : Kirigami.Theme.textColor
                            Behavior on color { ColorAnimation { duration: Kirigami.Units.shortDuration; easing.type: Easing.InOutQuad } }
                            visible: !modelData.isNote ? true : false
                        }

                        Text {
                            id:text_display
                            x: carot.x + 20
                            font.pointSize: 12
                            visible: parent.visible

                            color: !controlRoot.hovered
                                    ? Kirigami.Theme.textColor
                                    : carot.visible
                                        ? Kirigami.Theme.activeTextColor
                                        : Kirigami.Theme.textColor
                            text: modelData.name
                        }
                        width:root.width
                        height:childrenRect.height
                        color: !controlRoot.hovered
                                ? "transparent"
                                : carot.visible
                                    ? "transparent"
                                    : Kirigami.Theme.activeTextColor

                    }
                }

                ListView {
                    id: subentryColumn
                    width: parent.width
                    height: childrenRect.height * opacity
                    visible: opacity > 0
                    opacity: infoRow.expanded ? 1 : 0
                    delegate: accordion
                    model: modelData.content
                    interactive: false

                    Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; easing.type: Easing.InOutQuad } }
                }
            }
        }
    }
}
