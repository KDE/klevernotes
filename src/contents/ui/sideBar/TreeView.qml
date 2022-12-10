import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3
import org.kde.Klever 1.0

Controls.ScrollView {
    id: root
    width: parent.width
    property alias model: columnRepeater.model;
    property QtObject currentlySelected: column.children[0];

    Column{
        id: column
        width: parent.width

        Repeater {
            id: columnRepeater
            delegate: accordion
        }

        Component {
            id: accordion

            Column {
                id: infoRow

                width: parent.width
                height: childrenRect.height

                property bool expanded: false

                property QtObject parentRow: parent.parent.parent
                // Not super clean but this make it easier to handle rename
                property string parentPath: {
                    // This chain of parent let's us acces the upper infoRow holding the current one (if it exist)
                    const parPath = parentRow.path
                    return (parPath != undefined) ? parPath : Config.path
                }

                property string name: textDisplay.text

                readonly property string path: parentPath+"/"+name
                readonly property string useCase: modelData.useCase

                readonly property QtObject mouseArea: controlRoot
                readonly property QtObject textDisplay: textDisplay
                Rectangle{
                    width: root.width
                    height: textDisplay.height

                    color: (root.currentlySelected === infoRow)
                                ? Kirigami.Theme.highlightColor
                                : "transparent"

                    Kirigami.Icon {
                        id: carot
                        x: 20 * modelData.lvl
                        width: Kirigami.Units.iconSizes.small
                        height: textDisplay.height
                        source: infoRow.expanded ? "go-down-symbolic" : "go-next-symbolic"
                        isMask: true
                        color: !(root.currentlySelected === infoRow)
                                ?(controlRoot.hovered)
                                    ? Kirigami.Theme.highlightColor
                                    : Kirigami.Theme.textColor
                                : Kirigami.Theme.textColor

                        Behavior on color {
                            ColorAnimation {
                                duration: Kirigami.Units.shortDuration;
                                easing.type: Easing.InOutQuad
                            }
                        }
                        visible: infoRow.useCase != "Note"
                    }

                    TextEdit {
                        id: textDisplay
                        x: carot.x + 20
                        font.pointSize: 12
                        visible: parent.visible

                        color: !(root.currentlySelected === infoRow)
                                ?(controlRoot.hovered)
                                    ? Kirigami.Theme.highlightColor
                                    : Kirigami.Theme.textColor
                                : Kirigami.Theme.textColor

                        text: modelData.name
                        readOnly: true

                        onTextChanged: {
                            text = text.replace(/(\r\n|\n|\r)/gm, "");
                        }
                    }

                    MouseArea {
                        id: controlRoot
                        anchors.fill: parent

                        hoverEnabled: true
                        property bool hovered : false
                        onEntered: hovered = true
                        onExited: hovered = false
                        onClicked: {
                            if(modelData.useCase != "Note")infoRow.expanded =!infoRow.expanded;
                                root.currentlySelected = infoRow
                        }
                        enabled: true
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
