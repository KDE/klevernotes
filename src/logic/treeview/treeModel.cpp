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

NoteTreeModel::NoteTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

void NoteTreeModel::initModel(bool convert)
{
    if (KleverConfig::storagePath().isEmpty()) {
        return;
    }

    if (!QDir(KleverConfig::storagePath()).exists()) {
        const bool storageCreated = makeStorage(KleverConfig::storagePath());
        if (!storageCreated) {
            m_rootItem = nullptr;
            return;
        }
    }

    const QString storagePath = KleverConfig::storagePath();
    const QString metadataPath = storagePath + QStringLiteral("/.klevernotesFolder.metadata.json");
    if (KleverConfig::storagePath().toLower().endsWith(QStringLiteral("klevernotes")) && !QFile(metadataPath).exists()) {
        if (!convert) {
            Q_EMIT oldStorageStructure();
            return;
        }
        treeModelConverter::convertFileStructure(KleverConfig::storagePath());
    }

    beginResetModel();
    m_rootItem = std::make_unique<TreeItem>(KleverConfig::storagePath(), 0, this);
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
        {DisplayNameRole, "displayName"},
        {PathRole, "path"},
        {IconNameRole, "iconName"},
        {UseCaseRole, "useCase"},
        {NoteNameRole, "noteName"},
        {BranchNameRole, "branchName"},
        {FullNameRole, "fullName"},
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

QModelIndex NoteTreeModel::addRow(const QString &rowName, const QString &parentPath, const bool isNote, const QModelIndex &parentModelIndex)
{
    const auto parentRow = !parentModelIndex.isValid() ? m_rootItem.get() : static_cast<TreeItem *>(parentModelIndex.internalPointer());

    bool rowCreated;
    if (isNote) {
        rowCreated = fileSystemHelper::createFile(parentPath + QStringLiteral("/") + rowName + QStringLiteral(".md"));
        rowCreated = fileSystemHelper::createFile(parentPath + QStringLiteral("/") + rowName + QStringLiteral(".todo.json"));
    } else {
        rowCreated = fileSystemHelper::createFolder(parentPath + QStringLiteral("/") + rowName);
    }

    if (!rowCreated) {
        return QModelIndex();
    }

    const QString rowPath = parentPath + QLatin1Char('/') + rowName;
    auto newRow = std::make_unique<TreeItem>(rowPath, isNote, this, parentRow);

    const int childCount = parentRow->childCount();
    beginInsertRows(parentModelIndex, childCount, childCount);
    parentRow->appendChild(std::move(newRow));
    endInsertRows();

    QModelIndex currentModelIndex = createIndex(childCount, 0, parentRow->child(childCount));
    return currentModelIndex;
}

void NoteTreeModel::removeFromTree(const QModelIndex &index)
{
    auto row = static_cast<TreeItem *>(index.internalPointer());
    const QString rowPath = row->data(PathRole).toString();

    // Prevent KDescendantsProxyModel from crashing
    if (row->childCount() > 0) {
        row->askForExpand(index);
    }

    auto *job = KIO::trash(QUrl::fromLocalFile(rowPath));
    job->start();

    connect(job, &KJob::result, this, [job, row, index, this] {
        if (!job->error()) {
            beginRemoveRows(parent(index), index.row(), index.row());
            row->remove();
            endRemoveRows();
            return;
        }
        Q_EMIT errorOccurred(i18n("An error occurred while trying to remove this item."));
        qWarning() << job->errorString();
    });
}

void NoteTreeModel::moveRow(const QModelIndex &rowModelIndex, const QModelIndex &newParentIndex, const QString &newName)
{
    static const QChar slash = QLatin1Char('/');
    auto row = static_cast<TreeItem *>(rowModelIndex.internalPointer());

    const auto newParent = static_cast<TreeItem *>(newParentIndex.internalPointer());

    if (row->getParentItem() == newParent) {
        return;
    }

    const QString rowPath = row->data(PathRole).toString();
    const QString parentPath = newParent->data(PathRole).toString();
    QString dest = parentPath + slash;

    if (newParent->getDepth() == 1 && row->getDepth() == 3) {
        dest += QStringLiteral(".BaseGroup") + slash;
    }
    const QString finalName = newName.isEmpty() ? row->getRealName() : newName;
    dest += finalName;

    if (QDir(dest).exists()) {
        Q_EMIT moveError(rowModelIndex, newParentIndex, row->data(NoteTreeModel::UseCaseRole).toString(), row->getRealName(), parentPath);
        return;
    }

    auto *job = KIO::move(QUrl::fromLocalFile(rowPath), QUrl::fromLocalFile(dest));
    job->start();

    connect(job, &KJob::result, this, [job, rowModelIndex, finalName, newParent, newParentIndex, this] {
        if (!job->error()) {
            newParent->askForExpand(newParentIndex);

            const int oldRowNumber = rowModelIndex.row();
            const int lastRow = newParent->childCount();
            const QModelIndex oldParentIndex = parent(rowModelIndex);

            beginMoveRows(oldParentIndex, oldRowNumber, oldRowNumber, newParentIndex, lastRow);

            const auto oldParent = static_cast<TreeItem *>(oldParentIndex.internalPointer());
            // // actually remove the row, that's why we don't use the already avalaible 'row' TreeItem
            auto row = oldParent->takeUniqueChildAt(oldRowNumber);
            row->setDisplayName(finalName);
            row->setRealName(finalName);
            newParent->appendChild(std::move(row));

            endMoveRows();
            return;
        }
        Q_EMIT errorOccurred(i18n("An error occurred while trying to move this item."));
        qWarning() << job->errorString();
    });
}

void NoteTreeModel::rename(const QModelIndex &rowModelIndex, const QString &newName)
{
    const auto row = static_cast<TreeItem *>(rowModelIndex.internalPointer());

    const QString rowPath = row->data(PathRole).toString();

    const bool isBaseCategory = rowPath.endsWith(QStringLiteral(".BaseCategory"));
    if (isBaseCategory) {
        KleverConfig::setCategoryDisplayName(newName);
    } else {
        QDir dir(rowPath);
        dir.cdUp();

        const QString newPath = dir.absolutePath() + QLatin1Char('/') + newName;

        const bool renamed = QDir().rename(rowPath, newPath);

        if (!renamed) {
            Q_EMIT errorOccurred(i18n("An error occurred while trying to rename this item."));
            return;
        }
    }

    row->setDisplayName(newName);
    if (!isBaseCategory) {
        row->setRealName(newName);
    }
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
    QStringList currentPathParts = notePath.split(QStringLiteral("/"));
    currentPathParts.pop_front(); // remove the first empty string
    QString currentPathPart = currentPathParts.takeAt(0);

    auto currentParentItem = m_rootItem.get();
    QModelIndex currentModelIndex;

    bool hasBreak = false;
    for (int i = 0; i < currentParentItem->childCount();) {
        const auto currentItem = currentParentItem->child(i);
        const QString currentItemPath = currentItem->data(PathRole).toString();
        if (currentItemPath.endsWith(currentPathPart)) {
            currentModelIndex = createIndex(i, 0, currentItem);
            if (currentPathParts.isEmpty()) {
                hasBreak = true;
                break;
            } else {
                currentPathPart = currentPathParts.takeAt(0);

                if (currentPathPart == QStringLiteral(".BaseGroup"))
                    currentPathPart = currentPathParts.takeAt(0);
            }
            currentParentItem = currentItem;
            i = 0;
            continue;
        }
        i++;
    }

    return hasBreak ? currentModelIndex : QModelIndex(); // Easier to handle in qml
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
bool NoteTreeModel::makeNote(const QString &folderPath, const QString &noteName)
{
    const QString notePath = folderPath + QLatin1Char('/') + noteName;

    bool creationSucces = fileSystemHelper::createFile(notePath + QStringLiteral(".md"));
    if (creationSucces) {
        creationSucces = fileSystemHelper::createFile(notePath + QStringLiteral(".todo.json"));
    }
    if (!creationSucces) {
        Q_EMIT errorOccurred(i18n("An error occurred while trying to create the note."));
    }

    return (creationSucces);
}

bool NoteTreeModel::makeFolder(const QString &folderPath)
{
    const bool folderCreated = fileSystemHelper::createFolder(folderPath);
    if (!folderCreated) {
        Q_EMIT errorOccurred(i18n("An error occurred while trying to create this folder."));
    }
    return folderCreated;
}

bool NoteTreeModel::makeStorage(const QString &storagePath)
{
    const QString storageErrorMessage = i18n("An error occurred while trying to create the storage.");
    const QString demoErrorMessage = i18n("An error occurred while trying to create the demo note.");

    const QString mainFolderPath = storagePath + QStringLiteral("/") + i18nc("Main folder name, where all the notes will be stored by default", "Notes");
    if (!makeFolder(mainFolderPath)) {
        Q_EMIT errorOccurred(storageErrorMessage);
        return false;
    }

    const QString demoName = i18nc("The name for a demo note", "Demo");
    const bool initDemoNote = makeNote(mainFolderPath, demoName);
    if (!initDemoNote) {
        Q_EMIT errorOccurred(demoErrorMessage);
    } else {
        const QString demoPath = mainFolderPath + QStringLiteral("/") + demoName + QStringLiteral(".md");
        if (QFile::copy(QStringLiteral(":/demo_note.md"), demoPath)) {
            QFile(demoPath).setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::WriteGroup
                                           | QFile::ReadOther | QFile::WriteOther);
        } else {
            // No need to return false, the user simply won't have the Demo, no big deal
            Q_EMIT errorOccurred(demoErrorMessage);
        }

        if (!makeFolder(storagePath + QStringLiteral("/Images/"))) {
            Q_EMIT errorOccurred(demoErrorMessage);
        } else if (!QFile::copy(QStringLiteral(":/Images/logo.png"), mainFolderPath + QStringLiteral("/Images/logo.png"))) {
            Q_EMIT errorOccurred(demoErrorMessage);
        }
    }

    return true;
}
