/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/
#include "noteMapper.h"
#include "kleverconfig.h"
#include <QDebug>

LinkedNoteItem::LinkedNoteItem(const QString &path, const QString &exists, const QString &header, const bool headerExists)
    : m_exists(exists)
    , m_header(header)
    , m_headerExists(headerExists)
{
    updatePath(path);
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

    case NoteMapper::HeaderRole:
        return m_header;

    case NoteMapper::HeaderExistsRole:
        return m_headerExists;

    default:
        Q_UNREACHABLE();
    }
    return 0;
};

void LinkedNoteItem::updatePath(const QString &path)
{
    m_path = path;
    QString toDisplay(path);
    setDisplayPath(toDisplay);
}

void LinkedNoteItem::setDisplayPath(QString &path)
{
    path.replace(".BaseCategory", KleverConfig::defaultCategoryDisplayNameValue()).remove(QStringLiteral(".BaseGroup/"));
    m_displayPath = path;
}

void LinkedNoteItem::updateExists(const QString &exists)
{
    m_exists = exists;
}

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
    return {{DisplayedPathRole, "displayedPath"}, {PathRole, "realPath"}, {ExistsRole, "exists"}, {HeaderRole, "header"}, {HeaderExistsRole, "headerExists"}};
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

void NoteMapper::addRow(const QString &path, const QString &header)
{
    QPair<QString, QString> pathHeaderPair = qMakePair(path, header);
    if (m_existingPathHeaderPair.contains(pathHeaderPair))
        return;

    m_existingPathHeaderPair.insert(pathHeaderPair);

    QString exists;
    bool headerExists = false;
    if (m_treeViewPaths.contains(path)) {
        exists = QStringLiteral("Yes");
        if (!header.isEmpty()) {
            // check for header here
        }
    } else {
        exists = QStringLiteral("No");
    }

    auto newRow = std::make_unique<LinkedNoteItem>(path, exists, header, headerExists);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_list.push_back(std::move(newRow));
    endInsertRows();
}

// Treeview
void NoteMapper::addGlobalPath(const QString &path)
{
    m_treeViewPaths.insert(path);

    for (auto it = m_list.cbegin(); it != m_list.cend(); it++) {
        auto child = static_cast<LinkedNoteItem *>(it->get());

        if (child->data(PathRole).toString() == path && child->data(ExistsRole) != QStringLiteral("Yes")) {
            child->updateExists(QStringLiteral("Yes"));
            // Check if header exists
            QModelIndex childIndex = createIndex(0, 0, child);
            Q_EMIT dataChanged(childIndex, childIndex);
        }
    }
}

void NoteMapper::updateGlobalPath(const QString &oldPath, const QString &newPath)
{
    if (!m_treeViewPaths.remove(oldPath))
        return;

    m_treeViewPaths.insert(newPath);
    for (auto it = m_list.cbegin(); it != m_list.cend(); it++) {
        auto child = static_cast<LinkedNoteItem *>(it->get());

        bool needUpdate = false;
        if (child->data(PathRole).toString() == oldPath) {
            needUpdate = true;
            child->updatePath(newPath);
        }
        if (child->data(PathRole).toString() == newPath) {
            needUpdate = true;
            child->updateExists(QStringLiteral("Yes"));
        }

        if (needUpdate) {
            QModelIndex childIndex = createIndex(0, 0, child);
            Q_EMIT dataChanged(childIndex, childIndex);
        }
    }
}

void NoteMapper::removeGlobalPath(const QString &path)
{
    if (!m_treeViewPaths.contains(path))
        return;

    m_treeViewPaths.erase(m_treeViewPaths.find(path));

    for (auto it = m_list.cbegin(); it != m_list.cend();) {
        auto child = static_cast<LinkedNoteItem *>(it->get());

        if (child->data(PathRole).toString() == path) {
            beginRemoveRows(QModelIndex(), it - m_list.begin(), it - m_list.begin());
            m_list.erase(it);
            endRemoveRows();
        } else {
            it++;
        }
    }
}

// Parser
void NoteMapper::addNotePaths(const QStringList &notePathHeaderPairs)
{
    Q_ASSERT(notePathHeaderPairs.size() % 2 == 0);

    if (notePathHeaderPairs == m_notePathHeaderPairs)
        return;

    clear();
    m_notePathHeaderPairs = notePathHeaderPairs;

    for (int i = 0; i < notePathHeaderPairs.size(); i += 2) {
        addRow(notePathHeaderPairs[i], notePathHeaderPairs[i+1]);
    }
}
