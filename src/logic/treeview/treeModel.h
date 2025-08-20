// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

// KleverNotes includes
#include "kleverconfig.h"
#include "treeItem.h"

// Qt includes
#include <QAbstractItemModel>
#include <QFileInfo>
#include <QQmlEngine>

class NoteTreeModel : public QAbstractItemModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool noteMapEnabled WRITE setNoteMapEnabled) // QML will handle the signal and change it for us

public:
    explicit NoteTreeModel(QObject *parent = nullptr);

    enum ExtraRoles {
        PathRole = Qt::UserRole + 1, // To get a string with the fullPath of the Note/Folder
        DirRole, // To get a string with the fullPath the folder or the folder containing the note
        ParentPathRole, // To get a string with the fullPath to the parent folder of the item
        NameRole, // To get a string with the name of the Item (strip down of ".md" for the note)
        IconNameRole, // To get a string with the icon name associated with the Item
        ColorRole, // To get a string with the color associated with the Item
        IsNote, // To know if the item is a Note (else it's a folder)
        WantFocusRole, // For send a signal to the qml ItemDelegate using dataChanged, asking for focus
        WantExpandRole, // For send a signal to the qml ItemDelegate using dataChanged, asking to expands
    };
    Q_ENUM(ExtraRoles)

    // Override default
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QHash<int, QByteArray> roleNames() const override;

    /*
     * Create a file/folder (based on `isNote`) inside the folder corresponding to `parentModelIndex` and adds it to the model.
     * Returns the newly created QModelIndex corresponding to the Item.
     *
     * @param rowName: The name of the futur row
     * @param isNote: Whether this row should be a note (a file) or not (a folder)
     * @param parentModelIndex: The QModelIndex pointing to the parent. If this param is invalid (by default) it is considered to be the root item
     */
    Q_INVOKABLE QModelIndex addRow(const QString &rowName, const bool isNote, const QModelIndex &parentModelIndex = QModelIndex());

    /*
     * Make the Item ask to be expended by the KDescendantsProxyModel inside the Treeview
     * Usually works in tandem with `askForFocus`
     *
     * @param rowModelIndex: The QModelIndex pointing to the row to expand
     */
    Q_INVOKABLE void askForExpand(const QModelIndex &rowModelIndex);

    /*
     * Make the Item ask to be focused by the KDescendantsProxyModel inside the Treeview
     * Usually works in tandem with `askForExpand`
     *
     * @param rowModelIndex: The QModelIndex pointing to the row to focus
     */
    Q_INVOKABLE void askForFocus(const QModelIndex &rowModelIndex);

    /*
     * Get the model index for a given notePath.
     * A default QModelIndex (invalid) is returned if the notePath wasn't found.
     */
    Q_INVOKABLE QModelIndex getNoteModelIndex(const QString &notePath);

    /*
     * Initialize the model
     *
     * @param convert: Whether the old file structure should be converted if it is detected
     *
     * @signal oldStorageStructure: If the old file structure is detected and `convert` is set to false
     */
    Q_INVOKABLE void initModel(bool convert = false);

    /*
     * Move the Item (and children) to a new parent.
     * This also renames it to `newName` if not empty.
     *
     * @param rowModelIndex: The QModelIndex pointing to the row to move
     * @param newParentIndex: The QModelIndex pointing to the new parent for the row
     * @param newName: The possible new name for the row
     */
    Q_INVOKABLE void moveRow(const QModelIndex &rowModelIndex, const QModelIndex &newParentIndex = {}, const QString &newName = QLatin1String());

    /*
     * Remove the Item from the model
     *
     * @param index: The QModelIndex pointing to the row to remove
     * @param permanent: Whether the file/folder corresponding to this item should be delete (true) or placed inside the trash (false)
     */
    Q_INVOKABLE void removeFromTree(const QModelIndex &index, const bool permanent);

    /*
     * Try to rename the Item to `newName`
     *
     * @param rowModelIndex: The QModelIndex pointing to the row to remove
     * @param newName: The expected new name for the item
     *
     * @signal errorOccurred: If the item could not be renamed for whatever reason
     */
    Q_INVOKABLE void rename(const QModelIndex &rowModelIndex, const QString &newName);

    /*
     * Save the metadata of each Item
     */
    Q_INVOKABLE void saveMetaData();

    /*
     * Set the properties (color and icon) for the given Item
     * There's no checks for the validity of these properties
     *
     * @param rowModelIndex: The QModelIndex pointing to the row
     * @param color: The color of this row
     * @param icon: The icon of this row
     */
    Q_INVOKABLE void setProperties(const QModelIndex &rowModelIndex, const QString color, const QString icon);

    // NoteMapper
    /*
     * Enable/disable the note map based on noteMapEnabled
     */
    void setNoteMapEnabled(const bool noteMapEnabled);

    /*
     * Check if the note map is enabled
     */
    bool noteMapEnabled();

    /*
     * Check if the model is initialized
     */
    bool isInit();

    /*
     * Add `path` to the initial global path inside the note map
     */
    void addInitialGlobalPath(const QString &path);

