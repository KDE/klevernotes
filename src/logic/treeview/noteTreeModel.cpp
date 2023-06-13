// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "noteTreeModel.h"
#include "kleverconfig.h"
#include "../kleverUtility.h"
#include <QDir>
#include <QFileInfo>
#include <QIcon>
#include <klocalizedstring.h>
#include <KIO/CopyJob>

TreeItem::TreeItem(const QString &path, const int &depth_level, QAbstractItemModel *model, TreeItem *parentItem)
    : m_parentItem(parentItem)
    , m_model(model)
    , m_path(path)
    , m_depth_level(depth_level)
{
    QFileInfo fileInfo(path);
    Q_ASSERT(fileInfo.exists());

    const QString fileName = fileInfo.fileName();
    m_displayName = fileName == QStringLiteral(".BaseCategory") ? KleverConfig::categoryDisplayName() : fileName;

    if (depth_level < 3) {
        const QFileInfoList fileList = QDir(path).entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries | QDir::Filter::AccessMask);

        for (const QFileInfo &file : fileList) {
            QString name = file.fileName();

            const bool isNotBaseFolder = (name != QStringLiteral(".BaseCategory") && name != QStringLiteral(".BaseGroup"));

            if (!file.isDir() || (name.startsWith(QStringLiteral(".")) && isNotBaseFolder)) {
                continue;
            }

            auto subTree = std::make_unique<TreeItem>(file.absoluteFilePath(), m_depth_level + 1, m_model, this);

            if (name == QStringLiteral(".BaseGroup")) {
                // Notes inside ".BaseGroup" folder should be shown as being held by the category directly, not by a group
                // Move all the subTree child to the parent of the subTree
                for (int i = subTree->childCount() - 1; i >= 0; i--) {
                    auto categoryNote = subTree->uniqueChildAt(i);

                    categoryNote->m_parentItem = this;

                    appendChild(std::move(categoryNote));
                }
                // Delete the now useless subtree
                subTree.reset(nullptr);
            } else {
                appendChild(std::move(subTree));
            }
        }
    }
}

void TreeItem::appendChild(std::unique_ptr<TreeItem> &&item)
{
    m_childItems.push_back(std::move(item));
}

TreeItem *TreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size()) {
        return nullptr;
    }
    return m_childItems.at(row).get();
}

std::unique_ptr<TreeItem> TreeItem::uniqueChildAt(int row)
{
    if (row < 0 || row >= m_childItems.size()) {
        return nullptr;
    }

    return std::move(m_childItems.at(row));
}

int TreeItem::childCount() const
{
    // Not super clean, but required to not let the app crash when the storage can't be created
    if (this == nullptr) return 0;
    return m_childItems.size();
}

int TreeItem::row() const
{
    if (m_parentItem) {
        const auto it = std::find_if(m_parentItem->m_childItems.cbegin(), m_parentItem->m_childItems.cend(), [this](const std::unique_ptr<TreeItem> &treeItem) {
            return treeItem.get() == const_cast<TreeItem *>(this);
        });

        Q_ASSERT(it != m_parentItem->m_childItems.cend());

        return std::distance(m_parentItem->m_childItems.cbegin(), it);
    }

    return 0;
}

QVariant TreeItem::data(int role) const
{
    switch (role) {
    case NoteTreeModel::PathRole:
        return m_path;

    case Qt::DisplayRole:
    case NoteTreeModel::DisplayNameRole:
        return m_displayName;

    case Qt::DecorationRole:
        return QIcon::fromTheme(QStringLiteral("document-edit-sign"));

    case NoteTreeModel::IconNameRole:
        switch (m_depth_level) {
        case 1:
            return QStringLiteral("documentation");
        case 2:
            return QStringLiteral("document-open-folder");
        case 3:
            return QStringLiteral("document-edit-sign");
        default:
            Q_UNREACHABLE();
        }

    case NoteTreeModel::UseCaseRole:
        switch (m_depth_level) {
        case 1:
            return QStringLiteral("Category");
        case 2:
            return QStringLiteral("Group");
        case 3:
            return QStringLiteral("Note");
        default:
            Q_UNREACHABLE();
        }

    case NoteTreeModel::NoteNameRole:
        if (m_depth_level != 3) return QStringLiteral("");
        return data(NoteTreeModel::DisplayNameRole);

    case NoteTreeModel::BranchNameRole:
        if (m_depth_level != 3) return QStringLiteral("");
        else { //The switch statement is not happy without this else...
            QString parentName = m_parentItem->data(NoteTreeModel::DisplayNameRole).toString();
            if (m_parentItem->data(NoteTreeModel::UseCaseRole).toString() != "Group") {
                return parentName;
            }
            QString grandParentName = m_parentItem->parentItem()->data(NoteTreeModel::DisplayNameRole).toString();

            return grandParentName + QStringLiteral("→")+parentName;
        }

    case NoteTreeModel::WantFocusRole:
        return m_wantFocus;

    case NoteTreeModel::WantExpandRole:
        return m_wantExpand;

    default:
        Q_UNREACHABLE();
    }
};

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

