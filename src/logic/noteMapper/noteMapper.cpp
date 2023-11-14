/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/
#include "noteMapper.h"
#include <QDebug>
#include <qstringliteral.h>

LinkedNoteItem::LinkedNoteItem(const QString &path, const QString &displayedPath, const QString &exists)
    : m_path(path)
    , m_displayPath(displayedPath)
    , m_exists(exists)
{
}

QVariant LinkedNoteItem::data(int role) const
{
    switch (role) {
    case NoteMapper::PathRole:
        return m_path;

    case NoteMapper::DisplayedPathRole:
        return m_displayPath;

    case NoteMapper::ExistsRole:
        return m_exists;

    default:
        Q_UNREACHABLE();
    }
    return 0;
};

NoteMapper::NoteMapper(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex NoteMapper::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return createIndex(row, column, static_cast<LinkedNoteItem *>(m_list.at(row).get()));
}

QHash<int, QByteArray> NoteMapper::roleNames() const
{
    return {{DisplayedPathRole, "displayedPath"}, {PathRole, "realPath"}, {ExistsRole, "exists"}};
}

QModelIndex NoteMapper::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return {};
}

int NoteMapper::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_list.size();
}

int NoteMapper::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant NoteMapper::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    const auto item = static_cast<LinkedNoteItem *>(index.internalPointer());

    return item->data(role);
}

void NoteMapper::clear()
{
    beginResetModel();
    m_list.clear();
    endResetModel();
}

void NoteMapper::addRow(const QString &path, const QString &displayedPath)
{
    QString exists = m_treeViewPaths.contains(path) ? QStringLiteral("Yes") : QStringLiteral("No");

    auto newRow = std::make_unique<LinkedNoteItem>(path, displayedPath, exists);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_list.push_back(std::move(newRow));
    endInsertRows();
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
    if (notePaths == m_linkedNotePaths)
        return;

    clear();
    m_linkedNotePaths = notePaths;

    for (auto it = notePaths.constBegin(); it != notePaths.constEnd(); it++) {
        addRow(it.key(), it.value().toString());
    }
}
