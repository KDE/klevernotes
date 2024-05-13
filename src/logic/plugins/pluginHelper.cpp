/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "pluginHelper.h"

#include "kleverconfig.h"
#include <qcontainerfwd.h>

PluginHelper::PluginHelper(Parser *parser)
    : m_highlightParserUtils(new HighlightParserUtils)
    , m_pumlParserUtils(new PUMLParserUtils)
{
    m_mapperParserUtils = new NoteMapperParserUtils(parser);
}

void PluginHelper::clearPluginsInfo()
{
    if (KleverConfig::noteMapEnabled()) {
        m_mapperParserUtils->clearInfo();
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

void PluginHelper::postTokChanges()
{
    if (KleverConfig::noteMapEnabled()) {
        m_mapperParserUtils->postTok();
    }
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
