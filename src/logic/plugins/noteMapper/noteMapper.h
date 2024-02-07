/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/
#pragma once

#include <QAbstractItemModel>
#include <QSet>
#include <QVariant>
#include <memory>

class LinkedNoteItem
{
public:
    explicit LinkedNoteItem(const QString &path,
                            const QString &exists,
                            const QString &header,
                            const bool headerExists,
                            const int headerLevel,
                            const QString &title);

    QVariant data(int role) const;
    void updatePath(const QString &path);
    void updateExists(const QString &exists);
    void updateHeaderExists(const bool exists);

private:
    void setDisplayPath(const QString &path);
    QString m_path;
    QString m_displayPath;
    QString m_exists; // QString because we use a KSortFilterProxyModel

    QString m_header;
    bool m_headerExists;
    int m_headerLevel;

    QString m_title;
};

class NoteMapper : public QAbstractItemModel
{
    Q_OBJECT
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

    void clear();
    void addRow(const QStringList &infos);

    Q_INVOKABLE QVariantList getCleanedHeaderAndLevel(const QString &header) const;
    Q_INVOKABLE void saveMap() const;

    Q_INVOKABLE QList<QVariantMap> getNoteHeaders(const QString &notePath);

    // Treeview
    Q_INVOKABLE void addInitialGlobalPaths(const QStringList &paths);
    Q_INVOKABLE void addGlobalPath(const QString &path);
    Q_INVOKABLE void updateGlobalPath(const QString &_oldPath, const QString &_newPath);
    Q_INVOKABLE void removeGlobalPath(const QString &_path);

    // Parser
    Q_INVOKABLE void addLinkedNotesInfos(const QList<QStringList> &linkedNotesInfos);
    Q_INVOKABLE void updatePathInfo(const QString &path, const QStringList &headers);

private:
    QVariantMap m_existsMap;
    QVariantMap m_savedMap;

    QVariantMap getPathInfo(const QString &path) const;

    // Model
    std::vector<std::unique_ptr<LinkedNoteItem>> m_list;

    // Treeview
    QSet<QString> m_treeViewPaths;
};
