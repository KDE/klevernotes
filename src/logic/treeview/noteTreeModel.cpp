// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#include "noteTreeModel.h"
#include "../kleverUtility.h"
#include <KIO/CopyJob>
#include <QDir>
#include <QIcon>
#include <klocalizedstring.h>

TreeItem::TreeItem(const QString &path, const int depth_level, NoteTreeModel *model, TreeItem *parentItem)
    : m_parentItem(parentItem)
    , m_model(model)
    , m_depth_level(depth_level)
{
    const QFileInfo fileInfo(path);
    Q_ASSERT(fileInfo.exists());

    static const QString baseCategStr = QStringLiteral(".BaseCategory");
    static const QString baseGroupStr = QStringLiteral(".BaseGroup");

    const QString fileName = fileInfo.fileName();
    m_realName = fileName;
    m_displayName = fileName == baseCategStr ? KleverConfig::categoryDisplayName() : fileName;

    if (depth_level < 3) {
        const QDir::SortFlags sortFlags = fileName == baseGroupStr ? QDir::Name | QDir::Reversed : QDir::Name;
        const QFileInfoList fileList = QDir(path).entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries | QDir::Filter::AccessMask, sortFlags);

        for (const QFileInfo &file : fileList) {
            const QString name = file.fileName();

            const bool isNotBaseFolder = (name != baseCategStr && name != baseGroupStr);

            if (!file.isDir() || (name.startsWith(QStringLiteral(".")) && isNotBaseFolder)) {
                continue;
            }

            auto subTree = std::make_unique<TreeItem>(file.absoluteFilePath(), m_depth_level + 1, m_model, this);

            if (name == baseGroupStr) {
                // Notes inside ".BaseGroup" folder should be shown as being held by the category directly, not by a group
                // Move all the subTree child to the parent of the subTree
                for (int i = subTree->childCount() - 1; i >= 0; i--) {
                    auto categoryNote = subTree->takeUniqueChildAt(i);

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
    if (item->m_depth_level == 3 && m_model->noteMapEnabled()) {
        // very important to make a copy here !
        const QString path = item->data(NoteTreeModel::PathRole).toString().remove(KleverConfig::storagePath());
        if (m_model->isInit()) {
            Q_EMIT m_model->newGlobalPathFound(path);
        } else {
            m_model->addInitialGlobalPath(path);
        }
    }
    if (item->getParentItem() != this) {
        item->setParentItem(this);
    }
    m_childItems.push_back(std::move(item));
}

TreeItem *TreeItem::child(int row) const
{
    if (row < 0 || row >= static_cast<int>(m_childItems.size())) {
        return nullptr;
    }
    return m_childItems.at(row).get();
}

std::unique_ptr<TreeItem> TreeItem::takeUniqueChildAt(int row)
{
    if (row < 0 || row >= static_cast<int>(m_childItems.size())) {
        return nullptr;
    }

    auto item = std::move(m_childItems.at(row));

    m_childItems.erase(m_childItems.begin() + row);

    return item;
}

int TreeItem::childCount() const
{
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
    case NoteTreeModel::PathRole: {
        static const QChar slash = QLatin1Char('/');
        QString path;
        if (m_depth_level == 1) {
            path = KleverConfig::storagePath() + slash + m_realName;
        } else {
            path = m_parentItem->data(NoteTreeModel::PathRole).toString() + slash;
            if (m_depth_level == 3 && m_parentItem->getDepth() == 1) {
                path += QStringLiteral(".BaseGroup") + slash;
            }
            path.append(m_realName);
        }
        return path;
    }

    case Qt::DisplayRole:
    case NoteTreeModel::DisplayNameRole:
        return m_displayName;

    case Qt::DecorationRole:
        return QIcon::fromTheme(QStringLiteral("document-edit-sign"));

    case NoteTreeModel::IconNameRole:
        switch (m_depth_level) {
        case 1:
            return QStringLiteral("documentation-symbolic");
        case 2:
            return QStringLiteral("document-open-symbolic");
        case 3:
            return QStringLiteral("document-edit-sign-symbolic");
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
        if (m_depth_level != 3)
            return QStringLiteral(".Not a note"); // No note can start with a '.'
        return data(NoteTreeModel::DisplayNameRole);

    case NoteTreeModel::BranchNameRole:
        if (m_depth_level == 1) {
            return QLatin1String();
        } else { // The switch statement is not happy without this else...
            const QString parentName = m_parentItem->data(NoteTreeModel::DisplayNameRole).toString();
            if (m_parentItem->data(NoteTreeModel::UseCaseRole).toString() != QStringLiteral("Group")) {
                return parentName;
            }
            const QString grandParentName = m_parentItem->getParentItem()->data(NoteTreeModel::DisplayNameRole).toString();
            const QString finalValue = grandParentName + QStringLiteral("→") + parentName;
            return finalValue;
        }

    case NoteTreeModel::FullNameRole: {
        const QString returnValue = data(NoteTreeModel::BranchNameRole).toString() + QStringLiteral(": ") + data(NoteTreeModel::NoteNameRole).toString();
        return returnValue;
    }

    case NoteTreeModel::WantFocusRole:
        return m_wantFocus;

    case NoteTreeModel::WantExpandRole:
        return m_wantExpand;

    default:
        Q_UNREACHABLE();
    }
};

TreeItem *TreeItem::getParentItem() const
{
    return m_parentItem;
}

void TreeItem::setParentItem(TreeItem *parentItem)
{
    m_parentItem = parentItem;
}

int TreeItem::getDepth() const
{
    return m_depth_level;
}

void TreeItem::remove()
{
    Q_ASSERT(m_parentItem);

    if (m_model->noteMapEnabled())
        Q_EMIT m_model->globalPathRemoved(data(NoteTreeModel::PathRole).toString());

    const auto it = std::find_if(m_parentItem->m_childItems.cbegin(), m_parentItem->m_childItems.cend(), [this](const std::unique_ptr<TreeItem> &treeItem) {
        return treeItem.get() == const_cast<TreeItem *>(this);
    });
    m_parentItem->m_childItems.erase(it);
}

void TreeItem::setDisplayName(const QString &name)
{
    m_displayName = name;
}

void TreeItem::setRealName(const QString &name)
{
    m_realName = name;
}

QString TreeItem::getRealName() const
{
    return m_realName;
}

void TreeItem::askForFocus(const QModelIndex &itemIndex)
{
    // We just want to send a signal to QML
    m_wantFocus = true;
    Q_EMIT m_model->dataChanged(itemIndex, itemIndex);
    m_wantFocus = false;
    Q_EMIT m_model->dataChanged(itemIndex, itemIndex);
}

void TreeItem::askForExpand(const QModelIndex &itemIndex)
{
    // We just want to send a signal to QML
    m_wantExpand = true;
    Q_EMIT m_model->dataChanged(itemIndex, itemIndex);
    m_wantExpand = false;
    Q_EMIT m_model->dataChanged(itemIndex, itemIndex);
}

NoteTreeModel::NoteTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

void NoteTreeModel::initModel()
{
    if (KleverConfig::storagePath().isEmpty() || !KleverConfig::storagePath().toLower().endsWith(QStringLiteral("klevernotes"))) {
        return;
    }

    if (!KleverUtility::exists(KleverConfig::storagePath())) {
        const bool storageCreated = makeStorage(KleverConfig::storagePath());
        if (!storageCreated) {
            m_rootItem = nullptr;
            return;
        }

        // This normally won't happen, but who knows
        const QString basePath = KleverConfig::storagePath().append(QStringLiteral("/.BaseCategory/.BaseGroup"));
        bool groupCreated = KleverUtility::exists(basePath);
        if (!groupCreated) {
            const QString categoryPath = KleverConfig::storagePath().append(QStringLiteral("/.BaseCategory"));
            groupCreated = makeGroup(categoryPath, QStringLiteral(".BaseGroup"));
            if (!groupCreated) {
                m_rootItem = nullptr;
                return;
            }
        }
        const bool initDemoNote = makeNote(basePath, QStringLiteral("Demo"));
        if (!initDemoNote) {
            Q_EMIT errorOccurred(i18n("An error occurred while trying to create the demo note."));
        } else {
            const QString notePath = basePath + QStringLiteral("/Demo/");

            const QString mdPath = notePath + QStringLiteral("note.md");
            QFile::remove(mdPath);
            QFile::copy(QStringLiteral(":/demo_note.md"), mdPath);

            QFile demoNote(mdPath);
            demoNote.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::WriteGroup
                                    | QFile::ReadOther | QFile::WriteOther);

            const QString imagePath = notePath + QStringLiteral("Images/");
            QDir().mkpath(imagePath);
            QFile::copy(QStringLiteral(":/Images/logo.png"), imagePath + QStringLiteral("logo.png"));
        }
    }

    beginResetModel();
    m_rootItem = std::make_unique<TreeItem>(KleverConfig::storagePath(), 0, this);
    endResetModel();

    if (m_noteMapEnabled) {
        m_isInit = true;
        Q_EMIT initialGlobalPathsSent(m_initialGlobalPaths);
    }
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

QModelIndex NoteTreeModel::addRow(const QString &rowName, const QString &parentPath, const int rowLevel, const QModelIndex &parentModelIndex)
{
    const auto parentRow = !parentModelIndex.isValid() ? m_rootItem.get() : static_cast<TreeItem *>(parentModelIndex.internalPointer());

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
        Q_EMIT errorOccurred(i18n("An error occurred while trying to create this item."));
        rowCreated = false;
        break;
    }
    if (!rowCreated)
        return QModelIndex();

    const QString rowPath = parentPath + QLatin1Char('/') + rowName;
    auto newRow = std::make_unique<TreeItem>(rowPath, rowLevel, this, parentRow);

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
bool NoteTreeModel::makeNote(const QString &groupPath, const QString &noteName)
{
    const QString notePath = groupPath + QLatin1Char('/') + noteName;

    QFile note(notePath + QStringLiteral("/note.md"));
    QFile todo(notePath + QStringLiteral("/todo.json"));

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
    if (!noteFolderCreated || !creationSucces)
        Q_EMIT errorOccurred(i18n("An error occurred while trying to create the note."));

    return (noteFolderCreated && creationSucces);
}

bool NoteTreeModel::makeGroup(const QString &categoryPath, const QString &groupName)
{
    const QString groupPath = categoryPath + QLatin1Char('/') + groupName;

    const bool groupCreated = KleverUtility::create(groupPath);
    if (!groupCreated)
        Q_EMIT errorOccurred(i18n("An error occurred while trying to create the group."));
    return groupCreated;
}

bool NoteTreeModel::makeCategory(const QString &storagePath, const QString &categoryName)
{
    const QString categoryPath = storagePath + QLatin1Char('/') + categoryName;

    const bool groupCreated = makeGroup(categoryPath, QStringLiteral(".BaseGroup"));
    if (!groupCreated)
        Q_EMIT errorOccurred(i18n("An error occurred while trying to create the category."));
    return groupCreated;
}

bool NoteTreeModel::makeStorage(const QString &storagePath)
{
    const bool categoryCreated = makeCategory(storagePath, QStringLiteral("/.BaseCategory"));
    if (!categoryCreated)
        Q_EMIT errorOccurred(i18n("An error occurred while trying to create the storage."));
    return categoryCreated;
}
