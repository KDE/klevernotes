/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#include "linkedNotesModel.h"
#include <QDebug>

LinkedNoteItem::LinkedNoteItem(const QString &path, const QString &displayedPath)
    : m_path(path)
    , m_displayPath(displayedPath)
{
}

QVariant LinkedNoteItem::data(int role) const
{
    switch (role) {
    case LinkedNotesModel::PathRole:
        return m_path;

    case LinkedNotesModel::DisplayedPathRole:
        return m_displayPath;

    default:
        Q_UNREACHABLE();
    }
    return 0;
};

LinkedNotesModel::LinkedNotesModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex LinkedNotesModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return createIndex(row, column, static_cast<LinkedNoteItem *>(m_list.at(row).get()));
}

QHash<int, QByteArray> LinkedNotesModel::roleNames() const
{
    return {{DisplayedPathRole, "displayedPath"}, {PathRole, "realPath"}};
}

QModelIndex LinkedNotesModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return {};
}

int LinkedNotesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_list.size();
}

int LinkedNotesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant LinkedNotesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    const auto item = static_cast<LinkedNoteItem *>(index.internalPointer());

    return item->data(role);
}

void LinkedNotesModel::clear()
{
    beginResetModel();
    m_list.clear();
    endResetModel();
}

void LinkedNotesModel::addRow(const QString &path, const QString &displayedPath)
{
    m_pathsList.append(path);
    auto newRow = std::make_unique<LinkedNoteItem>(path, displayedPath);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_list.push_back(std::move(newRow));
    endInsertRows();
}

bool LinkedNotesModel::hasPath(const QString &path)
{
    return m_pathsList.contains(path);
}
