import QtQuick 2.6
import QtQuick.Layouts 1.4
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.13 as Kirigami

Item {
    // property alias contentItem: content.contentItem;
    property string title: ""
    Item {
        id: titleBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30
        Row {
            anchors.fill: parent
            Controls.CheckBox{
                id:expand
                checked: true
            }
            Text {
                Layout.alignment: Qt.AlignLeft
                text: title
            }
        }
    }
    Controls.Pane {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: titleBar.bottom
        anchors.bottom: parent.bottom
        topPadding: 0
        visible: expand.checked
        id: content
    }
}
