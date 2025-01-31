// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

// KleverNotes includes
#include "kleverconfig.h"
#include "treeItem.h"

// Qt includes
#include <QAbstractItemModel>
#include <QFileInfo>

class NoteTreeModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(bool noteMapEnabled WRITE setNoteMapEnabled) // QML will handle the signal and change it for us

public:
    explicit NoteTreeModel(QObject *parent = nullptr);

    enum ExtraRoles {
        PathRole = Qt::UserRole + 1, // To get a string with the fullPath of the Note/Folder
        DirRole, // To get a string with the fullPath the folder or the folder containing the note
        DisplayNameRole, // To get a string with the name of the Category/Group/Note to be displayed instead of the hidden name
        IconNameRole, // To get a string with the icon name associated with the Category/Group/Note
        IsNote, // To know if the item is a Note (else it's a folder)
        NoteNameRole, // For only filtering between notes in the searchBar
        BranchNameRole, // To get the name of the category + group of a note
        FullNameRole, // To get the "full" name of the category + group + note of a note
        WantFocusRole, // For send a signal to the qml ItemDelegate using dataChanged, asking for focus
        WantExpandRole, // For send a signal to the qml ItemDelegate using dataChanged, asking to expands
    };

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE QModelIndex addRow(const QString &rowName, const bool isNote, const QModelIndex &parentModelIndex = QModelIndex());
    Q_INVOKABLE void removeFromTree(const QModelIndex &index);
    Q_INVOKABLE void rename(const QModelIndex &rowModelIndex, const QString &newName);
    Q_INVOKABLE void askForFocus(const QModelIndex &rowModelIndex);
    Q_INVOKABLE void askForExpand(const QModelIndex &rowModelIndex);
    Q_INVOKABLE void initModel(bool convert = false);
    Q_INVOKABLE QModelIndex getNoteModelIndex(const QString &notePath);
    Q_INVOKABLE void moveRow(const QModelIndex &rowModelIndex, const QModelIndex &newParentIndex, const QString &newName = QLatin1String());
    Q_INVOKABLE void saveMetaData();

    // NoteMapper
    void setNoteMapEnabled(const bool noteMapEnabled);
    bool noteMapEnabled();
    bool isInit();
    void addInitialGlobalPath(const QString &path);

Q_SIGNALS:
    void errorOccurred(const QString &errorMessage);
    void
    moveError(const QModelIndex &rowModelIndex, const QModelIndex &newParentIndex, const QString &useCase, const QString &shownName, const QString &parentPath);
    // NoteMapper
    void newGlobalPathFound(const QString &path);
    void globalPathUpdated(const QString &oldPath, const QString &newPath);
    void globalPathRemoved(const QString &path);
    void initialGlobalPathsSent(const QStringList &initialGlobalPaths);

    void oldStorageStructure();

private:
    // Storage Handler
    bool makeStorage(const QString &storagePath);
    QString makeFolder(const QString &parentPath, const QString &folderName);
    QString makeNote(const QString &parentPath, const QString &noteName);

private:
    // NoteMapper
    bool m_noteMapEnabled = KleverConfig::noteMapEnabled();
    bool m_isInit = false;
    QStringList m_initialGlobalPaths;

    QString m_path;
    std::unique_ptr<TreeItem> m_rootItem;
    QFileInfo m_fileInfo;
};
