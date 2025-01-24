// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

// KleverNotes includes
#include "treeItem.h"

#include "logic/treeview/fileSystemHelper.h"
#include "treeModel.h"

// Qt includes
#include <QDir>
#include <QFileInfo>
#include <QIcon>

TreeItem::TreeItem(const QString &path, const bool isNote, NoteTreeModel *model, TreeItem *parentItem)
    : m_parentItem(parentItem)
    , m_model(model)
    , m_isNote(isNote)
{
    const QFileInfo fileInfo(path);
    Q_ASSERT(fileInfo.exists());

    const QString fileName = fileInfo.fileName();

    m_name = fileInfo.isFile() ? fileName.chopped(3) : fileName;
    m_isNote = fileInfo.isFile();
    m_path = fileInfo.absoluteFilePath();

    const QString metadataPath =
        m_path + QStringLiteral("/") + (m_isNote ? m_name : QStringLiteral("/.klevernotesFolder.metadata.json")) + QStringLiteral(".metadata.json");

    if (!QFile(metadataPath).exists()) {
        fileSystemHelper::createFile(metadataPath);
    }
    if (m_isNote) {
        const QString todoPath = m_path + QStringLiteral("/") + m_name + QStringLiteral(".todo.json");
        if (!QFile(todoPath).exists()) {
            fileSystemHelper::createFile(todoPath);
        }
    }

    const QFileInfoList fileList = QDir(path).entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries | QDir::Filter::AccessMask, QDir::Name);

    for (const QFileInfo &file : fileList) {
        const QString name = file.fileName();

        if (name.startsWith(QStringLiteral(".")) || (file.isFile() && !name.endsWith(QStringLiteral(".md")))) {
            continue;
        }

        auto subTree = std::make_unique<TreeItem>(file.absoluteFilePath(), file.isFile(), m_model, this);

        appendChild(std::move(subTree));
    }

    // check out metadata and deal with them
}

void TreeItem::appendChild(std::unique_ptr<TreeItem> &&item)
{
    // if (item->m_depth_level == 3 && m_model->noteMapEnabled()) {
    //     // very important to make a copy here !
    //     const QString path = item->data(NoteTreeModel::PathRole).toString().remove(KleverConfig::storagePath());
    //     if (m_model->isInit()) {
    //         Q_EMIT m_model->newGlobalPathFound(path);
    //     } else {
    //         m_model->addInitialGlobalPath(path);
    //     }
    // }
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
        return m_path;
    }

    case Qt::DisplayRole:
    case NoteTreeModel::DisplayNameRole:
        return m_name;

    case Qt::DecorationRole:
        return QIcon::fromTheme(QStringLiteral("document-edit-sign"));

    case NoteTreeModel::IconNameRole:
        return m_isNote ? QStringLiteral("document-open-symbolic") : QStringLiteral("document-edit-sign-symbolic");

    case NoteTreeModel::UseCaseRole:
        return m_isNote ? QStringLiteral("Note") : QStringLiteral("Category");

    case NoteTreeModel::NoteNameRole:
        return data(NoteTreeModel::DisplayNameRole);

    case NoteTreeModel::BranchNameRole:
        return QLatin1String();

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
    return 0;
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
    m_name = name;
}

void TreeItem::setRealName(const QString &name)
{
    m_name = name;
}

QString TreeItem::getRealName() const
{
    return m_name;
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
