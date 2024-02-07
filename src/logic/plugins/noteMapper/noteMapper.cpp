/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#include "noteMapper.h"
#include "kleverconfig.h"
#include "logic/documentHandler.h"
#include "noteMapperUtils.h"
// #include <QDebug>
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
    setDisplayPath(path);
}

void LinkedNoteItem::setDisplayPath(const QString &path)
{
    auto newPath = path;
    newPath.replace(QStringLiteral(".BaseCategory"), KleverConfig::defaultCategoryDisplayNameValue()).remove(QStringLiteral(".BaseGroup/"));
    m_displayPath = newPath;
}

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
    m_savedMap = NoteMapperUtils::convertSavedMap(DocumentHandler::getJson(mapPath));
}

void NoteMapper::saveMap() const
{
    const QJsonObject json = QJsonObject::fromVariantMap(m_existsMap);
    const QString savingPath = KleverConfig::storagePath() + QStringLiteral("/notesMap.json");
    DocumentHandler::saveJson(json, savingPath);
}

QModelIndex NoteMapper::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return createIndex(row, column, static_cast<LinkedNoteItem *>(m_list.at(row).get()));
}

QHash<int, QByteArray> NoteMapper::roleNames() const
{
    return {
        {DisplayedPathRole, "displayedPath"},
        {PathRole, "notePath"},
        {ExistsRole, "exists"},
        {HeaderRole, "header"},
        {HeaderExistsRole, "headerExists"},
        {HeaderLevelRole, "headerLevel"},
        {TitleRole, "title"},
    };
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
    const LinkedNoteItem *item = static_cast<LinkedNoteItem *>(index.internalPointer());
    return item->data(role);
}

void NoteMapper::clear()
{
    beginResetModel();
    m_list.clear();
    endResetModel();
}

QVariantMap NoteMapper::getPathInfo(const QString &path) const
{
    static const QVariantMap emptyMap;
    return m_existsMap.contains(path) ? m_existsMap[path].toMap() : emptyMap;
}

void NoteMapper::addRow(const QStringList &infos)
{
    bool headerExists = false;
    const QString path = infos.at(0);
    const QString header = infos.at(1);
    const QString title = infos.at(2);

    const QString cleanedHeader = NoteMapperUtils::cleanHeader(header); // The header is trimmed by default, see inlineLexer wikilink => captured(3).trimmed
    const int headerLevel = NoteMapperUtils::headerLevel(cleanedHeader);
    const QString headerText = NoteMapperUtils::headerText(cleanedHeader);

    QString exists = QStringLiteral("No"); // Not a bool because used for KSortFilterProxyModel filterString
    if (m_treeViewPaths.contains(path)) {
        exists = QStringLiteral("Yes");

        static const QString headersStr = QStringLiteral("headers");
        static const QString entirelyCheckStr = QStringLiteral("entirelyCheck");
        if (!headerText.isEmpty()) {
            const QVariantMap pathInfo = getPathInfo(path);
            const bool entirelyCheck = NoteMapperUtils::entirelyChecked(pathInfo);
            QStringList headers = NoteMapperUtils::getNoteHeaders(pathInfo);
            headerExists = headers.contains(cleanedHeader);

            if (!headerExists && !entirelyCheck) {
                const QString filePath = KleverConfig::storagePath() + path + QStringLiteral("/note.md");
                headerExists = DocumentHandler::checkForHeader(filePath, cleanedHeader);
                if (headerExists) {
                    // We don't update the "entirelyCheck" value, only NoteMapper::updatePathInfo can do it
                    headers.append(cleanedHeader);
                    const QVariantMap newPathInfo = {{headersStr, QVariant(headers)}, {entirelyCheckStr, QVariant(entirelyCheck)}};
                    m_existsMap[path] = newPathInfo;
                }
            }
        }
    }

    auto newRow = std::make_unique<LinkedNoteItem>(path, exists, headerText, headerExists, headerLevel, title); // making it a const prevent std::move

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_list.push_back(std::move(newRow));
    endInsertRows();
}

