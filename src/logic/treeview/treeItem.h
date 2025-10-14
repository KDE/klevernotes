// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

// Qt includes
#include <QJsonArray>
#include <QVariant>

// C++ includes
#include <memory>

class NoteTreeModel;

/**
 * @class TreeItem
 * @brief Class representing a tree item.
 */
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
     * @brief Insert child at index.
     *
     * @param child: a unique_ptr to the TreeItem to be inserted.
     * @param index: the index at which the child should be inserted.
     */
    void insertChild(std::unique_ptr<TreeItem> &&child, int index);

    /*
     * @brief Make the Item ask to be expended by the KDescendantsProxyModel inside the Treeview.
     * Usually works in tandem with `askForFocus`.
     *
     * @param itemIndex: The QModelIndex pointing to this row.
     */
    void askForFocus(const QModelIndex &itemIndex);

    /*
     * @brief Make the Item ask to be expended by the KDescendantsProxyModel inside the Treeview.
     * Usually works in tandem with `askForFocus`.
     *
     * @param itemModelIndex: The QModelIndex pointing to this row.
     */
    void askForExpand(const QModelIndex &itemIndex);

    /*
     * @brief Whether this Item is a note.
     *
     * @return True if the item represent a note, false otherwise.
     */
    bool isNote() const;

    /*
     * @brief Get the path to the "closest" directory. The parent if it's a note, the Item path itself otherwise.
     *
     * @return The path to closest directory.
     */
    QString getDir() const;

    /*
     * @brief Get the parent Item of this Item.
     *
     * @return The parent TreeItem of this object.
     */
    TreeItem *getParentItem() const;

    /*
     * @brief Get the name of this Item.
     *
     * @return The name of this item.
     */
    QString getName() const;

    /*
     * @brief Get the path of this Item.
     *
     * @return The path of this item.
     */
    QString getPath() const;

    /*
     * @brief Removes itself from its parent.
     */
    void remove();

    /*
     * @brief Save the Items Metadata recursively.
     */
    void saveMetaData();

    /*
     * @brief Set the given color.
     *
     * @param color The color for this item.
     */
    void setColor(QString color);

    /*
     * @brief Set the given icon.
     *
     * @param icon The icon for this item.
     */
    void setIcon(QString icon);

    /*
     * @brief Set the given parent item.
     *
     * @param parentItem The tree item to be used as the parent.
     */
    void setParentItem(TreeItem *parentItem);

    /*
     * @brief Set the given path.
     *
     * @param path The path to be set.
     */
    void setPath(const QString &path);

    /*
     * @brief Set the given name.
     *
     * @param name The name to be set.
     */
    void setName(const QString &name);

    /*
     * @brief Takes and return the child at the `row` index.
     *
     * @param row The index of the wanted row.
     * @return A unique_ptr of the TreeItem at the `row` index.
     */
    std::unique_ptr<TreeItem> takeUniqueChildAt(int row);

    /*
     * @brief Get the index of where the new child would be based on the compare method.
     * Default to the end of the list.
     *
     * @param name Name of the new child.
     * @param isNote Whether the new child is a note (a directory otherwise).
     */
    int getNewChildIndex(const QString &name, bool isNote) const;

    /*
     * @brief Compare this TreeItem to the other.
     *
     * @param other A TreeItem unique_ptr.
     */
    int compare(const std::unique_ptr<TreeItem> &other) const;

    /* Will come later...*/
    /*
    public:
        // Metadata
        int place = -1;
    */
private:
    /*
     * @brief Set the Item metadata.
     */
    void setMetaData();

    /*
     * @brief Change the parent path part in the path.
     *
     * @param newPart The new part.
     */
    void changeParentPathPart(const QString &newPart);

    /*
     * @brief Order the children.
     */
    void orderChildren();

    /*
     * @brief Compare this TreeItem to another based on name and status.
     *
     * @param name The other TreeItem name.
     * @param isNote Whether the other TreeItem is a note (a directory otherwise).
     * @return A negative integere if this item name is inferior to `name`, a positive one if it is superior, 0 if it is equal.
     */
    int compare(const QString &name, bool isNote) const;

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
