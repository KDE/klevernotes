/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QObject>
#include <QSet>

#include "noteMapper/noteMapperParserUtils.h"
#include "puml/pumlParserUtils.h"
#include "syntaxHighlight/highlightParserUtils.h"

class PluginHelper
{
public:
    explicit PluginHelper(MdEditor::EditorHandler *editorHandler);
    void clearPluginsInfo();
    void clearPluginsPreviousInfo();
    void postTokChanges();

    // NoteMapper
    NoteMapperParserUtils *mapperParserUtils() const;

    // Syntax highlight
    HighlightParserUtils *highlightParserUtils() const;

    // PUML
    PUMLParserUtils *pumlParserUtils() const;

private:
    // Synthax highlight
    HighlightParserUtils *m_highlightParserUtils = nullptr;

    // NoteMapper
    NoteMapperParserUtils *m_mapperParserUtils = nullptr;

    // PUML
    PUMLParserUtils *m_pumlParserUtils = nullptr;
};
