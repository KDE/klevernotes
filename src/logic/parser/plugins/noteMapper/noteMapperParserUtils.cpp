/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024-2025 Louis Schul <schul9louis@gmail.com>
*/

#include "noteMapperParserUtils.h"
#include "kleverconfig.h"
#include "logic/editor/editorHandler.hpp"

#define slash QStringLiteral("/")

NoteMapperParserUtils::NoteMapperParserUtils(MdEditor::EditorHandler *editorHandler)
    : m_editorHandler(editorHandler)
{
}

QString NoteMapperParserUtils::sanitizePath(const QString &_path, const QString &notePath)
{
    QString path(_path);
    if (_path.endsWith(QStringLiteral(".md"))) {
        path.chop(3);
    }

    QDir dir(path);
    QDir note(notePath);

    static const QString dot = QStringLiteral(".");
    QString finalPath = {};
    QString noteName = dir.dirName();
    bool valid = !noteName.isEmpty() && !noteName.startsWith(dot);
    if (dir.isRelative() && valid) {
        QString prefix = QStringLiteral("../"); // This way we don't have to `cdUp` the note
        if (!dir.path().startsWith(dot)) {
            prefix += QStringLiteral("./");
        }
        dir.setPath(path);

        QString cdPath = prefix + path;
        // Simply doing `cdUp` on `dir` doesn't always work we need another way to get the cdPath
        cdPath = cdPath.left(cdPath.length() - noteName.length());

        valid = note.cd(cdPath);
        finalPath = note.path() + slash + noteName;
    } else if (valid) {
        dir = QDir(KleverConfig::storagePath() + path);
        finalPath = dir.path();
    }
    finalPath.remove(0, KleverConfig::storagePath().length());

    return valid ? finalPath : QLatin1String();
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
