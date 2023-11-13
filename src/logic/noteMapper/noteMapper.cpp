/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#include "noteMapper.h"
#include <qmap.h>
#include <qobject.h>
#include <qobjectdefs.h>

NoteMapper::NoteMapper(QObject *parent)
    : QObject(parent)
{
}

LinkedNotesModel *NoteMapper::getAbsentLinkedPaths()
{
    return &m_absentLinkedPath;
}

LinkedNotesModel *NoteMapper::getExistingLinkedPaths()
{
    return &m_existingLinkedPath;
}

void NoteMapper::filterLinkedPath()
{
    m_existingLinkedPath.clear();
    for (auto it = m_treeViewPaths.constBegin(); it != m_treeViewPaths.constEnd(); it++) {
        if (m_linkedNotePaths.contains(it.key())) {
            m_existingLinkedPath.addRow(it.key(), it.value());
        }
    }
    Q_EMIT existingLinksChanged();

    m_absentLinkedPath.clear();
    for (auto it = m_linkedNotePaths.constBegin(); it != m_linkedNotePaths.constEnd(); it++) {
        if (m_existingLinkedPath.hasPath(it.key()))
            continue;
        m_absentLinkedPath.addRow(it.key(), it.value().toString());
    }
    Q_EMIT absentLinksChanged();
}

// Treeview
void NoteMapper::addGlobalPath(const QString &path, const QString &displayedPath)
{
    m_treeViewPaths[path] = displayedPath;
}

void NoteMapper::updateGlobalPath(const QString &oldPath, const QString &newPath, const QString &displayedPath)
{
    if (!m_treeViewPaths.contains(oldPath))
        return;

    m_treeViewPaths.erase(m_treeViewPaths.find(oldPath));
    m_treeViewPaths[newPath] = displayedPath;
}

void NoteMapper::removeGlobalPath(const QString &path)
{
    if (!m_treeViewPaths.contains(path))
        return;

    m_treeViewPaths.erase(m_treeViewPaths.find(path));
}

// Parser
void NoteMapper::addNotePaths(const QVariantMap &notePaths)
{
    m_linkedNotePaths = notePaths;
    filterLinkedPath();
}
