/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>
*/
#pragma once

#include <QAbstractItemModel>
#include <QQmlEngine>
#include <QSet>
#include <QVariant>

/**
 * @class LinkedNoteItem
 * @brief Class representing a Linked Note info.
 */
class LinkedNoteItem
{
public:
    explicit LinkedNoteItem(const QString &path,
                            const bool exists,
                            const QString &header,
                            const bool headerExists,
                            const int headerLevel,
                            const QString &title);

    QVariant data(int role) const;

    /**
     * @brief Update the full path and displayed path.
     *
     * @param path The new path.
     */
    void updatePath(const QString &path);

    /**
     * @brief Update whether the file pointed by the path exists.
     *
     * @param exists Whether it exists.
     */
    void updateExists(const bool exists);

    /**
     * @brief Update whether the header exists in the note.
     *
     * @param exists Whether it exists.
     */
    void updateHeaderExists(const bool exists);

private:
    /**
     * @brief Set the displayed path.
     *
     * @param path The full path.
     */
    void setDisplayPath(const QString &path);
    QString m_path;
    QString m_displayPath;
    bool m_exists;

    QString m_header;
    bool m_headerExists;
    int m_headerLevel;

    QString m_title;
};

/**
 * @class NoteMapper
 * @brief Model exposed to QML holding LinkedNoteItem.
 */
class NoteMapper : public QAbstractItemModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(int count READ rowCount CONSTANT) // QML will handle the signal and change it for us
public:
    explicit NoteMapper(QObject *parent = nullptr);

    enum ExtraRoles {
        PathRole = Qt::UserRole + 1, // To get a string with the fullPath of the Category/Group/Note
        DisplayedPathRole, // To get a string with the name of the Category/Group/Note to be displayed instead of the hidden name
        ExistsRole, // To know if the path exist or not
        HeaderRole, // To get the referenced header
        HeaderExistsRole, // To know if the header exists
        HeaderLevelRole, // To know the level (1-6) of the header
        TitleRole, // To get the title displayed on the preview
    };

    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Clears the model.
     */
    void clear();

    /**
     * @brief Add a new LinkedNoteItem to the model based on the given info.
     *
     * @param infos The informations about the new LinkedNoteItem.
     */
    void addRow(const QStringList &infos);

    /**
     * @brief Get the text and level of a given header.
     *
     * @param header The header to be treated.
     * @return A list containing the level and the text of the given header.
     */
    Q_INVOKABLE QVariantList getCleanedHeaderAndLevel(const QString &header) const;

    /**
     * @brief Save the model to a json file.
     */
    Q_INVOKABLE void saveMap() const;

    /**
     * @brief Get a list of headers infos for the note located at `notePath`.
     *
     * @param notePath The path to the note.
     * @return A list of map with the headers info, header text as key and full header as value.
     */
    Q_INVOKABLE QList<QVariantMap> getNoteHeaders(const QString &notePath);

    // Treeview
    /**
     * @brief Add all the path found when the Treeview is created.
     *
     * @param paths A list of all the paths found.
     */
    Q_INVOKABLE void addInitialGlobalPaths(const QStringList &paths);

    /**
     * @brief Add a path that was newly added to the Treeview.
     *
     * @param path The newly added path.
     */
    Q_INVOKABLE void addGlobalPath(const QString &path);

    /**
     * @brief Update an already existing path.
     *
     * @param _oldPath The old path.
     * @param _newPath The new path.
     */
    Q_INVOKABLE void updateGlobalPath(const QString &_oldPath, const QString &_newPath);

    /**
     * @brief Remove a path.
     *
     * @param _path The path to remove.
     */
    Q_INVOKABLE void removeGlobalPath(const QString &_path);

    // Parser
    /**
     * @brief Add new linked notes to the model.
     *
     * @param linkedNotesInfos A list of informations about the linked notes.
     */
    Q_INVOKABLE void addLinkedNotesInfos(const QList<QStringList> &linkedNotesInfos);

    /**
     * @brief Update the informations of a specific path.
     *
     * @param path The path to update.
     * @param headers A list of headers to associate to this path.
     */
    Q_INVOKABLE void updatePathInfo(const QString &path, const QStringList &headers);

private:
    QVariantMap m_existsMap;
    QVariantMap m_savedMap;

    /**
     * @brief Get the information of a given path.
     *
     * @param path The path for which we want the informations.
     * @return A map containing all the necessary informations.
     */
    QVariantMap getPathInfo(const QString &path) const;

    // Model
    std::vector<std::unique_ptr<LinkedNoteItem>> m_list;

    // Treeview
    QSet<QString> m_treeViewPaths;
};
