import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.3
import org.kde.Klever 1.0

Controls.ScrollView {
    id: tree
    width: parent.width
    property alias model: columnRepeater.model;
    property QtObject currentlySelected: column.children[0];
    property bool itemEdtiting : false

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
                    return (parPath != undefined) ? parPath : KleverUtility.getPath(Config.storagePath)
                }
                property string name : modelData.name
                readonly property string path: parentPath+"/"+name

                readonly property string useCase: modelData.useCase

                readonly property QtObject mouseArea: controlRoot
                readonly property QtObject textDisplay: textDisplay
                Rectangle{
                    width: tree.width
                    height: textDisplay.height

                    color: (tree.currentlySelected === infoRow && !tree.itemEdtiting)
                                ? Kirigami.Theme.highlightColor
                                : "transparent"

                    Kirigami.Icon {
                        id: carot
                        x: 20 * modelData.lvl
                        width: Kirigami.Units.iconSizes.small
                        height: textDisplay.height
                        source: infoRow.expanded ? "go-down-symbolic" : "go-next-symbolic"
                        isMask: true
                        color: !(tree.currentlySelected === infoRow)
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


                    Controls.TextField {
                        id: textDisplay
                        x: carot.x + 20
                        width: parent.width-x
                        font.pointSize: 12
                        visible: parent.visible

                        property string oldText

                        autoScroll:false

                        color: !(tree.currentlySelected === infoRow)
                                ?(controlRoot.hovered)
                                    ? Kirigami.Theme.highlightColor
                                    : Kirigami.Theme.textColor
                                : Kirigami.Theme.textColor

                        text: (infoRow.name == ".BaseCategory")
                                    ? Config.categoryDisplayName
                                    : infoRow.name


                        readOnly:(!tree.itemEdtiting && (tree.currentlySelected === infoRow))
                        enabled: (tree.currentlySelected === infoRow) || !tree.itemEdtiting

                        background: Rectangle {
                            width: parent.width - parent.x

                            color: "transparent"
                            border.color: (tree.itemEdtiting && tree.currentlySelected === infoRow)
                                                ? Kirigami.Theme.focusColor
                                                : "transparent"
                        }

                        Keys.onEscapePressed: {tree.itemEdtiting = false; text = oldText ; oldText = ""}

                        function checkRename(newPath){
                            let can = true
                            let info = ""
                            if (KleverUtility.exist(newPath)) {can = false ; info = 'exist'}

                            if (textDisplay.text === Config.categoryDisplayName) {can = false ; info = 'exist'}

                            if (infoRow.name === ".BaseCategory") info = 'base'

                            return {'can':can,'info':info}
                        }

                        function handleRename(){
                            const newPath = parentPath+"/"+textDisplay.text
                            const result = checkRename(newPath)

                            if (!result.can) {

                                let component = Qt.createComponent("qrc:/contents/ui/dialogs/RenameErrorDialog.qml")

                                if (component.status == Component.Ready) {
                                    var dialog = component.createObject(root);

                                    dialog.textDisplay = textDisplay
                                    dialog.useCase = infoRow.useCase
                                    dialog.open()
                                }
                                return
                            }

                            if (result.info === "base") {
                                Config.categoryDisplayName = textDisplay.text
                            }
                            else{
                                StorageHandler.rename(infoRow.path,newPath)
                                infoRow.name = textDisplay.text
                            }

                            tree.itemEdtiting = false
                        }

                        Keys.onReturnPressed: handleRename()
                        Keys.onEnterPressed: handleRename()

                        onReadOnlyChanged: {
                            const oldName = text
                            oldText = oldName
                        }

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
                                tree.currentlySelected = infoRow
                        }
                        enabled: !tree.itemEdtiting
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
