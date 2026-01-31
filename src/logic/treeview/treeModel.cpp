// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

// KleverNotes includes
#include "treeModel.h"

#include "fileSystemHelper.h"
#include "kleverconfig.h"
#include "oldModelConverter.h"

// KDE includes
#include <KIO/CopyJob>
#include <KLocalizedString>

// Qt includes
#include <QDir>

#define slash QLatin1Char('/')
#define mdEnding QStringLiteral(".md")
#define todoEnding QStringLiteral(".todo.json")

NoteTreeModel::NoteTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

void NoteTreeModel::initModel(bool convert)
{
    const QString storagePath = KleverConfig::storagePath();
    if (storagePath.isEmpty()) {
        return;
    }

    if (!QDir(storagePath).exists()) {
        const bool storageCreated = makeStorage(storagePath);
        if (!storageCreated) {
            m_rootItem = nullptr;
            return;
        }
    }

    const QString metadataPath = storagePath + QStringLiteral("/.klevernotesFolder.metadata.json");
    if (storagePath.toLower().endsWith(QStringLiteral("klevernotes")) && !QFile(metadataPath).exists()) {
        if (!convert) {
            Q_EMIT oldStorageStructure();
            return;
        }
        treeModelConverter::convertFileStructure(storagePath);
    }

    beginResetModel();
    m_rootItem = std::make_unique<TreeItem>(storagePath, this);
    endResetModel();

    if (m_noteMapEnabled) {
        m_isInit = true;

        Q_EMIT initialGlobalPathsSent(m_initialGlobalPaths);
    }
}

void NoteTreeModel::saveMetaData()
{
    m_rootItem->saveMetaData();
}

QModelIndex NoteTreeModel::index(int row, int column, const QModelIndex &parent) const
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

QHash<int, QByteArray> NoteTreeModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {PathRole, "path"},
        {DirRole, "dir"},
        {ParentPathRole, "parentPath"},
        {IconNameRole, "iconName"},
        {ColorRole, "color"},
        {IsNote, "isNote"},
        {WantFocusRole, "wantFocus"},
        {WantExpandRole, "wantExpand"},
    };
}

QModelIndex NoteTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto childItem = static_cast<TreeItem *>(index.internalPointer());
    if (childItem == m_rootItem.get()) {
        return {};
    }
    const auto parentItem = childItem->getParentItem();
    if (parentItem == m_rootItem.get()) {
        return {};
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int NoteTreeModel::rowCount(const QModelIndex &parent) const
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

    if (m_rootItem == nullptr) {
        return 0;
    }

    return parentItem->childCount();
}

int NoteTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant NoteTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto item = static_cast<TreeItem *>(index.internalPointer());

    return item->data(role);
}

QModelIndex NoteTreeModel::addRow(const QString &rowName, const bool isNote, const QModelIndex &parentModelIndex)
{
    const auto parentRow = !parentModelIndex.isValid() ? m_rootItem.get() : static_cast<TreeItem *>(parentModelIndex.internalPointer());
    const QString parentPath = parentRow->getPath();

    const QString rowPath = isNote ? makeNote(parentPath, rowName) : makeFolder(parentPath, rowName);

    if (rowPath.isEmpty()) {
        return QModelIndex();
    }

    auto newRow = std::make_unique<TreeItem>(rowPath, this, parentRow);
    const int rowNewIndex = parentRow->getNewChildIndex(rowName, isNote);

    beginInsertRows(parentModelIndex, rowNewIndex, rowNewIndex);
    parentRow->insertChild(std::move(newRow), rowNewIndex);
    endInsertRows();

    QModelIndex currentModelIndex = createIndex(rowNewIndex, 0, parentRow->child(rowNewIndex));
    return currentModelIndex;
}

