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
    explicit LinkedNoteItem(const QString &path, const QString &displayedPath, const QString &exists);

    QVariant data(int role) const;

private:
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
    bool hasPath(const QString &path);

    // Treeview
    Q_INVOKABLE void addGlobalPath(const QString &path, const QString &displayedPath);
    Q_INVOKABLE void updateGlobalPath(const QString &oldPath, const QString &newPath, const QString &displayedPath);
    Q_INVOKABLE void removeGlobalPath(const QString &path);

    // Parser
    Q_INVOKABLE void addNotePaths(const QVariantMap &notePaths);

private:
    // void filterLinkedPath();

    std::vector<std::unique_ptr<LinkedNoteItem>> m_list;

    // Parser
    QVariantMap m_linkedNotePaths; // Stored to avoid unnecessary looping

    // Treeview
    QMap<QString, QString> m_treeViewPaths;
};
/*

#include "linkedNotesModel.h"
#include <QDebug>
#include <QMap>
#include <QObject>
#include <QString>

class NoteMapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LinkedNotesModel *absentLinks READ getAbsentLinkedPaths NOTIFY absentLinksChanged)
    Q_PROPERTY(LinkedNotesModel *existingLinks READ getExistingLinkedPaths NOTIFY existingLinksChanged)
public:
    NoteMapper(QObject *parent = nullptr);

    LinkedNotesModel *getExistingLinkedPaths();
    LinkedNotesModel *getAbsentLinkedPaths();

    // Treeview
    Q_INVOKABLE void addGlobalPath(const QString &path, const QString &displayedPath);
    Q_INVOKABLE void updateGlobalPath(const QString &oldPath, const QString &newPath, const QString &displayedPath);
    Q_INVOKABLE void removeGlobalPath(const QString &path);

    // Parser
    Q_INVOKABLE void addNotePaths(const QVariantMap &notePaths);

signals:
    void absentLinksChanged();
    void existingLinksChanged();

private:
    void filterLinkedPath();

    LinkedNotesModel m_existingLinkedPath;
    LinkedNotesModel m_absentLinkedPath;

    // Parser
    QVariantMap m_linkedNotePaths;

    // Treeview
    QMap<QString, QString> m_treeViewPaths;
}; */
