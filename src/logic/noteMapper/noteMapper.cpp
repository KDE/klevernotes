/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/
#include "noteMapper.h"
#include "kleverconfig.h"
#include <QDebug>
#include <qstringliteral.h>

LinkedNoteItem::LinkedNoteItem(const QString &path, const QString &exists, QString &header, const bool headerExists, const QString &title)
    : m_exists(exists)
    , m_headerExists(headerExists)
    , m_title(title)
{
    m_header = header.replace(QStringLiteral("#"), QStringLiteral("")).trimmed();
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

    case NoteMapper::TitleRole:
        return m_title;

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

void LinkedNoteItem::updateHeaderExists(const bool exists)
{
    m_headerExists = exists;
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
    return {{DisplayedPathRole, "displayedPath"},
            {PathRole, "realPath"},
            {ExistsRole, "exists"},
            {HeaderRole, "header"},
            {HeaderExistsRole, "headerExists"},
            {TitleRole, "title"}};
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
    m_existingLinkedNoteInfos.clear();
}

void NoteMapper::addRow(const QString &path, QString &header, const QString &title)
{
    std::tuple<QString, QString, QString> linkedNoteInfo = {path, header, title};
    if (m_existingLinkedNoteInfos.find(linkedNoteInfo) != m_existingLinkedNoteInfos.cend()) // Can't use .contains (C++ 20)
        return;

    m_existingLinkedNoteInfos.insert(linkedNoteInfo);

    QString exists;
    bool headerExists = false;
    if (m_treeViewPaths.contains(path)) {
        exists = QStringLiteral("Yes");
        if (!header.isEmpty() && header.startsWith(QStringLiteral("#"))) {
            QString filePath = KleverConfig::storagePath() + path + QStringLiteral("/note.md");
            headerExists = m_headerChecker->readFile(filePath, header) == QStringLiteral("true");
        }
    } else {
        exists = QStringLiteral("No");
    }

    auto newRow = std::make_unique<LinkedNoteItem>(path, exists, header, headerExists, title);

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
            // Don't need to check for header since this is a brand new file
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
void NoteMapper::addNotePaths(const QStringList &linkedNoteInfos)
{
    Q_ASSERT(linkedNoteInfos.size() % 3 == 0);

    if (linkedNoteInfos == m_previousLinkedNoteInfos)
        return;

    clear();
    m_previousLinkedNoteInfos = linkedNoteInfos;

    for (int i = 0; i < m_previousLinkedNoteInfos.size(); i += 3) {
        addRow(m_previousLinkedNoteInfos[i], m_previousLinkedNoteInfos[i + 1], m_previousLinkedNoteInfos[i + 2]);
    }
}
