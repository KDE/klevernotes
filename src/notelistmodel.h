// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

#include <QAbstractItemModel>
#include <QFileInfo>
#include <memory>

class TreeItem
{
public:
    explicit TreeItem(const QString &path, TreeItem *parentItem = nullptr);

    void appendChild(std::unique_ptr<TreeItem> &&child);

    TreeItem *child(int row);
    int childCount() const;
    QVariant data(int role) const;
    int row() const;
    TreeItem *parentItem();

private:
    // Position in treee
    std::vector<std::unique_ptr<TreeItem>> m_childItems;
    TreeItem *m_parentItem;

    // Content
    QString m_path;
    QString m_displayName;
};

class NoteListModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit NoteListModel(QObject *parent = nullptr);

    enum ExtraRoles {
        PathRole = Qt::UserRole + 1,
        DisplayNameRole,
        IconNameRole,
        UseCaseRole,
    };

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QHash<int, QByteArray> roleNames() const override;

Q_SIGNALS:
    void pathChanged();

private:
    QString m_path;
    std::unique_ptr<TreeItem> m_rootItem;
    QFileInfo m_fileInfo;
};