Q_SIGNALS:
    /*
     * Signals an error with a given error message to display
     */
    void errorOccurred(const QString &errorMessage);

    /*
     * Signals that this item need focus
     *
     * @param rowModelIndex: The QModelIndex pointing to the item needing focus
     */
    void forceFocus(const QModelIndex &rowModelIndex);

    /*
     * Signals on error while trying to move an item.
     *
     * @param rowModelIndex: The QModelIndex pointing to the item we were trying to move
     * @param newParentIndex: The QModelIndex pointing to the new parent item we were trying to move to
     * @param rowName: The row name
     * @param newParentPath: The path of the new parent item
     */
    void moveError(const QModelIndex &rowModelIndex, const QModelIndex &newParentIndex, bool isNote, const QString &rowName, const QString &newParentPath);

    /*
     * Signals that the old file structure has been detected
     */
    void oldStorageStructure();

    // NoteMapper
    /*
     * Signals that a new global path has been found
     */
    void newGlobalPathFound(const QString &path);

    /*
     * Signals that a global path has been updated
     */
    void globalPathUpdated(const QString &oldPath, const QString &newPath);

    /*
     * Signals that a global path has been removed
     */
    void globalPathRemoved(const QString &path);

    /*
     * Signals that this item need focus
     *
     * @param rowModelIndex: The QModelIndex pointing to the item needing focus
     */
    void initialGlobalPathsSent(const QStringList &initialGlobalPaths);

private:
    enum MoveError {
        NoError, // For code readability
        NameExist,
        FailedToMove
    };

    /*
     * Handle the result of the removing of an Item. Signals error if needed or update the model.
     *
     * @param index: The QModelIndex pointing to the Item that has been removed
     * @param succes: Whether the operation was successful
     *
     * @signal forceFocus: Ask for a specific item to be focused if the removing was successful
     * @signal errorOccurred: If the Item could not be removed
     */
    void handleRemoveItem(const QModelIndex &index, const bool succes);

    /*
     * Handle the result of the moving of an Item. Signals error or update the model.
     *
     * @param rowModelIndex: The QModelIndex pointing to the Item that has been moved
     * @param newParentIndex: The QModelIndex pointing to the new parent of the Item
     * @param newPath: The potential new path for the row
     * @param newName: The potential new name for the row
     * @param error: A potential error code based on the `MoveError` enum
     *
     * @signal moveError: If an Item with the same name already exists in the new location
     * @signal errorOccurred: If the Item could not be moved
     * @signal forceFocus: Ask for the Item to be focused
     */
    void handleMoveItem(const QModelIndex &rowModelIndex, const QModelIndex &newParentIndex, const QString &newPath, const QString &newName, MoveError error);

    // Storage Handler
    /*
     * Create the storage at the given storage path. Return the succes of this operation.
     *
     * @param storagePath: The path where the storage should be placed
     *
     * @signal errorOccurred: For what ever error that could happen
     */
    bool makeStorage(const QString &storagePath);

    /*
     * Create a folder inside the folder given by `parentPath`
     *
     * @param parentPath: The path where the folder should be placed
     * @param folderName: The name of the folder
     *
     * @signal errorOccurred: For what ever error that could happen
     */
    QString makeFolder(const QString &parentPath, const QString &folderName);

    /*
     * Create a note inside the folder given by `parentPath`
     *
     * @param parentPath: The path where the folder should be placed
     * @param noteName: The name of the note
     *
     * @signal errorOccurred: For what ever error that could happen
     */
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