void TreeItem::remove()
{
    Q_ASSERT(m_parentItem);

    const auto it = std::find_if(m_parentItem->m_childItems.cbegin(), m_parentItem->m_childItems.cend(), [this](const std::unique_ptr<TreeItem> &treeItem) {
        return treeItem.get() == const_cast<TreeItem *>(this);
    });
    m_parentItem->m_childItems.erase(it);
}

void TreeItem::changePath(const QString &newPart, const QModelIndex &parentModelIndex, int newPartIdx)
{
    QStringList currentPathParts = m_path.split("/");
    if (currentPathParts.last() == ".BaseCategory")
        return;

    if (newPartIdx == -1)
        newPartIdx = currentPathParts.size() - 1;
    currentPathParts[newPartIdx] = newPart;

    QString newPath = currentPathParts.join("/");
    m_path = newPath;

    // By default we assume that we are in the first call
    // So the parentModelIndex is actually the model index of this object
    QModelIndex thisModelIndex = parentModelIndex;

    // We have the parent model index and not the model index of this object
    if (static_cast<TreeItem *>(parentModelIndex.internalPointer()) != this) {
        thisModelIndex = m_model->index(this->row(), 0, thisModelIndex);
    }
    emit m_model->dataChanged(thisModelIndex, thisModelIndex);

    for (const std::unique_ptr<TreeItem> &child : m_childItems) {
        child->changePath(newPart, thisModelIndex, newPartIdx);
    }
}

void TreeItem::changeDisplayName(const QString &name)
{
    m_displayName = name;
}

void TreeItem::askForFocus(const QModelIndex &itemIndex)
{
    // We just want to send a signal to QML
    m_wantFocus = true;
    emit m_model->dataChanged(itemIndex, itemIndex);
    m_wantFocus = false;
    emit m_model->dataChanged(itemIndex, itemIndex);
}

void TreeItem::askForExpand(const QModelIndex &itemIndex)
{
    // We just want to send a signal to QML
    m_wantExpand = true;
    emit m_model->dataChanged(itemIndex, itemIndex);
    m_wantExpand = false;
    emit m_model->dataChanged(itemIndex, itemIndex);
}


NoteTreeModel::NoteTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

void NoteTreeModel::initModel()
{
    if (!KleverUtility::exists(KleverConfig::storagePath())) {
        const bool storageCreated = makeStorage(KleverConfig::storagePath());
        if (!storageCreated) {
            m_rootItem = nullptr;
            return;
        }
    }
    // This normally won't happen, but who knows
    QString basePath = KleverConfig::storagePath().append(QStringLiteral("/.BaseCategory/.BaseGroup"));
    if (!KleverUtility::exists(basePath)) {
        const QString categoryPath = KleverConfig::storagePath().append(QStringLiteral("/.BaseCategory"));
        const bool groupCreated = makeGroup(categoryPath, ".BaseGroup");
        if (!groupCreated) {
            m_rootItem = nullptr;
            return;
        }
    }

    beginResetModel();
    m_rootItem = std::make_unique<TreeItem>(KleverConfig::storagePath(), 0, this);
    endResetModel();
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
        {WantFocusRole, "wantFocus"},
        {WantExpandRole, "wantExpand"}
    };
}

QModelIndex NoteTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return {};
    }

    const auto childItem = static_cast<TreeItem *>(index.internalPointer());
    const auto parentItem = childItem->parentItem();

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

