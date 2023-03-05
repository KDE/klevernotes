// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami

Column{
    id: subEntryColumn

    width: parent.width
    height: childrenRect.height * opacity

    property var entries: []
    property int delimiter: 1

    Behavior on visible { NumberAnimation { duration: Kirigami.Units.shortDuration } }
    Behavior on height { NumberAnimation { duration: Kirigami.Units.veryShortDuration } }


    function reorderEntries(useCase) {
        let firstHalf = subEntryColumn.entries.slice(0,subEntryColumn.delimiter)
        let secondHalf = subEntryColumn.entries.slice(subEntryColumn.delimiter,subEntryColumn.entries.length-1)

        if (useCase === "Note"){
            firstHalf.push(subEntryColumn.entries[subEntryColumn.entries.length-1])
            firstHalf.sort((a,b) => a.localeCompare(b))
        }
        else{
            secondHalf.push(subEntryColumn.entries[subEntryColumn.entries.length-1])
            secondHalf.sort((a,b) => a.localeCompare(b))
        }
        const ordered = firstHalf.concat(secondHalf)

        return ordered
    }

    function reorderSubEntries(useCase) {
        const reorderedEntries = reorderEntries(useCase)

        if (useCase === "Note") subEntryColumn.delimiter += 1

        let newChildren = new Array(children.length).fill(0)
        for(var i = entries.length-1 ; i >= 0 ; i--){
            const entry = entries[i]
            const reorderIndex = reorderedEntries.indexOf(entry)

            let child = children[i]

            newChildren[reorderIndex] = child
        }

        subEntryColumn.entries = reorderedEntries
        subEntryColumn.children = newChildren
    }


    function addRow(data,forcedLvl,reorder){
        subEntryColumn.entries.push(data.displayedName)

        let component = Qt.createComponent("qrc:/contents/ui/sideBar/TreeNode.qml")
        if (component.status == Component.Ready) {
            let row = component.createObject(subEntryColumn)
            row.useCase = data.useCase
            row.parentRow = subEntryColumn.parent
            row.name = data.name
            row.displayedName = data.displayedName
            row.lvl = (!forcedLvl || forcedLvl === Infinity) ? data.lvl : forcedLvl
            row.expanded = tree.currentlySelected.path.includes(row.path)

            // This property will be usefull to reorder the category while keeping notes on top when we add an elem
            if (row.useCase === "Note" && subEntryColumn.parent.useCase === "Category" && !reorder) subEntryColumn.delimiter += 1

            if (data.content.length > 0) {
                const newHolder = row.subEntryColumn
                newHolder.addRows(data)
            }
            if (reorder) {
                reorderSubEntries(row.useCase)
            }
        }
    }

    function addRows(hierarchy) {
        const content = hierarchy.content
        content.forEach(data => {
            subEntryColumn.addRow(data)
        })
    }
}
