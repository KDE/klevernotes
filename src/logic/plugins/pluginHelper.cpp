/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "pluginHelper.h"

#include "kleverconfig.h"

PluginHelper::PluginHelper(Parser *parser)
    : m_highlightParserUtils(new HighlightParserUtils)
    , m_pumlParserUtils(new PUMLParserUtils)
{
    m_mapperParserUtils = new NoteMapperParserUtils(parser);
}

void PluginHelper::clearPluginsInfo()
{
    // Syntax highlight
    m_highlightParserUtils->clearInfo();
    // NoteMapper
    m_mapperParserUtils->clearInfo();
    // PUML
    m_pumlParserUtils->clearInfo();
}

void PluginHelper::clearPluginsPreviousInfo()
{
    // Syntax highlight
    m_highlightParserUtils->clearPreviousInfo();
    // NoteMapper
    m_mapperParserUtils->clearPreviousInfo();
    // PUML
    m_pumlParserUtils->clearPreviousInfo();
}

void PluginHelper::preTokChanges()
{
    // Syntax highlight
    m_highlightParserUtils->preTok();
    // PUML
    m_pumlParserUtils->preTok();
}

void PluginHelper::postTokChanges()
{
    // NoteMapper
    m_mapperParserUtils->postTok();
}

QString PluginHelper::blockCodePlugins(const QString &lang, const QString &_text)
{
    static const QString pumlStr = QStringLiteral("puml");
    static const QString plantUMLStr = QStringLiteral("plantuml");

    QString returnValue;
    if (KleverConfig::pumlEnabled() && (lang.toLower() == pumlStr || lang.toLower() == plantUMLStr)) {
        returnValue = m_pumlParserUtils->renderCode(_text);
    } else {
        const bool highlightEnabled = KleverConfig::codeSynthaxHighlightEnabled();
        const bool highlight = highlightEnabled && !lang.isEmpty();

        returnValue = m_highlightParserUtils->renderCode(highlight, _text, lang);
    }

    return returnValue;
}

// NoteMapper
NoteMapperParserUtils *PluginHelper::getMapperParserUtils() const
{
    return m_mapperParserUtils;
}

// Syntax highlight
HighlightParserUtils *PluginHelper::getHighlightParserUtils() const
{
    return m_highlightParserUtils;
}

// PUML
PUMLParserUtils *PluginHelper::getPUMLParserUtils() const
{
    return m_pumlParserUtils;
}