void NoteTreeModel::addRow(const QString &rowName, const QString &parentPath, const int rowLevel, const QModelIndex &parentModelIndex)
{
    const auto parent = !parentModelIndex.isValid() ? m_rootItem.get() : static_cast<TreeItem *>(parentModelIndex.internalPointer());

    bool rowCreated;
    switch (rowLevel) {
        case 1:
            rowCreated = makeCategory(parentPath, rowName);
            break;
        case 2:
            rowCreated = makeGroup(parentPath, rowName);
            break;
        case 3:
            rowCreated = makeNote(parentPath, rowName);
            break;
        default:
            emit errorOccurred(i18n("An error occurred while trying to create this item."));
            rowCreated = false;
            break;
    }
    if (!rowCreated) return;

    const QString rowPath = parentPath + (QLatin1Char('/') + rowName);
    auto newRow = std::make_unique<TreeItem>(rowPath, rowLevel, this, parent);

    beginInsertRows(parentModelIndex, parent->childCount(), parent->childCount());
    parent->appendChild(std::move(newRow));
    endInsertRows();
}

void NoteTreeModel::removeFromTree(const QModelIndex &index)
{
    auto row = static_cast<TreeItem *>(index.internalPointer());
    const QString rowPath = row->data(PathRole).toString();

    auto *job = KIO::trash(QUrl::fromLocalFile(rowPath));
    job->start();

    connect(job, &KJob::result, this, [job, row, index, this] {
        if (!job->error()) {
            beginRemoveRows(parent(index), index.row(), index.row());
            row->remove();
            endRemoveRows();
            return;
        }
        emit errorOccurred(i18n("An error occurred while trying to remove this item."));
        qWarning() << job->errorString();
    });
}

void NoteTreeModel::rename(const QModelIndex &rowModelIndex, const QString &newName)
{
    const auto row = static_cast<TreeItem *>(rowModelIndex.internalPointer());

    const QString rowPath = row->data(PathRole).toString();

    if (rowPath.endsWith(QStringLiteral(".BaseCategory"))) {
        KleverConfig::setCategoryDisplayName(newName);
    } else {
        QDir dir(rowPath);
        const bool renamed = dir.rename(dir.dirName(), newName);

        if (!renamed) {
            emit errorOccurred(i18n("An error occurred while trying to rename this item."));
            return;
        }
    }

    row->changeDisplayName(newName);
    row->changePath(newName, rowModelIndex);
}

void NoteTreeModel::askForFocus(const QModelIndex& rowModelIndex)
{
    const auto row = static_cast<TreeItem *>(rowModelIndex.internalPointer());
    row->askForFocus(rowModelIndex);
}

void NoteTreeModel::askForExpand(const QModelIndex& rowModelIndex)
{
    const auto row = static_cast<TreeItem *>(rowModelIndex.internalPointer());
    row->askForExpand(rowModelIndex);
}


// Storage Handler
bool NoteTreeModel::makeNote(const QString &groupPath, const QString &noteName)
{
    const QString notePath = QString(groupPath).append(QLatin1Char('/') + noteName);

    QFile note(notePath + "/note.md");
    QFile todo(notePath + "/todo.json");

    const bool noteFolderCreated = KleverUtility::create(notePath);
    bool creationSucces;
    if (noteFolderCreated) {
        creationSucces = note.open(QIODevice::ReadWrite);
        note.close();
        if (creationSucces) {
            creationSucces = todo.open(QIODevice::ReadWrite);
            todo.close();
        }
    }
    if (!noteFolderCreated || !creationSucces) emit errorOccurred(i18n("An error occurred while trying to create the note."));

    return (noteFolderCreated && creationSucces);
}

bool NoteTreeModel::makeGroup(const QString &categoryPath, const QString &groupName)
{
    const QString groupPath = QString(categoryPath).append(QLatin1Char('/') + groupName);

    const bool groupCreated = KleverUtility::create(groupPath);
    if (!groupCreated) emit errorOccurred(i18n("An error occurred while trying to create the group."));
    return groupCreated;
}

bool NoteTreeModel::makeCategory(const QString &storagePath, const QString &categoryName)
{
    const QString categoryPath = QString(storagePath).append(QLatin1Char('/') + categoryName);

    const bool groupCreated = makeGroup(categoryPath, ".BaseGroup");
    if (!groupCreated) emit errorOccurred(i18n("An error occurred while trying to create the category."));
    return groupCreated;
}

bool NoteTreeModel::makeStorage(const QString &storagePath)
{
    const bool categoryCreated = makeCategory(storagePath, "/.BaseCategory");
    if (!categoryCreated) emit errorOccurred(i18n("An error occurred while trying to create the storage."));
    return categoryCreated;
}



