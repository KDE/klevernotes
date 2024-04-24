/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#pragma once

#include <QSet>

#include "../plugins/pluginHelper.h"
#include "blockLexer.h"
#include "inlineLexer.h"

class EditorHandler;
class Parser
{
public:
    Parser(EditorHandler *editorHandler = nullptr);

    EditorHandler *getEditorHandler() const
    {
        return m_editorHandler;
    }

    QString parse();
    void lex(QString src);

    QString getNotePath() const;
    void setNotePath(const QString &notePath);

    void addToken(const QVariantMap &token);
    bool tagExists(const QString &tag);
    void addLink(const QString &tag, const QMap<QString, QString> &link);
    QMap<QString, QString> getLink(const QString &tag);
    void addHighlightToken(const std::tuple<QString, int, int> &token);

    PluginHelper *getPluginHelper() const;

    // NoteMapper
    void setHeaderInfo(const QStringList &headerInfo);
    QString headerLevel() const;
    // Syntax highlight
    void newHighlightStyle();
    // PUML
    void pumlDarkChanged();

private:
    EditorHandler *m_editorHandler = nullptr;

    QString tok();
    QString parseText();
    QString peekType() const;
    bool getNextToken();

    BlockLexer blockLexer = BlockLexer(this);
    InlineLexer inlineLexer = InlineLexer(this);
    PluginHelper *pluginHelper = new PluginHelper(this);

    QString m_notePath;
    QVariantMap m_token;
    QVector<QVariantMap> m_tokens;
    QMap<QString, QMap<QString, QString>> m_links;
};