QVariantList NoteMapper::getCleanedHeaderAndLevel(const QString &header) const
{
    const QString cleanedHeader = NoteMapperUtils::cleanHeader(header);
    const int headerLevel = NoteMapperUtils::headerLevel(cleanedHeader);
    const QString headerText = NoteMapperUtils::headerText(cleanedHeader);

    return {headerText, headerLevel};
}

QList<QVariantMap> NoteMapper::getNoteHeaders(const QString &notePath)
{
    const QString cleanedPath = QString(notePath).remove(KleverConfig::storagePath());
    const QVariantMap pathInfo = getPathInfo(cleanedPath);

    if (NoteMapperUtils::entirelyChecked(pathInfo)) {
        const QStringList headers = NoteMapperUtils::getNoteHeaders(pathInfo);
        return NoteMapperUtils::getHeadersComboList(headers);
    }

    QString note = DocumentHandler::readFile(notePath + QStringLiteral("/note.md"));

    static const QRegularExpression block_fences =
        QRegularExpression(QStringLiteral("^ *(\\`{3,}|~{3,})[ \\.]*(\\S+)? *\n([\\s\\S]*?)\n? *\\1 *(?:\n+|$)"), QRegularExpression::MultilineOption);
    note.remove(block_fences); // Avoid things like "#include <lib>" to be catched as a heading

    static const QRegularExpression block_heading =
        QRegularExpression(QStringLiteral("^ *(#{1,6}) *([^\n]+?) *(?:#+ *)?(?:\n+|$)"), QRegularExpression::MultilineOption);
    QRegularExpressionMatchIterator heading_i = block_heading.globalMatch(note);

    QStringList headers;
    QRegularExpressionMatch match;
    while (heading_i.hasNext()) {
        match = heading_i.next();
        const QString header = match.captured(0).trimmed();
        headers.append(header);
    }

    updatePathInfo(cleanedPath, headers);

    return NoteMapperUtils::getHeadersComboList(headers);
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
        const auto child = static_cast<LinkedNoteItem *>(it->get());

        if (child->data(PathRole).toString() == path && child->data(ExistsRole) != QStringLiteral("Yes")) {
            child->updateExists(QStringLiteral("Yes"));
            // Don't need to check for header since this is a brand new file
            QModelIndex childIndex = createIndex(0, 0, child);
            Q_EMIT dataChanged(childIndex, childIndex);
        }
    }
}

void NoteMapper::updateGlobalPath(const QString &_oldPath, const QString &_newPath)
{
    const auto oldPath = QString(_oldPath).remove(KleverConfig::storagePath());
    const auto newPath = QString(_newPath).remove(KleverConfig::storagePath());
    if (!m_treeViewPaths.remove(oldPath))
        return;

    if (m_existsMap.contains(oldPath))
        m_existsMap.insert(newPath, m_existsMap.take(oldPath));

    m_treeViewPaths.insert(newPath);
    for (auto it = m_list.cbegin(); it != m_list.cend(); it++) {
        const auto child = static_cast<LinkedNoteItem *>(it->get());

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
            const QModelIndex childIndex = createIndex(0, 0, child);
            Q_EMIT dataChanged(childIndex, childIndex);
        }
    }
}

void NoteMapper::removeGlobalPath(const QString &_path)
{
    const auto path = QString(_path).remove(KleverConfig::storagePath());
    if (!m_treeViewPaths.contains(path))
        return;

    m_existsMap.remove(path);

    m_treeViewPaths.erase(m_treeViewPaths.find(path));

    for (auto it = m_list.cbegin(); it != m_list.cend();) {
        const LinkedNoteItem *child = static_cast<LinkedNoteItem *>(it->get());

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
void NoteMapper::addLinkedNotesInfos(const QList<QStringList> &linkedNotesInfos)
{
    clear();

    for (auto infos = linkedNotesInfos.begin(); infos != linkedNotesInfos.end(); infos++) {
        addRow(*infos);
    }
}

void NoteMapper::updatePathInfo(const QString &path, const QStringList &headers)
{
    const QVariantMap newPathInfo = {{QStringLiteral("headers"), QVariant(headers)}, {QStringLiteral("entirelyCheck"), QVariant(true)}};

    m_existsMap[path] = newPathInfo;
}
