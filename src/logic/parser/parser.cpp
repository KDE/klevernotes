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

PluginHelper *Parser::getPluginHelper()
{
    return pluginHelper;
}

void Parser::setHeaderInfo(const QStringList &headerInfo)
{
    pluginHelper->setHeaderInfo(headerInfo);
}

QString Parser::headerLevel() const
{
    return pluginHelper->headerLevel();
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
    pluginHelper->clearPluginsInfo();

    pluginHelper->setNoteMapperInfo(notePath);
}

QString Parser::getNotePath() const
{
    return m_notePath;
}

QString Parser::parse(QString src)
{
    pluginHelper->clearPluginsInfo();

    blockLexer.lex(src);

    pluginHelper->preTokChanges();

    std::reverse(tokens.begin(), tokens.end());

    QString out;
    while (getNextToken()) {
        out += tok();
    }

    pluginHelper->postTokChanges();

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
        pluginHelper->checkHeaderFound(text, level);

        outputed = inlineLexer.output(text);
        const QString outputedText = inlineLexer.output(text, true);
        const QString unescaped = Renderer::unescape(outputedText);

        return Renderer::heading(outputed, level, unescaped, pluginHelper->headerFound());
    }

    if (type == QStringLiteral("code")) { // adding const with the Synthax Highlighting MR
        text = m_token[QStringLiteral("text")].toString();
        const QString lang = m_token[QStringLiteral("lang")].toString().trimmed();

        return pluginHelper->blockCodePlugins(lang, text);
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
void Parser::newHighlightStyle()
{
    pluginHelper->newHighlightStyle();
}

// PUML
void Parser::pumlDarkChanged()
{
    pluginHelper->pumlDarkChanged();
}
