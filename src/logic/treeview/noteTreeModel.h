// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

#include <QAbstractItemModel>
#include <QFileInfo>
#include <memory>

class NoteTreeModel;
class TreeItem
{
public:
    explicit TreeItem(const QString &path, const int &depth_level, NoteTreeModel *model, TreeItem *parentItem = nullptr);

    void appendChild(std::unique_ptr<TreeItem> &&child);

    TreeItem *child(int row);
    std::unique_ptr<TreeItem> uniqueChildAt(int row);
    int childCount() const;
    QVariant data(int role) const;
    int row() const;
    TreeItem *parentItem();
    void remove();
    void changeDisplayName(const QString &name);
    void changePath(const QString &newPart, const QModelIndex &parentModelIndex, int newPartIdx = -1);
    void askForFocus(const QModelIndex &itemIndex);
    void askForExpand(const QModelIndex &itemIndex);

private:
    // Position in tree
    std::vector<std::unique_ptr<TreeItem>> m_childItems;
    TreeItem *m_parentItem;

    NoteTreeModel *m_model;

    // Content
    QString m_path;
    int m_depth_level;
    QString m_displayName;
    bool m_wantFocus = false;
    bool m_wantExpand = false;
};

class NoteTreeModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(bool noteMapEnabled WRITE setNoteMapEnabled) // QML will handle the signal and change it for us
public:
    explicit NoteTreeModel(QObject *parent = nullptr);

    enum ExtraRoles {
        PathRole = Qt::UserRole + 1, // For getting a string with the fullPath of the Category/Group/Note
        DisplayNameRole, // For getting a string with the name of the Category/Group/Note to be displayed instead of the hidden name
        IconNameRole, // For getting a string with the icon name associated with the Category/Group/Note
        UseCaseRole, // For getting a string to know if the item is a Category/Group/Note
        NoteNameRole, // For only filtering between notes in the searchBar
        BranchNameRole, // For getting the name of the category + group of a note
        WantFocusRole, // For sending a signal to the qml ItemDelegate using dataChanged, asking for focus
        WantExpandRole, // For sending a signal to the qml ItemDelegate using dataChanged, asking to expands
    };

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE void addRow(const QString &rowName, const QString &path, const int rowLevel, const QModelIndex &parentModelIndex = QModelIndex());
    Q_INVOKABLE void removeFromTree(const QModelIndex &index);
    Q_INVOKABLE void rename(const QModelIndex &rowModelIndex, const QString &newName);
    Q_INVOKABLE void askForFocus(const QModelIndex &rowModelIndex);
    Q_INVOKABLE void askForExpand(const QModelIndex &rowModelIndex);
    Q_INVOKABLE void initModel();
    Q_INVOKABLE QModelIndex getNoteModelIndex(const QString &notePath);

    // NoteMapper
    void setNoteMapEnabled(const bool noteMapEnabled);
    bool noteMapEnabled();
    bool isInit();
    void addInitialGlobalPath(const QString &path);

signals:
    void errorOccurred(const QString &errorMessage);
    // NoteMapper
    void newGlobalPathFound(const QString &path);
    void globalPathUpdated(const QString &oldPath, const QString &newPath);
    void globalPathRemoved(const QString &path);
    void initialGlobalPathsSent(const QStringList &initialGlobalPaths);

private:
    // NoteMapper
    bool m_noteMapEnabled;
    bool m_isInit = false;
    QStringList m_initialGlobalPaths;

    QString m_path;
    std::unique_ptr<TreeItem> m_rootItem;
    QFileInfo m_fileInfo;

    // Storage Handler
    bool makeStorage(const QString &storagePath);
    bool makeCategory(const QString &storagePath, const QString &categoryName);
    bool makeGroup(const QString &categoryPath, const QString &groupName);
    bool makeNote(const QString &groupPath, const QString &noteName);
};
