
import org.kde.kirigami 2.19 as Kirigami

import org.kde.Klever 1.0
import "qrc:/contents/ui/textEditor"

Kirigami.Page {
    id: page

    title: i18n("Main Page")

    Editor{
        id:editor
    }

    footer: BottomToolBar{}
}
