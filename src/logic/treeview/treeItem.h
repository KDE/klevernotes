// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

// Qt includes
#include <QVariant>

// C++ includes
#include <memory>

class NoteTreeModel;
class TreeItem
{
public:
    explicit TreeItem(const QString &path, const bool isNote, NoteTreeModel *model, TreeItem *parentItem = nullptr);

    void appendChild(std::unique_ptr<TreeItem> &&child);

    TreeItem *child(int row) const;
    std::unique_ptr<TreeItem> takeUniqueChildAt(int row);
    int childCount() const;
    QVariant data(int role) const;
    int row() const;
    TreeItem *getParentItem() const;
    void setParentItem(TreeItem *parentItem);
    void remove();
    void setRealName(const QString &name);
    QString getRealName() const;
    void setDisplayName(const QString &name);
    void askForFocus(const QModelIndex &itemIndex);
    void askForExpand(const QModelIndex &itemIndex);
    int getDepth() const;

private:
    // Position in tree
    std::vector<std::unique_ptr<TreeItem>> m_childItems;
    TreeItem *m_parentItem;

    NoteTreeModel *m_model;

    // Content
    bool m_isNote;
    QString m_name;
    QString m_path;
    bool m_wantFocus = false;
    bool m_wantExpand = false;
};
