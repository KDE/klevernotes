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
    if (KleverConfig::codeSynthaxHighlightEnabled()) {
        m_highlightParserUtils->clearInfo();
    }
    if (KleverConfig::noteMapEnabled()) {
        m_mapperParserUtils->clearInfo();
    }
    if (KleverConfig::pumlEnabled()) {
        m_pumlParserUtils->clearInfo();
    }
}

void PluginHelper::clearPluginsPreviousInfo()
{
    if (KleverConfig::codeSynthaxHighlightEnabled()) {
        m_highlightParserUtils->clearPreviousInfo();
    }
    if (KleverConfig::noteMapEnabled()) {
        m_mapperParserUtils->clearPreviousInfo();
    }
    if (KleverConfig::pumlEnabled()) {
        m_pumlParserUtils->clearPreviousInfo();
    }
}

void PluginHelper::preTokChanges()
{
    // Syntax highlight
    if (KleverConfig::codeSynthaxHighlightEnabled()) {
        m_highlightParserUtils->preTok();
    }
    if (KleverConfig::pumlEnabled()) {
        m_pumlParserUtils->preTok();
    }
}

void PluginHelper::postTokChanges()
{
    if (KleverConfig::noteMapEnabled()) {
        m_mapperParserUtils->postTok();
    }
}

QString PluginHelper::blockCodePlugins(const QString &lang, const QString &_text)
{
    static const QString pumlStr = QStringLiteral("puml");
    static const QString plantUMLStr = QStringLiteral("plantuml");

    QString returnValue;
    if (KleverConfig::pumlEnabled() && (lang.toLower() == pumlStr || lang.toLower() == plantUMLStr)) {
        returnValue = m_pumlParserUtils->renderCode(_text, KleverConfig::pumlDark());
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
