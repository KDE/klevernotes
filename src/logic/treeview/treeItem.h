// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

// Qt includes
#include <QJsonArray>
#include <QVariant>

// C++ includes
#include <memory>

class NoteTreeModel;
class TreeItem
{
public:
    explicit TreeItem(const QString &path, NoteTreeModel *model, TreeItem *parentItem = nullptr);

    // Override default
    void appendChild(std::unique_ptr<TreeItem> &&child);
    TreeItem *child(int row) const;
    int childCount() const;
    int row() const;
    QVariant data(int role) const;

    /*
     * Make the Item ask to be expended by the KDescendantsProxyModel inside the Treeview
     * Usually works in tandem with `askForFocus`
     *
     * @param itemIndex: The QModelIndex pointing to this row
     */
    void askForFocus(const QModelIndex &itemIndex);

    /*
     * Make the Item ask to be expended by the KDescendantsProxyModel inside the Treeview
     * Usually works in tandem with `askForFocus`
     *
     * @param itemModelIndex: The QModelIndex pointing to this row
     */
    void askForExpand(const QModelIndex &itemIndex);

    /*
     * Whether this Item is a note
     */
    bool isNote() const;

    /*
     * Get the path to the "closest" directory. The parent if it's a note, the Item path itself otherwise
     */
    QString getDir() const;

    /*
     * Get the parent Item of this Item
     */
    TreeItem *getParentItem() const;

    /*
     * Get the name of this Item
     */
    QString getName() const;

    /*
     * Get the path of this Item
     */
    QString getPath() const;

    /*
     * Removes itself from its parent
     */
    void remove();

    /*
     * Save the Items Metadata recursively
     */
    void saveMetaData();

    /*
     * Set the given color
     */
    void setColor(QString color);

    /*
     * Set the given icon
     */
    void setIcon(QString icon);

    /*
     * Set the given parent item
     */
    void setParentItem(TreeItem *parentItem);

    /*
     * Set the given path
     */
    void setPath(const QString &path);

    /*
     * Set the given name
     */
    void setName(const QString &name);

    /*
     * Takes and return the child at the `row` index
     */
    std::unique_ptr<TreeItem> takeUniqueChildAt(int row);

public:
    // Metadata
    int place = -1;

private:
    /*
     * Set the Item metadata
     */
    void setMetaData();

private:
    // Position in tree
    std::vector<std::unique_ptr<TreeItem>> m_children;
    TreeItem *m_parentItem = nullptr;

    NoteTreeModel *m_model;

    // Temp Metadata
    QList<QString> m_tempChildrenNames;
    QJsonArray m_tempChildrenInfo;

    // Content
    bool m_isNote;
    QString m_name;
    QString m_path;
    QString m_dir;
    QString m_icon;
    QString m_color;
    bool m_wantFocus = false;
    bool m_wantExpand = false;
};
