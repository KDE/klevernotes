import QtQuick 2.15
import QtQuick.Controls 2.2
import org.kde.Klever 1.0

ScrollView {
    id: view

    property string path
    onPathChanged: textArea.text = DocumentHandler.readNote(path)
    property string text: textArea.text

    readonly property TextArea textArea: textArea

    TextArea{
        id: textArea
        persistentSelection: true

        wrapMode: TextEdit.WrapAnywhere
    }
}