void NoteTreeModel::handleRemoveItem(const QModelIndex &index, const bool succes)
{
    if (succes) {
        QModelIndex parentModelIndex = parent(index);

        const auto row = static_cast<TreeItem *>(index.internalPointer());
        const int rowIndex = row->row();

        beginRemoveRows(parentModelIndex, rowIndex, rowIndex);
        row->remove();
        endRemoveRows();

        parentModelIndex = parentModelIndex.isValid() ? parentModelIndex : createIndex(m_rootItem->row(), 0, m_rootItem.get());
        const auto parentRow = static_cast<TreeItem *>(parentModelIndex.internalPointer());

        QModelIndex needFocus = parentModelIndex;
        if (parentRow->childCount()) {
            needFocus = rowIndex ? createIndex(rowIndex - 1, 0, parentRow->child(rowIndex - 1)) : createIndex(0, 0, parentRow->child(0));
        }
        Q_EMIT forceFocus(needFocus);
    } else {
        Q_EMIT errorOccurred(i18n("An error occurred while trying to remove this item."));
    }
}

void NoteTreeModel::removeFromTree(const QModelIndex &index, const bool permanent)
{
    auto row = static_cast<TreeItem *>(index.internalPointer());

    // Prevent KDescendantsProxyModel from crashing
    if (row->childCount() > 0) {
        row->askForExpand(index);
    }

    const bool isNote = row->isNote();
    const QString rowPath = row->getPath();
    const QString dirPath = row->getDir();
    const QString name = row->getName();
    const QString todoPath = dirPath + slash + name + todoEnding;

    const bool todoExists = QDir().exists(todoPath);

    if (!permanent) {
        KIO::CopyJob *job = nullptr;

        if (isNote && todoExists) {
            job = KIO::trash({QUrl::fromLocalFile(rowPath), QUrl::fromLocalFile(todoPath)});
        } else if (isNote) {
            job = KIO::trash(QUrl::fromLocalFile(rowPath));
        } else {
            KIO::trash(QUrl::fromLocalFile(rowPath));
        }
        job->start();

        connect(job, &KJob::result, this, [job, index, this] {
            handleRemoveItem(index, !job->error());
        });
    } else {
        bool success = false;
        if (isNote) {
            success = QFile(rowPath).remove();
            if (success && todoExists) {
                success = QFile(todoPath).remove();
            }
        } else {
            success = QDir(rowPath).removeRecursively();
        }

        handleRemoveItem(index, success);
    }
}

void NoteTreeModel::handleMoveItem(const QModelIndex &rowModelIndex,
                                   const QModelIndex &newParentIndex,
                                   const QString &path,
                                   const QString &name,
                                   MoveError error)
{
    const auto newParent = static_cast<TreeItem *>(newParentIndex.internalPointer());

    switch (error) {
    case MoveError::NameExist: {
        const auto row = static_cast<TreeItem *>(rowModelIndex.internalPointer());
        Q_EMIT moveError(rowModelIndex, newParentIndex, row->isNote(), row->getName(), newParent->getPath());
        break;
    }
    case MoveError::FailedToMove: {
        Q_EMIT errorOccurred(i18n("An error occurred while trying to move this item."));
        break;
    }
    default: {
        if (static_cast<TreeItem *>(newParentIndex.internalPointer()) != m_rootItem.get()) {
            newParent->askForExpand(newParentIndex);
        }

        const int oldRowNumber = rowModelIndex.row();
        const QModelIndex oldParentIndex = parent(rowModelIndex);
        const auto oldParent = oldParentIndex.isValid() ? static_cast<TreeItem *>(oldParentIndex.internalPointer()) : m_rootItem.get();

        const auto row = oldParent->child(oldRowNumber);
        const int newRowIndex = newParent->getNewChildIndex(row->getName(), row->isNote());

        beginMoveRows(oldParentIndex, oldRowNumber, oldRowNumber, newParentIndex, newRowIndex);
        auto unique_row = oldParent->takeUniqueChildAt(oldRowNumber);
        unique_row->setName(name);
        unique_row->setPath(path);
        newParent->insertChild(std::move(unique_row), newRowIndex);
        endMoveRows();

        Q_EMIT forceFocus(createIndex(newRowIndex, 0, newParent->child(newRowIndex)));
    }
    }
}

