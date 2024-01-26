/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "kleverconfig.h"
#include <QObject>
#include <QSet>

#include "logic/plugins/noteMapper/noteMapperParserUtils.h"
#include "logic/plugins/syntaxHighlight/highlightParserUtils.h"

class PluginHelper
{
public:
    explicit PluginHelper(Parser *parser);
    void clearPluginsInfo();
    void clearPluginsPreviousInfo();
    void preTokChanges();
    void postTokChanges();

    QString blockCodePlugins(const QString &lang, const QString &_text);

    // NoteMapper
    NoteMapperParserUtils *getMapperParserUtils() const;

    // Syntax highlight
    HighlightParserUtils *getHighlightParserUtils() const;

    // PUML
    void addToNotePUMLBlock(const QString &pumlBlock);
    void pumlDarkChanged();

private:
    // Synthax highlight
    HighlightParserUtils *m_highlightParserUtils = nullptr;

    // NoteMapper
    NoteMapperParserUtils *m_mapperParserUtils = nullptr;

    // Emoji support
    QString m_emojiTone = KleverConfig::emojiTone();

    // PUML
    bool m_pumlDarkChanged = true;
    bool m_samePUMLBlocks = false;
    int m_currentPUMLBlockIndex = 0;
    QStringList m_notePUMLBlocks;
    QStringList m_previousNotePUMLBlocks;
    QStringList m_previousPUMLDiag;
};
