/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QAbstractItemModel>
#include <QList>
#include <memory>

class LinkedNoteItem
{
public:
    explicit LinkedNoteItem(const QString &path, const QString &displayedPath);

    QVariant data(int role) const;

private:
    QString m_path;
    QString m_displayPath;
};

class LinkedNotesModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit LinkedNotesModel(QObject *parent = nullptr);

    enum ExtraRoles {
        PathRole = Qt::UserRole + 1, // For getting a string with the fullPath of the Category/Group/Note
        DisplayedPathRole, // For getting a string with the name of the Category/Group/Note to be displayed instead of the hidden name
    };

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QHash<int, QByteArray> roleNames() const override;
    void clear();
    void addRow(const QString &path, const QString &displayedPath);
    bool hasPath(const QString &path);

private:
    std::vector<std::unique_ptr<LinkedNoteItem>> m_list;
    QStringList m_pathsList;
};
