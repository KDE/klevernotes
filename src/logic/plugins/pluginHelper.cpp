/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "pluginHelper.h"

#include "kleverconfig.h"

PluginHelper::PluginHelper(MdEditor::Parser *parser)
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
    if (KleverConfig::pumlEnabled()) {
        m_pumlParserUtils->clearInfo();
    }
    if (KleverConfig::codeSynthaxHighlightEnabled()) {
        m_highlightParserUtils->clearInfo();
    }
}

void PluginHelper::clearPluginsPreviousInfo()
{
    if (KleverConfig::noteMapEnabled()) {
        m_mapperParserUtils->clearPreviousInfo();
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
