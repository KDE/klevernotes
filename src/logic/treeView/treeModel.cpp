// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#include "treeModel.h"
#include "kleverconfig.h"
#include <QDir>

MyTreeModel::MyTreeModel(QObject *parent)
    : QStandardItemModel(parent)
{
}

QHash<int, QByteArray> MyTreeModel::roleNames() const
{
    static QHash<int, QByteArray> mapping{
        {DisplayedName, "displayedName"},
        {Path, "path"},
        {Level, "level"},
        {UseCase, "useCase"},
        {Icon, "iconName"},
    };
    return mapping;
}

QStandardItem *MyTreeModel::makeRow(const QString &path, const int lvl)
{
    QFileInfo testingFile(path);

    if (!testingFile.exists())
        return nullptr;

    QString rowName = testingFile.fileName();
    QString displayedName = (rowName == ".BaseCategory") ? KleverConfig::categoryDisplayName() : rowName;
    QString rowPath = testingFile.absoluteFilePath();
    QString iconName;
    QString useCase;
    switch (lvl) {
    case 0:
        useCase = "Category";
        iconName = "documentation";
        break;
    case 1:
        useCase = "Group";
        iconName = "document-open-folder";
        break;
    default:
        useCase = "Note";
        iconName = "document-edit-sign";
        break;
    }

    QStandardItem *entry = new QStandardItem(rowPath);
    entry->setData(displayedName, DisplayedName);
    entry->setData(rowPath, Path);
    entry->setData(lvl, Level);
    entry->setData(useCase, UseCase);
    entry->setData(iconName, Icon);

    return entry;
}

QStandardItem *MyTreeModel::getHierarchy(const QString &path, const int lvl)
{
    QStandardItem *finalTree = makeRow(path, lvl);

    if (finalTree == nullptr)
        return nullptr;

    QFileInfoList fileList = QDir(path).entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries | QDir::Filter::AccessMask);

    foreach (const QFileInfo &file, fileList) {
        QString name = file.fileName();
        bool isNotBaseFolder = (name != ".BaseCategory" && name != ".BaseGroup");
        if (!file.isDir() || (name.startsWith(".") && isNotBaseFolder))
            continue;

        QString nextPath = file.absoluteFilePath();

        if (lvl + 1 < 3) {
            QStandardItem *subTree = getHierarchy(nextPath, lvl + 1);

            if (name == ".BaseGroup") {
                for (int i = subTree->rowCount(); --i >= 0;) {
                    QStandardItem *categoryNote = subTree->takeRow(i).first();
                    categoryNote->setData(1, Level);
                    finalTree->insertRow(0, categoryNote);
                }
            } else
                finalTree->appendRow(subTree);
        }
    }

    return finalTree;
}

void MyTreeModel::makeModel(const QString &path)
{
    QDir dir(path);
    if (!dir.exists())
        return;

    m_storagePath = dir.absolutePath();

    QStandardItem *rootItem = this->invisibleRootItem();

    QFileInfoList categoryList = dir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries | QDir::Filter::AccessMask);

    foreach (const QFileInfo &file, categoryList) {
        QString categoryName = file.fileName();
        if (categoryName == ".directory")
            continue;

        QString categoryPath = file.absoluteFilePath();

        QStandardItem *categoryTree = getHierarchy(categoryPath, 0);
        if (categoryTree == nullptr)
            continue;

        if (categoryName == ".BaseCategory") {
            rootItem->insertRow(0, categoryTree);
            m_basedCategoryObject = categoryTree;
        } else
            rootItem->appendRow(categoryTree);
    }
}

void MyTreeModel::removeFromTree(const QModelIndex &rowModelIndex)
{
    QStandardItem *rowItem = this->itemFromIndex(rowModelIndex);
    if (rowItem == nullptr)
        return;

    QStandardItem *parent = rowItem->parent();

    if (parent == nullptr)
        this->removeRow(rowItem->row());
    else
        parent->removeRow(rowItem->row());
}

void MyTreeModel::changePath(QStandardItem *&row, const QString &newPart, int newPartIdx)
{
    QStringList currentPathParts = row->data(Path).toString().split("/");
    if (currentPathParts.last() == ".BaseCategory")
        return;

    if (newPartIdx == -1)
        newPartIdx = currentPathParts.size() - 1;
    currentPathParts[newPartIdx] = newPart;

    QString newPath = currentPathParts.join("/");
    row->setData(newPath, Path);
    row->setText(newPath);

    for (int i = row->rowCount(); --i >= 0;) {
        QStandardItem *child = row->child(i);
        changePath(child, newPart, newPartIdx);
    }
}

void MyTreeModel::renameRow(const QModelIndex &rowModelIndex, const QString &newName)
{
    QStandardItem *row = this->itemFromIndex(rowModelIndex);
    if (row == nullptr)
        return;

    QString currentName = row->data(DisplayedName).toString();
    row->setData(newName, DisplayedName);

    changePath(row, newName);

    reorder();
}

void MyTreeModel::addRow(const QString &rowPath, const int rowLevel, const QModelIndex &parentModelIndex)
{
    QStandardItem *newRow = getHierarchy(rowPath, rowLevel);
    if (newRow == nullptr)
        return;

    if (!parentModelIndex.isValid()) {
        this->appendRow(newRow);
        return this->reorder();
    }

    QStandardItem *parent = this->itemFromIndex(parentModelIndex);

    if (parent == nullptr)
        return;

    parent->appendRow(newRow);
    this->reorder();
}

void MyTreeModel::reorder()
{
    this->sort(0);

    if (m_basedCategoryObject != nullptr) {
        int basedCategoryObjectIndex = m_basedCategoryObject->row();
        // This should never append, but who knows
        if (basedCategoryObjectIndex != 0) {
            QStandardItem *basedCategoryObject = this->takeRow(basedCategoryObjectIndex).first();
            this->insertRow(0, basedCategoryObject);
        }
    }
}

QModelIndex MyTreeModel::getParentIndex(const QModelIndex &index)
{
    return index.parent();
}
