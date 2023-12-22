/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#include "parser.h"

#include "renderer.h"
#include <QJsonArray>

using namespace std;

Parser::Parser(QObject *parent)
    : QObject(parent)
{
}

QPair<QString, bool> Parser::sanitizePath(const QString &_path) const
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

    if (parts[0] == KleverConfig::defaultCategoryDisplayNameValue())
        parts[0] = QStringLiteral(".BaseCategory");

    QString path = _path;
    switch (parts.count()) {
    case 1: // Note name only
        path = m_groupPath + parts[0];
        break;
    case 2:
        if (parts[0] == QStringLiteral(".")) { // Note name only
            path = m_groupPath + parts[1];
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

void Parser::setHeaderInfo(const QStringList &headerInfo)
{
    m_header = headerInfo[0];
    m_headerLevel = m_header.isEmpty() ? QStringLiteral("0") : headerInfo[1];
}

QString Parser::headerLevel() const
{
    return m_headerLevel;
};

void Parser::setNotePath(const QString &notePath)
{
    if (notePath.isEmpty() || m_notePath == notePath) {
        return;
    }

    m_notePath = notePath;
    if (notePath == QStringLiteral("qrc:")) {
        return;
    }
    // We do this here because we're sure to be in another note
    // Syntax highlight
    m_previousNoteCodeBlocks.clear();
    // NoteMapper
    m_previousLinkedNotesInfos.clear();
    m_previousNoteHeaders.clear();
    m_linkedNotesChanged = true;
    m_notePathChanged = true;

    // notePath == storagePath/Category/Group/Note/ => /Category/Group/Note
    m_mapperNotePath = notePath.chopped(1).remove(KleverConfig::storagePath());

    // /Category/Group/Note => /Category/Group (no '/' at the end to make it easier for m_categPath)
    const QString groupPath = m_mapperNotePath.chopped(m_mapperNotePath.size() - m_mapperNotePath.lastIndexOf(QStringLiteral("/")));
    if (m_groupPath != groupPath) {
        m_groupPath = groupPath + QStringLiteral("/"); // /Category/Group => /Category/Group/
        m_categPath = groupPath.chopped(groupPath.size() - groupPath.lastIndexOf(QStringLiteral("/")) - 1); // /Category/Group => /Category/
    }
}

QString Parser::getNotePath() const
{
    return m_notePath;
}

QString Parser::parse(QString src)
{
    links.clear();
    tokens.clear();
    // Syntax highlight
    m_noteCodeBlocks.clear();
    // NoteMapper
    m_linkedNotesInfos.clear();

    m_noteHeaders.clear();
    m_headerFound = false;
    m_linkedNotesInfos.clear();
    if (!m_notePathChanged) {
        m_linkedNotesChanged = false;
    }

    blockLexer.lex(src);

    if (m_highlightEnabled) {
        m_sameCodeBlocks = m_previousNoteCodeBlocks == m_noteCodeBlocks && !m_noteCodeBlocks.isEmpty();
        if (!m_sameCodeBlocks || m_newHighlightStyle) {
            m_previousNoteCodeBlocks = m_noteCodeBlocks;
            m_previousHighlightedBlocks.clear();
            m_newHighlightStyle = false;
            m_sameCodeBlocks = false;
        }
        m_currentBlockIndex = 0;
    }

    std::reverse(tokens.begin(), tokens.end());

    QString out;
    while (getNextToken()) {
        out += tok();
    }

    m_notePathChanged = false;

    if (m_noteMapEnabled) {
        // We try to not spam with signals
        if (m_linkedNotesChanged || !m_previousLinkedNotesInfos.isEmpty()) { // The previous is not empty, some links notes are no longer there
            Q_EMIT newLinkedNotesInfos(m_linkedNotesInfos);
        }
        m_previousLinkedNotesInfos = m_linkedNotesInfos;

        if (m_linkedNotesChanged || !m_previousNoteHeaders.isEmpty()) { // The previous is not empty, some headers are no longer there
            m_emptyHeadersSent = false;
            Q_EMIT noteHeadersSent(m_mapperNotePath, m_noteHeaders.values());
        } else if (m_noteHeaders.isEmpty() && !m_emptyHeadersSent) {
            // This way the mapper can receive info about the note (the note has no header), and we still prevent spamming
            m_emptyHeadersSent = true;
            Q_EMIT noteHeadersSent(m_mapperNotePath, {});
        }
        m_previousNoteHeaders = m_noteHeaders;
    }

    if (!m_headerFound) { // Prevent the TextDisplay.qml scrollToHeader to search an unexisting header
        m_headerLevel = QStringLiteral("0");
        m_header = QLatin1String();
    }

    return out;
}

QString Parser::tok()
{
    const QString type = m_token[QStringLiteral("type")].toString();
    QString outputed, text, body;
    QVariantMap flags;
    static const QString emptyStr = QLatin1String();

    if (type == QStringLiteral("space")) {
        return {};
    }

    if (type == QStringLiteral("hr")) {
        return Renderer::hr();
    }

    if (type == QStringLiteral("heading")) {
        text = m_token[QStringLiteral("text")].toString();

        const QString level = m_token[QStringLiteral("depth")].toString();
        if (text == m_header && level == QString(m_headerLevel))
            m_headerFound = true;

        outputed = inlineLexer.output(text);
        const QString outputedText = inlineLexer.output(text, true);
        const QString unescaped = Renderer::unescape(outputedText);

        return Renderer::heading(outputed, level, unescaped, m_headerFound);
    }

    if (type == QStringLiteral("code")) { // adding const with the Synthax Highlighting MR
        text = m_token[QStringLiteral("text")].toString();
        const QString lang = m_token[QStringLiteral("lang")].toString();

        const bool highlight = m_highlightEnabled && !lang.isEmpty();

        QString returnValue;
        if (m_sameCodeBlocks && highlight) { // Only the highlighted values are stored in here
            returnValue = m_previousHighlightedBlocks[m_currentBlockIndex];
            m_currentBlockIndex++;
        } else {
            returnValue = Renderer::code(text, lang, highlight);
            if (m_highlightEnabled && highlight) { // We want to store only the highlighted values
                m_previousHighlightedBlocks.append(returnValue);
            }
        }

        return returnValue;
    }

    if (type == QStringLiteral("table")) {
        body = emptyStr;
        int i, j;

        QString cell = emptyStr;
        const QStringList headersList = m_token[QStringLiteral("header")].toStringList();
        const QStringList alignList = m_token[QStringLiteral("align")].toStringList();
        for (i = 0; i < headersList.size(); i++) {
            QString currentHeader = headersList[i];
            outputed = inlineLexer.output(currentHeader);

            flags = {{QStringLiteral("header"), true}, {QStringLiteral("align"), alignList[i]}};
            cell += Renderer::tableCell(outputed, flags);
        }

        const QString header = Renderer::tableRow(cell);

        const QJsonArray cellsList = m_token[QStringLiteral("cells")].toJsonArray();
        for (i = 0; i < cellsList.size(); i++) {
            const QJsonArray row = cellsList[i].toArray();

            cell = emptyStr;
            for (j = 0; j < row.size(); j++) {
                QString currentCell = row[j].toString();
                outputed = inlineLexer.output(currentCell);

                flags = {{QStringLiteral("header"), false}, {QStringLiteral("align"), alignList[j]}};

                cell += Renderer::tableCell(outputed, flags);
            }
            body += Renderer::tableRow(cell);
        }

        return Renderer::table(header, body);
    }

    if (type == QStringLiteral("blockquote_start")) {
        body = emptyStr;

        while (getNextToken() && m_token[QStringLiteral("type")].toString() != QStringLiteral("blockquote_end")) {
            body += tok();
        }

        return Renderer::blockquote(body);
    }

    if (type == QStringLiteral("list_start")) {
        body = emptyStr;
        const bool ordered = m_token[QStringLiteral("ordered")].toBool();
        const QString start = m_token[QStringLiteral("start")].toString();

        while (getNextToken() && m_token[QStringLiteral("type")].toString() != QStringLiteral("list_end")) {
            body += tok();
        }

        return Renderer::list(body, ordered, start);
    }

    if (type == QStringLiteral("list_item_start")) {
        body = emptyStr;

        const bool hasTask = m_token[QStringLiteral("task")].toBool();
        if (hasTask) {
            body += Renderer::checkbox(m_token[QStringLiteral("checked")].toBool());
        }

        while (getNextToken() && m_token[QStringLiteral("type")].toString() != QStringLiteral("list_item_end")) {
            body += m_token[QStringLiteral("type")].toString() == QStringLiteral("text") ? parseText() : tok();
        }

        return Renderer::listItem(body, hasTask);
    }

    if (type == QStringLiteral("loose_item_start")) {
        body = emptyStr;

        while (getNextToken() && m_token[QStringLiteral("type")].toString() != QStringLiteral("list_item_end")) {
            body += tok();
        }

        return Renderer::listItem(body);
    }

    if (type == QStringLiteral("html")) {
        text = m_token[QStringLiteral("text")].toString();

        return Renderer::html(text);
    }

    if (type == QStringLiteral("paragraph")) {
        text = m_token[QStringLiteral("text")].toString();
        outputed = inlineLexer.output(text);

        return Renderer::paragraph(outputed);
    }

    if (type == QStringLiteral("text")) {
        const QString parsedText = parseText();

        return Renderer::paragraph(parsedText);
    }

    return {};
}

QString Parser::parseText()
{
    QString body = m_token[QStringLiteral("text")].toString();
    while (peekType() == QStringLiteral("text")) {
        getNextToken();
        const QString text = m_token[QStringLiteral("text")].toString();

        body += QString::fromStdString("\n") + text;
    }

    return inlineLexer.output(body);
}

bool Parser::getNextToken()
{
    m_token = (tokens.isEmpty()) ? QVariantMap{} : tokens.takeLast();

    return !m_token.isEmpty();
}

QString Parser::peekType() const
{
    return (!tokens.isEmpty()) ? tokens.last()[QStringLiteral("type")].toString() : QLatin1String();
}

// Syntax highlight
void Parser::setHighlightEnabled(const bool highlightEnabled)
{
    m_highlightEnabled = highlightEnabled;
}

bool Parser::highlightEnabled() const
{
    return m_highlightEnabled;
}

void Parser::addToNoteCodeBlocks(const QString &codeBlock)
{
    m_noteCodeBlocks.append(codeBlock);
}

void Parser::newHighlightStyle()
{
    m_newHighlightStyle = true;
}

// NoteMapper
void Parser::addToLinkedNoteInfos(const QStringList &infos)
{
    if ((!m_previousLinkedNotesInfos.remove(infos) && !m_linkedNotesInfos.contains(infos)) || infos.isEmpty()) {
        m_linkedNotesChanged = true;
    }
    m_linkedNotesInfos.insert(infos);
}

void Parser::addToNoteHeaders(const QString &header)
{
    if (!m_previousNoteHeaders.remove(header) && !m_noteHeaders.contains(header)) {
        m_noteHeadersChanged = true;
    }
    m_noteHeaders.insert(header);
}

void Parser::setNoteMapEnabled(const bool noteMapEnabled)
{
    m_noteMapEnabled = noteMapEnabled;
}

bool Parser::noteMapEnabled() const
{
    return m_noteMapEnabled;
}