void NoteTreeModel::moveRow(const QModelIndex &rowModelIndex, const QModelIndex &_newParentIndex, const QString &newName)
{
    auto row = static_cast<TreeItem *>(rowModelIndex.internalPointer());

    QModelIndex newParentIndex = _newParentIndex.isValid() ? _newParentIndex : createIndex(m_rootItem->row(), 0, m_rootItem.get());

    const auto newParent = static_cast<TreeItem *>(newParentIndex.internalPointer());

    const QString rowDir = row->getDir();
    const QString rowName = row->getName();

    const QString newParentPath = newParent->getPath();
    const QString finalName = newName.isEmpty() ? rowName : newName;
    const QString newBasePath = newParentPath + slash + finalName;
    QString finalPath = newBasePath;

    MoveError error;
    QDir dir;
    if (row->isNote()) {
        const QString notePath = rowDir + slash + rowName + mdEnding;
        const QString todoPath = rowDir + slash + rowName + todoEnding;
        const QString newNotePath = newBasePath + mdEnding;
        const QString newTodoPath = newBasePath + todoEnding;

        finalPath = newNotePath;

        const bool todoExists = dir.exists(todoPath);
        error = (dir.exists(newNotePath) || (todoExists && dir.exists(newTodoPath))) ? MoveError::NameExist : MoveError::NoError;

        if (!error) {
            error = dir.rename(notePath, newNotePath) ? MoveError::NoError : MoveError::FailedToMove;
            dir.rename(todoPath, newTodoPath); // No need to error if it's just the todos
        }
    } else {
        error = dir.exists(newBasePath) ? MoveError::NameExist : MoveError::NoError;

        if (!error) {
            error = dir.rename(rowDir, newBasePath) ? MoveError::NoError : MoveError::FailedToMove;
        }
    }

    return handleMoveItem(rowModelIndex, newParentIndex, finalPath, finalName, error);
}

void NoteTreeModel::rename(const QModelIndex &rowModelIndex, const QString &newName)
{
    const auto row = static_cast<TreeItem *>(rowModelIndex.internalPointer());

    const auto currentDirPath = row->getDir();
    const QString rowPath = row->getPath();

    QString newPath;

    bool renamed = false;
    if (row->isNote()) {
        const QString newPartialPath = currentDirPath + slash + newName;
        newPath = newPartialPath + mdEnding;

        renamed = QFile(rowPath).rename(newPath);

        if (renamed) {
            const QString currentTodo = currentDirPath + slash + row->getName() + todoEnding;
            const QString newTodo = newPartialPath + todoEnding;
            QFile todoFile = QFile(currentTodo);
            if (todoFile.exists()) {
                renamed = todoFile.rename(newTodo);
            }
        }
    } else {
        QDir dir(rowPath);
        dir.cdUp();
        newPath = dir.path() + slash + newName;

        renamed = QDir(rowPath).rename(rowPath, newPath);
    }

    if (!renamed) {
        Q_EMIT errorOccurred(i18n("An error occurred while trying to rename this item."));
        return;
    }

    row->setPath(newPath);
    row->setName(newName);

    Q_EMIT dataChanged(rowModelIndex, rowModelIndex);
}

void NoteTreeModel::askForFocus(const QModelIndex &rowModelIndex)
{
    const auto row = static_cast<TreeItem *>(rowModelIndex.internalPointer());
    row->askForFocus(rowModelIndex);
}

void NoteTreeModel::askForExpand(const QModelIndex &rowModelIndex)
{
    const auto row = static_cast<TreeItem *>(rowModelIndex.internalPointer());
    row->askForExpand(rowModelIndex);
}

