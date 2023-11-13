/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#include "noteMapper.h"
#include <qobjectdefs.h>

NoteMapper::NoteMapper(QObject *parent)
    : QObject(parent)
{
}

QStringList NoteMapper::getAbsentLinkedPaths()
{
    return m_absentLinkedPath.isEmpty() ? QStringList() : m_absentLinkedPath.values();
}

QStringList NoteMapper::getExistingLinkedPaths()
{
    return m_existingLinkedPath.isEmpty() ? QStringList() : m_existingLinkedPath.values();
}

void NoteMapper::filterLinkedPath()
{
    m_existingLinkedPath = m_treeViewPaths & m_linkedNotePaths; // Intersect but doesn't modify m_treeViewPaths
    m_absentLinkedPath = m_linkedNotePaths - m_existingLinkedPath; // Substract but doesn't modify m_linkedNotePaths

    Q_EMIT absentLinksChanged();
    Q_EMIT existingLinksChanged();
}

// Treeview
void NoteMapper::addGlobalPath(const QString &path)
{
    m_treeViewPaths.insert(path);
}

void NoteMapper::updateGlobalPath(const QString &oldPath, const QString &newPath)
{
    if (!m_treeViewPaths.contains(oldPath))
        return;

    m_treeViewPaths.erase(m_treeViewPaths.find(oldPath));
    m_treeViewPaths.insert(newPath);
}

void NoteMapper::removeGlobalPath(const QString &path)
{
    if (!m_treeViewPaths.contains(path))
        return;

    m_treeViewPaths.erase(m_treeViewPaths.find(path));
}

// Parser
void NoteMapper::addNotePaths(const QStringList &notePaths)
{
    m_linkedNotePaths.clear();
    m_linkedNotePaths = QSet(notePaths.begin(), notePaths.end());
    filterLinkedPath();
}
