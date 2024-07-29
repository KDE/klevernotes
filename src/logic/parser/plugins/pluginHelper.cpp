/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "pluginHelper.h"

#include "kleverconfig.h"

PluginHelper::PluginHelper(MdEditor::EditorHandler *editorHandler)
    : m_highlightParserUtils(new HighlightParserUtils)
    , m_pumlParserUtils(new PUMLParserUtils)
{
    m_mapperParserUtils = new NoteMapperParserUtils(editorHandler);
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
NoteMapperParserUtils *PluginHelper::mapperParserUtils() const
{
    return m_mapperParserUtils;
}

// Syntax highlight
HighlightParserUtils *PluginHelper::highlightParserUtils() const
{
    return m_highlightParserUtils;
}

// PUML
PUMLParserUtils *PluginHelper::pumlParserUtils() const
{
    return m_pumlParserUtils;
}
