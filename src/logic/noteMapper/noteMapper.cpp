/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#include "noteMapper.h"
#include "kleverconfig.h"
#include <QDebug>
#include <QJsonArray>

LinkedNoteItem::LinkedNoteItem(const QString &path,
                               const QString &exists,
                               const QString &header,
                               const bool headerExists,
                               const int headerLevel,
                               const QString &title)
    : m_exists(exists)
    , m_header(header)
    , m_headerExists(headerExists)
    , m_headerLevel(headerLevel)
    , m_title(title)
{
    updatePath(path);
}

QVariant LinkedNoteItem::data(int role) const
{
    switch (role) {
    case NoteMapper::PathRole:
        return m_path;

    case NoteMapper::DisplayedPathRole:
        return m_displayPath;

    case NoteMapper::ExistsRole:
        return m_exists;

    case NoteMapper::HeaderRole:
        return m_header;

    case NoteMapper::HeaderExistsRole:
        return m_headerExists;

    case NoteMapper::HeaderLevelRole:
        return m_headerLevel;

    case NoteMapper::TitleRole:
        return m_title;
    }
    // default Q_UNREACHABLE would crash the app when doing the following :
    // - be in a note
    // - click on a group/category
    // - go back to the same note
    // => repeater goes crazy and send a role = 0, 2 times for each entry
    // before going back to normal
    return 0;
};

void LinkedNoteItem::updatePath(const QString &path)
{
    m_path = path;
    QString toDisplay(path);
    setDisplayPath(toDisplay);
}

void LinkedNoteItem::setDisplayPath(const QString &path)
{
    auto newPath = path;
    newPath.replace(".BaseCategory", KleverConfig::defaultCategoryDisplayNameValue()).remove(QStringLiteral(".BaseGroup/"));

void LinkedNoteItem::updateExists(const QString &exists)
{
    m_exists = exists;
}

void LinkedNoteItem::updateHeaderExists(const bool exists)
{
    m_headerExists = exists;
}

NoteMapper::NoteMapper(QObject *parent)
    : QAbstractItemModel(parent)
{
    const QString mapPath = KleverConfig::storagePath() + QStringLiteral("/notesMap.json");
    convertSavedMap(m_documentHandler->getSavedMap(mapPath));
}

void NoteMapper::convertSavedMap(const QJsonObject &savedMap)
{
    QString path;
    QJsonArray headersJson;
    QString header;
    for (auto jsonIter = savedMap.begin(); jsonIter != savedMap.end(); jsonIter++) {
        path = jsonIter.key();
        headersJson = jsonIter.value().toArray();
        QStringList headers;
        for (auto headersIter = headersJson.begin(); headersIter != headersJson.end(); headersIter++) {
            header = headersIter->toString();
            headers.append(header);
        }
        m_savedMap.insert(path, headers);
    }
}

void NoteMapper::saveMap()
{
    QJsonObject map;
    QVariantMap vmap;

    QMapIterator<QString, QStringList> i(m_existsMap);
    while (i.hasNext()) {
        i.next();
        vmap.insert(i.key(), i.value());
    }

    QJsonObject json = QJsonObject::fromVariantMap(vmap);
    QString savingPath = KleverConfig::storagePath() + QStringLiteral("/notesMap.json");
    m_documentHandler->saveMap(json, savingPath);
}

QModelIndex NoteMapper::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return createIndex(row, column, static_cast<LinkedNoteItem *>(m_list.at(row).get()));
}

QHash<int, QByteArray> NoteMapper::roleNames() const
{
    return {{DisplayedPathRole, "displayedPath"},
            {PathRole, "notePath"},
            {ExistsRole, "exists"},
            {HeaderRole, "header"},
            {HeaderExistsRole, "headerExists"},
            {HeaderLevelRole, "headerLevel"},
            {TitleRole, "title"},};
}

QModelIndex NoteMapper::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return {};
}

int NoteMapper::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_list.size();
}

int NoteMapper::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant NoteMapper::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    const auto item = static_cast<LinkedNoteItem *>(index.internalPointer());

    return item->data(role);
}

void NoteMapper::clear()
{
    beginResetModel();
    m_list.clear();
    endResetModel();
    m_existingLinkedNoteInfos.clear();
}

void NoteMapper::addRow(const QString &path, const QString &header, const QString &title)
{
    std::tuple<QString, QString, QString> linkedNoteInfo = {path, header, title};
    if (m_existingLinkedNoteInfos.find(linkedNoteInfo) != m_existingLinkedNoteInfos.cend()) // Can't use .contains (C++ 20)
        return;

    m_existingLinkedNoteInfos.insert(linkedNoteInfo);

    int headerLevel = 0;
    bool headerExists = false;
    QString cleanedHeader(header);
    QString exists = QStringLiteral("No");
    if (m_treeViewPaths.contains(path)) {
        exists = QStringLiteral("Yes");

        if (!cleanedHeader.isEmpty()) {
            cleanHeader(cleanedHeader);

            if (m_existsMap.contains(path)) {
                headerExists = m_existsMap[path].contains(cleanedHeader);
            } else {
                QString filePath = KleverConfig::storagePath() + path + QStringLiteral("/note.md");
                headerExists = m_documentHandler->readFile(filePath, cleanedHeader) == QStringLiteral("true");
                if (headerExists) {
                    QStringList headers(cleanedHeader);
                    m_existsMap.insert(path, headers);
                }
            }
            headerLevel = getHeaderLevel(cleanedHeader);
        }
    }

    const auto newRow = std::make_unique<LinkedNoteItem>(path, exists, cleanedHeader, headerExists, headerLevel, title);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_list.push_back(std::move(newRow));
    endInsertRows();
}

