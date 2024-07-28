/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "noteMapperParserUtils.h"
#include "kleverconfig.h"
#include "logic/editor/editorHandler.hpp"

NoteMapperParserUtils::NoteMapperParserUtils(MdEditor::EditorHandler *editorHandler)
    : m_editorHandler(editorHandler)
{
}

QString NoteMapperParserUtils::getGroupPath(const QString &notePath)
{
    // notePath == storagePath/Category/Group/Note/ => /Category/Group/Note
    const QString mapperNotePath = notePath.chopped(1).remove(KleverConfig::storagePath());

    // /Category/Group/Note => /Category/Group (no '/' at the end to make it easier for m_categPath)
    QString groupPath = mapperNotePath.chopped(mapperNotePath.size() - mapperNotePath.lastIndexOf(QStringLiteral("/")) - 1);

    return groupPath;
}

QPair<QString, bool> NoteMapperParserUtils::sanitizePath(const QString &_path, const QString &notePath)
{
    static const QString slashStr = QStringLiteral("/");
    QStringList parts = _path.split(slashStr);

    bool leadingSlashRemnant = false;
    for (int i = 0; i < parts.count(); i++) {
        QString part = parts[i].trimmed();
        if (part.isEmpty()) {
            if (i == 0) {
                leadingSlashRemnant = true;
            } else { // The path is not correctly formed
                return qMakePair(_path, false);
            }
        }
        parts[i] = part;
    }

    if (leadingSlashRemnant)
        parts.removeAt(0);

    if (parts[0] == KleverConfig::defaultCategoryDisplayNameValue() && 1 < parts.length()) {
        parts[0] = QStringLiteral(".BaseCategory");
    }

    const QString groupPath = getGroupPath(notePath);

    QString path = _path;
    switch (parts.count()) {
    case 1: // Note name only
        path = groupPath + parts[0];
        break;
    case 2:
        if (parts[0] == QStringLiteral(".")) { // Note name only
            path = groupPath + parts[1];
        } else { // Note inside category
            path = slashStr + parts[0] + QStringLiteral("/.BaseGroup/") + parts[1];
        }
        break;
    case 3: // 'Full' path
        path = slashStr + parts.join(slashStr);
        break;
    default: // Not a note path
        return qMakePair(_path, false);
    }

    return qMakePair(path, true);
}

void NoteMapperParserUtils::setNotePath(const QString &_path)
{
    QString path = _path;
    m_mapperNotePath = path.remove(0, KleverConfig::storagePath().length());
}

// Not used due to issue: https://invent.kde.org/office/klevernotes/-/issues/17
// =====
void NoteMapperParserUtils::setHeaderInfo(const QStringList &headerInfo)
{
    m_header = headerInfo[0];
    m_headerLevel = m_header.isEmpty() ? QStringLiteral("0") : headerInfo[1];
}

QString NoteMapperParserUtils::headerLevel() const
{
    return m_headerLevel;
};

void NoteMapperParserUtils::checkHeaderFound(const QString &header, const QString &level)
{
    if (header == m_header && level == QString(m_headerLevel)) {
        m_headerFound = true;
    }
}

bool NoteMapperParserUtils::headerFound() const
{
    return m_headerFound;
}
// =====

void NoteMapperParserUtils::addToLinkedNoteInfos(const QStringList &infos)
{
    if ((!m_previousLinkedNotesInfos.remove(infos) && !m_linkedNotesInfos.contains(infos)) || infos.isEmpty()) {
        m_linkedNotesChanged = true;
    }
    m_linkedNotesInfos.insert(infos);
}

void NoteMapperParserUtils::addToNoteHeaders(const QString &header)
{
    if (!m_previousNoteHeaders.remove(header) && !m_noteHeaders.contains(header)) {
        m_noteHeadersChanged = true;
    }
    m_noteHeaders.append(header);
}

// Signal about headers not used due to issue: https://invent.kde.org/office/klevernotes/-/issues/17
void NoteMapperParserUtils::postTok()
{
    m_notePathChanged = false;

    // We try to not spam with signals
    if (m_linkedNotesChanged || !m_previousLinkedNotesInfos.isEmpty()) { // The previous is not empty, some links notes are no longer there
        Q_EMIT m_editorHandler->newLinkedNotesInfos(m_linkedNotesInfos);
    }
    m_previousLinkedNotesInfos = m_linkedNotesInfos;
    m_noteHeaders.removeDuplicates();

    if (m_noteHeadersChanged || !m_previousNoteHeaders.isEmpty()) { // The previous is not empty, some headers are no longer there
        m_emptyHeadersSent = false;
        /* Q_EMIT m_editorHandler->noteHeadersSent(m_mapperNotePath, m_noteHeaders); */
    } else if (m_noteHeaders.isEmpty() && !m_emptyHeadersSent) {
        // This way the mapper can receive info about the note (the note has no header), and we still prevent spamming
        m_emptyHeadersSent = true;
        /* Q_EMIT m_editorHandler->noteHeadersSent(m_mapperNotePath, {}); */
    }
    m_previousNoteHeaders = QSet(m_noteHeaders.begin(), m_noteHeaders.end());

    // Not used due to issue: https://invent.kde.org/office/klevernotes/-/issues/17
    if (!m_headerFound) { // Prevent the TextDisplay.qml scrollToHeader to search an unexisting header
        m_headerLevel = QStringLiteral("0");
        m_header = QLatin1String();
    }
}

void NoteMapperParserUtils::clearInfo()
{
    m_linkedNotesInfos.clear();
    m_noteHeaders.clear();
    m_linkedNotesInfos.clear();

    m_headerFound = false;
    if (!m_notePathChanged) {
        m_linkedNotesChanged = false;
    }
}

void NoteMapperParserUtils::clearPreviousInfo()
{
    m_previousLinkedNotesInfos.clear();
    m_previousNoteHeaders.clear();
    m_linkedNotesChanged = true;
    m_notePathChanged = true;
}