QModelIndex NoteTreeModel::getNoteModelIndex(const QString &notePath)
{
    QStringList currentPathParts = notePath.split(slash);
    currentPathParts.pop_front(); // remove the first empty string
    QString currentPathPart = currentPathParts.takeAt(0);

    auto currentParentItem = m_rootItem.get();

    for (int i = 0; i < currentParentItem->childCount();) {
        const auto currentItem = currentParentItem->child(i);
        const QString currentItemPath = currentItem->getPath();

        if (currentItemPath.endsWith(currentPathPart)) {
            if (currentPathParts.isEmpty()) {
                return createIndex(i, 0, currentItem);
            }

            currentPathPart = currentPathParts.takeAt(0);
            currentParentItem = currentItem;
            i = 0;
            continue;
        }
        i++;
    }

    return QModelIndex(); // Easier to handle in qml
}

void NoteTreeModel::setProperties(const QModelIndex &rowModelIndex, const QString color, const QString icon)
{
    const auto row = static_cast<TreeItem *>(rowModelIndex.internalPointer());
    row->setColor(color);
    row->setIcon(icon);
    Q_EMIT dataChanged(rowModelIndex, rowModelIndex);
}

// NoteMapper
void NoteTreeModel::setNoteMapEnabled(const bool noteMapEnabled)
{
    m_noteMapEnabled = noteMapEnabled;
}

bool NoteTreeModel::noteMapEnabled()
{
    return m_noteMapEnabled;
}

bool NoteTreeModel::isInit()
{
    return m_isInit;
}

void NoteTreeModel::addInitialGlobalPath(const QString &path)
{
    m_initialGlobalPaths.append(path);
}

// Storage Handler
QString NoteTreeModel::makeNote(const QString &parentPath, const QString &noteName)
{
    const QString generalNotePath = parentPath + slash + noteName;
    const QString notePath = generalNotePath + mdEnding;

    if (!fileSystemHelper::createFile(notePath)) {
        Q_EMIT errorOccurred(i18n("An error occurred while trying to create the note."));
        return {};
    }

    return notePath;
}

QString NoteTreeModel::makeFolder(const QString &parentPath, const QString &folderName)
{
    const QString folderPath = parentPath + slash + folderName;

    if (!fileSystemHelper::createFolder(folderPath)) {
        Q_EMIT errorOccurred(i18n("An error occurred while trying to create this folder."));
        return {};
    }
    return folderPath;
}

bool NoteTreeModel::makeStorage(const QString &storagePath)
{
    const QString storageErrorMessage = i18n("An error occurred while trying to create the storage.");
    const QString demoErrorMessage = i18n("An error occurred while trying to create the demo note.");

    const QString mainFolderName = i18nc("Main folder name, where all the notes will be stored by default", "Notes");
    const QString mainFolderPath = makeFolder(storagePath, mainFolderName);
    if (mainFolderPath.isEmpty()) {
        Q_EMIT errorOccurred(storageErrorMessage);
        return false;
    }

    // Prevent warning for newly created storage
    fileSystemHelper::createFile(storagePath + QStringLiteral("/.klevernotesFolder.metadata.json"));

    const QString demoName = i18nc("The name for a demo note", "Demo");
    const QString generalNotePath = mainFolderPath + slash + demoName;
    const QString demoPath = generalNotePath + mdEnding;

    if (QFile::copy(QStringLiteral(":/demo_note.md"), demoPath)) {
        QFile(demoPath).setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::WriteGroup
                                       | QFile::ReadOther | QFile::WriteOther);
    } else {
        // No need to return false, the user simply won't have the Demo, no big deal
        Q_EMIT errorOccurred(demoErrorMessage);
    }

    if (!QFile::copy(QStringLiteral(":/Images/logo.png"), mainFolderPath + QStringLiteral("/logo.png"))) {
        Q_EMIT errorOccurred(demoErrorMessage);
    }

    return true;
}