int NoteMapper::getHeaderLevel(QString &header)
{
    int level = 0;

    while (!header.isEmpty() && header[0] == QStringLiteral("#")) {
        header.remove(0, 1);
        level++;
        if (level == 6)
            break;
    }

    header = header.trimmed(); // There's a space between the '#' and the header text
    return level;
}

void NoteMapper::cleanHeader(QString &header)
{
    // Note: This receive a trimmed string
    QString space = QStringLiteral(" ");
    QString hashtag = QStringLiteral("#");
    if (!header.isEmpty() && !header.startsWith(hashtag)) {
        header = hashtag + space + header;
        return;
    }

    int strSize = header.size();
    for (int i = 0; i < strSize; i++) {
        if (i > 6 && header[i] != space) { // case: ######something
            header = hashtag.repeated(6) + space + header.remove(0, 6);
            return;
        }

        if (header[i] != hashtag) { // case: ####something ; #### something
            if (header[i] != space) { // Combaning the ifs would exclude second case on return
                header = hashtag.repeated(i) + space + header.remove(0, i);
            }
            return;
        }
    }

    // The header was just a maximum of 6 concatenated '#'
    header = QStringLiteral("");
}

QVariantList NoteMapper::getCleanedHeaderAndLevel(QString header)
{
    cleanHeader(header);
    int headerLevel = getHeaderLevel(header);

    return {header, headerLevel};
}

// Treeview
void NoteMapper::addInitialGlobalPaths(const QStringList &paths)
{
    for (const auto &path : paths) {
        m_treeViewPaths.insert(path);

        if (m_savedMap.contains(path)) {
            m_existsMap.insert(path, m_savedMap.take(path));
        }
    }
    m_savedMap.clear(); // No need to preserve the rest
}

void NoteMapper::addGlobalPath(const QString &path)
{
    m_treeViewPaths.insert(path);

    for (auto it = m_list.cbegin(); it != m_list.cend(); it++) {
        auto child = static_cast<LinkedNoteItem *>(it->get());

        if (child->data(PathRole).toString() == path && child->data(ExistsRole) != QStringLiteral("Yes")) {
            child->updateExists(QStringLiteral("Yes"));
            // Don't need to check for header since this is a brand new file
            QModelIndex childIndex = createIndex(0, 0, child);
            Q_EMIT dataChanged(childIndex, childIndex);
        }
    }
}

void NoteMapper::updateGlobalPath(const QString &oldPath, const QString &newPath)
{
    if (!m_treeViewPaths.remove(oldPath))
        return;

    if (m_existsMap.contains(oldPath))
        m_existsMap.insert(newPath, m_existsMap.take(oldPath));

    m_treeViewPaths.insert(newPath);
    for (auto it = m_list.cbegin(); it != m_list.cend(); it++) {
        auto child = static_cast<LinkedNoteItem *>(it->get());

        bool needUpdate = false;
        if (child->data(PathRole).toString() == oldPath) {
            needUpdate = true;
            child->updatePath(newPath);
        }
        if (child->data(PathRole).toString() == newPath) {
            needUpdate = true;
            child->updateExists(QStringLiteral("Yes"));
        }

        if (needUpdate) {
            QModelIndex childIndex = createIndex(0, 0, child);
            Q_EMIT dataChanged(childIndex, childIndex);
        }
    }
}

void NoteMapper::removeGlobalPath(const QString &path)
{
    if (!m_treeViewPaths.contains(path))
        return;

    m_existsMap.remove(path);

    m_treeViewPaths.erase(m_treeViewPaths.find(path));

    for (auto it = m_list.cbegin(); it != m_list.cend();) {
        auto child = static_cast<LinkedNoteItem *>(it->get());

        if (child->data(PathRole).toString() == path) {
            beginRemoveRows(QModelIndex(), it - m_list.begin(), it - m_list.begin());
            m_list.erase(it);
            endRemoveRows();
        } else {
            it++;
        }
    }
}

// Parser
void NoteMapper::addNotePaths(const QStringList &linkedNoteInfos)
{
    Q_ASSERT(linkedNoteInfos.size() % 3 == 0);

    clear();

    for (int i = 0; i < linkedNoteInfos.size(); i += 3) {
        addRow(linkedNoteInfos[i], linkedNoteInfos[i + 1], linkedNoteInfos[i + 2]);
    }
}

void NoteMapper::updatePathInfo(const QString &path, const QStringList &headers)
{
    m_existsMap[path] = headers;
}
