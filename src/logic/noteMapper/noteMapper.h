/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/
#pragma once

#include <QAbstractItemModel>
#include <QSet>
#include <memory>

class LinkedNoteItem
{
public:
    explicit LinkedNoteItem(const QString &path, const QString &exists);

    QVariant data(int role) const;
    void updatePath(const QString &path);
    void updateExists(const QString &exists);

private:
    void setDisplayPath(QString &path);
    QString m_path;
    QString m_displayPath;
    QString m_exists; // QString because we use a KSortFilterProxyModel
};

class NoteMapper : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit NoteMapper(QObject *parent = nullptr);

    enum ExtraRoles {
        PathRole = Qt::UserRole + 1, // To get a string with the fullPath of the Category/Group/Note
        DisplayedPathRole, // To get a string with the name of the Category/Group/Note to be displayed instead of the hidden name
        ExistsRole, // To know if the path exist or not
    };

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QHash<int, QByteArray> roleNames() const override;

    void clear();
    void addRow(const QString &path, const QString &displayedPath);

    // Treeview
    Q_INVOKABLE void addGlobalPath(const QString &path);
    Q_INVOKABLE void updateGlobalPath(const QString &oldPath, const QString &newPath);
    Q_INVOKABLE void removeGlobalPath(const QString &path);

    // Parser
    Q_INVOKABLE void addNotePaths(const QVariantMap &notePaths);

private:
    std::vector<std::unique_ptr<LinkedNoteItem>> m_list;

    // Parser
    QVariantMap m_linkedNotePaths; // Stored to avoid unnecessary looping

    // Treeview
    QSet<QString> m_treeViewPaths;
};
