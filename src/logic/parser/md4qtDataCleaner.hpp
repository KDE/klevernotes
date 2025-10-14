/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "md4qtDataGetter.hpp"

namespace md4qtDataCleaner
{
const MD::TextPlugin dataCleanerId = static_cast<MD::TextPlugin>(1024);

/**
 * @brief Remove empty TextItem left after using plugins
 *
 * @param p The current MD::Paragraph being treated
 * @param po The current MD::TextParsingOpts used for `rawTextData`
 */
void removeEmpty(MDParagraphPtr p, MDParsingOpts &po);

/**
 * @brief Function exposed to md4qt::Parser::addTextPlugin
 *
 * @param p The current MD::Paragraph being treated
 * @param po The current MD::TextParsingOpts used for `rawTextData`
 * @param options Options of this plugins
 */
void dataCleaningFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options);
}
