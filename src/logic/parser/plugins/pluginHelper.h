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

/**
 * @class PluginHelper
 * @brief Helper class to interact with some plugins.
 */
class PluginHelper
{
public:
    explicit PluginHelper(MdEditor::EditorHandler *editorHandler);

    /**
     * @brief Clear all the cached plugin info.
     */
    void clearPluginsInfo();

    /**
     * @brief Clear all the previous cache info (used by NoteMapper).
     */
    void clearPluginsPreviousInfo();

    /**
     * @brief Apply changes once the tokenization is done (used by NoteMapper).
     */
    void postTokChanges();

    // NoteMapper
    /**
     * @brief Get the NoteMapperParserUtils.
     *
     * @return The NoteMapperParserUtils instance.
     */
    NoteMapperParserUtils *mapperParserUtils() const;

    // Syntax highlight
    /**
     * @brief Get the HighlightParserUtils.
     *
     * @return The HighlightParserUtils instance.
     */
    HighlightParserUtils *highlightParserUtils() const;

    // PUML
    /**
     * @brief Get the PUMLParserUtils.
     *
     * @return The PUMLParserUtils instance.
     */
    PUMLParserUtils *pumlParserUtils() const;

private:
    // Synthax highlight
    HighlightParserUtils *m_highlightParserUtils = nullptr;

    // NoteMapper
    NoteMapperParserUtils *m_mapperParserUtils = nullptr;

    // PUML
    PUMLParserUtils *m_pumlParserUtils = nullptr;
};
