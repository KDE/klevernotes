// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "notelistmodel.h"
#include "kleverconfig.h"
#include <QDir>
#include <QFileInfo>
#include <QIcon>

TreeItem::TreeItem(const QString &path, TreeItem *parent)
    : m_parentItem(parent)
    , m_path(path)
{
    QFileInfo fileInfo(path);
    Q_ASSERT(fileInfo.exists());

    const QString fileName = fileInfo.fileName();
    m_displayName = fileName == QStringLiteral(".BaseCategory") ? KleverConfig::categoryDisplayName() : fileName;

    const QFileInfoList fileList = QDir(path).entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries | QDir::Filter::AccessMask);

    for (const QFileInfo &file : fileList) {
        QString name = file.fileName();

        if (name == QStringLiteral("Images")) {
            continue;
        }

        appendChild(std::make_unique<TreeItem>(file.absoluteFilePath(), this));

        qDebug() << file;
    }
}

void TreeItem::appendChild(std::unique_ptr<TreeItem> &&item)
{
    m_childItems.push_back(std::move(item));
}

TreeItem *TreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size()) {
        return nullptr;
    }
    return m_childItems.at(row).get();
}

int TreeItem::childCount() const
{
    return m_childItems.size();
}

int TreeItem::row() const
{
    if (m_parentItem) {
        const auto it = std::find_if(m_parentItem->m_childItems.cbegin(), m_parentItem->m_childItems.cend(), [this](const std::unique_ptr<TreeItem> &treeItem) {
            return treeItem.get() == const_cast<TreeItem *>(this);
        });

        if (it != m_parentItem->m_childItems.cend()) {
            return std::distance(m_parentItem->m_childItems.cbegin(), it);
        }
        Q_ASSERT(false); // should not happen
        return -1;
    }

    return 0;
}

QVariant TreeItem::data(int role) const
{
    qDebug() << role << m_path;
    switch (role) {
    case NoteListModel::PathRole:
        return m_path;

    case Qt::DisplayRole:
    case NoteListModel::DisplayNameRole:
        return m_displayName;

    case Qt::DecorationRole:
        return QIcon::fromTheme(QStringLiteral("document-edit-sign"));

    case NoteListModel::IconNameRole:
        return QStringLiteral("document-edit-sign");

    default:
        return {};
    }
};

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

NoteListModel::NoteListModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_rootItem(std::make_unique<TreeItem>(KleverConfig::storagePath()))
{
}

QModelIndex NoteListModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    TreeItem *parentItem;

    if (!parent.isValid()) {
        parentItem = m_rootItem.get();
    } else {
        parentItem = static_cast<TreeItem *>(parent.internalPointer());
    }

    TreeItem *childItem = parentItem->child(row);
    if (childItem) {
        return createIndex(row, column, childItem);
    }
    return {};
}

QHash<int, QByteArray> NoteListModel::roleNames() const
{
    return {
        {DisplayNameRole, "displayName"},
        {PathRole, "path"},
        {IconNameRole, "iconName"},
    };
}

QModelIndex NoteListModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return {};
    }

    auto childItem = static_cast<TreeItem *>(index.internalPointer());
    auto parentItem = childItem->parentItem();

    if (parentItem == m_rootItem.get()) {
        return {};
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int NoteListModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0) {
        return 0;
    }

    if (!parent.isValid()) {
        parentItem = m_rootItem.get();
    } else {
        parentItem = static_cast<TreeItem *>(parent.internalPointer());
    }

    return parentItem->childCount();
}

int NoteListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant NoteListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    auto item = static_cast<TreeItem *>(index.internalPointer());

    return item->data(role);
}
