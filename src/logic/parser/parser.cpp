/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#include "parser.h"

#include "kleverconfig.h"
#include <QJsonArray>

#include "renderer.h"

using namespace std;

Parser::Parser(QObject *parent)
    : QObject(parent)
{
}

QPair<QString, bool> Parser::sanitizePath(QString path)
{
    QStringList parts = path.split("/");

    bool leadingSlashRemnant = false;
    for (int i = 0; i < parts.count(); i++) {
        QString part = parts[i].trimmed();
        if (part.isEmpty()) {
            if (i == 0) {
                leadingSlashRemnant = true;
            } else { // The path is not correctly formed
                return qMakePair(path, false);
            }
        }
        parts[i] = part;
    }

    if (leadingSlashRemnant)
        parts.removeAt(0);

    if (parts[0] == KleverConfig::defaultCategoryDisplayNameValue())
        parts[0] = QStringLiteral(".BaseCategory");

    switch (parts.count()) {
    case 1: // Note name only
        path = m_groupPath + parts[0];
        break;
    case 2:
        if (parts[0] == QStringLiteral(".")) { // Note name only
            path = m_groupPath + parts[1];
        } else { // Note inside category
            path = QStringLiteral("/") + parts[0] + QStringLiteral("/.BaseGroup/") + parts[1];
        }
        break;
    case 3: // 'Full' path
        path = QStringLiteral("/") + parts.join("/");
        break;
    default: // Not a note path
        return qMakePair(path, false);
    }

    return qMakePair(path, true);
}

void Parser::setHeaderInfo(const QVariantList &headerInfo)
{
    m_header = headerInfo[0].toString();
    m_headerLevel = m_header.isEmpty() ? 0 : headerInfo[1].toInt();
}

int Parser::headerLevel()
{
    return m_headerLevel;
};

void Parser::setNotePath(QString &notePath)
{
    if (m_notePath != notePath)
        m_notePath = notePath;

    m_mapperNotePath = notePath.remove(KleverConfig::storagePath()).chopped(1);
    QString groupPath(m_mapperNotePath);
    groupPath.chop(groupPath.size() - groupPath.lastIndexOf("/"));
    if (m_groupPath != groupPath) {
        m_groupPath = groupPath + "/";
        m_categPath = groupPath.chopped(groupPath.size() - groupPath.lastIndexOf("/") - 1);
    }
}

QString Parser::getNotePath()
{
    return m_notePath;
}

QString Parser::parse(QString src)
{
    links.clear();
    tokens.clear();
    m_linkedNoteInfos.clear();
    m_noteHeaders.clear();

    blockLexer.lex(src);

    std::reverse(tokens.begin(), tokens.end());

    QString out;
    while (getNextToken()) {
        out += tok();
    }

    // We try to not spam with signals
    if (m_linkedNoteInfos != m_previousLinkedNoteInfos) {
        m_previousLinkedNoteInfos = m_linkedNoteInfos;
        Q_EMIT newLinkedNotesPaths(m_linkedNoteInfos);
    }
    if (m_noteHeaders != m_previousNoteHeaders) {
        m_previousNoteHeaders = m_noteHeaders;
        Q_EMIT noteHeadersSent(m_mapperNotePath, m_noteHeaders);
    }

    return out;
}

QString Parser::tok()
{
    QString type = m_token["type"].toString();
    QString outputed, text, body;
    QVariantMap flags;

    if (type == "space") {
        return "";
    }

    if (type == "hr") {
        return Renderer::hr();
    }

    if (type == "heading") {
        text = m_token["text"].toString();

        int level = m_token["depth"].toInt();
        bool scrollTo = false;
        if (text == m_header && level == m_headerLevel)
            scrollTo = true;

        outputed = inlineLexer.output(text);
        QString outputedText = inlineLexer.output(text, true);
        QString unescaped = Renderer::unescape(outputedText);

        return Renderer::heading(outputed, level, unescaped, scrollTo);
    }

    if (type == "code") {
        text = m_token["text"].toString();
        QString lang = m_token["lang"].toString();

        return Renderer::code(text, lang);
    }

    if (type == "table") {
        QString header = "";
        body = "";
        int i, j;

        QString cell = "";
        QStringList headersList = m_token["header"].toStringList();
        QStringList alignList = m_token["align"].toStringList();
        for (i = 0; i < headersList.size(); i++) {
            QString currentHeader = headersList[i];
            outputed = inlineLexer.output(currentHeader);

            flags = {{"header", true}, {"align", alignList[i]}};
            cell += Renderer::tableCell(outputed, flags);
        }
        header += Renderer::tableRow(cell);

        QJsonArray cellsList = m_token["cells"].toJsonArray();
        for (i = 0; i < cellsList.size(); i++) {
            QJsonArray row = cellsList[i].toArray();

            cell = "";
            for (j = 0; j < row.size(); j++) {
                QString currentCell = row[j].toString();
                outputed = inlineLexer.output(currentCell);

                flags = {{"header", false}, {"align", alignList[j]}};

                cell += Renderer::tableCell(outputed, flags);
            }
            body += Renderer::tableRow(cell);
        }

        return Renderer::table(header, body);
    }

    if (type == "blockquote_start") {
        body = "";

        while (getNextToken() && m_token["type"].toString() != "blockquote_end") {
            body += tok();
        }

        return Renderer::blockquote(body);
    }

    if (type == "list_start") {
        body = "";
        bool ordered = m_token["ordered"].toBool();
        QString start = m_token["start"].toString();

        while (getNextToken() && m_token["type"].toString() != "list_end") {
            body += tok();
        }

        return Renderer::list(body, ordered, start);
    }

    if (type == "list_item_start") {
        body = "";

        bool hasTask = m_token["task"].toBool();
        if (hasTask) {
            body += Renderer::checkbox(m_token["checked"].toBool());
        }

        while (getNextToken() && m_token["type"].toString() != "list_item_end") {
            body += m_token["type"].toString() == "text" ? parseText() : tok();
        }

        return Renderer::listItem(body, hasTask);
    }

    if (type == "loose_item_start") {
        body = "";

        while (getNextToken() && m_token["type"].toString() != "list_item_end") {
            body += tok();
        }

        return Renderer::listItem(body);
    }

    if (type == "html") {
        text = m_token["text"].toString();

        return Renderer::html(text);
    }

    if (type == "paragraph") {
        text = m_token["text"].toString();
        outputed = inlineLexer.output(text);

        return Renderer::paragraph(outputed);
    }

    if (type == "text") {
        QString parsedText = parseText();

        return Renderer::paragraph(parsedText);
    }

    return "";
}

QString Parser::parseText()
{
    QString body = m_token["text"].toString();

    while (peekType() == "text") {
        getNextToken();
        QString text = m_token["text"].toString();

        body += QString::fromStdString("\n") + text;
    }

    return inlineLexer.output(body);
}

bool Parser::getNextToken()
{
    m_token = (tokens.isEmpty()) ? QVariantMap{} : tokens.takeLast();

    return !m_token.isEmpty();
}

QString Parser::peekType()
{
    return (!tokens.isEmpty()) ? tokens.last()["type"].toString() : "";
}

void Parser::addToLinkedNoteInfos(const QStringList &infos)
{
    m_linkedNoteInfos.append(infos);
}

void Parser::addToNoteHeaders(const QString &header)
{
    m_noteHeaders.append(header);
}
