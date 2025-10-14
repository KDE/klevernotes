/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "logic/parser/md4qtDataGetter.hpp"

namespace NoteLinkingPlugin
{
/**
 * @brief Provide a simple way to execute the whole parsing for the note linking.
 *
 * @param p The current MD::Paragraph being treated
 * @param po The current MD::TextParsingOpts used for `rawTextData`
 * @param rawIdx The index of the current rawTextData item being treated.
 */
inline long long int processNoteLinking(MDParagraphPtr p, MDParsingOpts &po, long long int rawIdx);

/**
 * @brief Function exposed to md4qt::Parser::addTextPlugin
 *
 * @param p The current MD::Paragraph being treated
 * @param po The current MD::TextParsingOpts used for `rawTextData`
 * @param options Options of this plugins
 */
void noteLinkingHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options);
}
