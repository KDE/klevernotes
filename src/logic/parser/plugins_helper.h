/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2026 Igor Mironchik <igor.mironchik@gmail.com>
*/

#pragma once

// md4qt include.
#include <md4qt/src/asterisk_emphasis_parser.h>
#include <md4qt/src/autolink_parser.h>
#include <md4qt/src/emphasis_parser.h>
#include <md4qt/src/gfm_autolink_parser.h>
#include <md4qt/src/hard_line_break_parser.h>
#include <md4qt/src/inline_code_parser.h>
#include <md4qt/src/inline_html_parser.h>
#include <md4qt/src/inline_math_parser.h>
#include <md4qt/src/link_image_parser.h>
#include <md4qt/src/parser.h>
#include <md4qt/src/strikethrough_emphasis_parser.h>
#include <md4qt/src/underline_emphasis_parser.h>

template<class... Plugins>
void addInlinePlugins(MD::Parser::InlineParsers &inlineParsers)
{
    (MD::Parser::appendInlineParser<Plugins>(inlineParsers), ...);
}

template<class... Plugins>
MD::Parser::InlineParsers setInlineParsers()
{
    MD::Parser::InlineParsers inlineParsers;

    MD::Parser::appendInlineParser<MD::InlineCodeParser>(inlineParsers);

    auto linkParser = MD::Parser::appendInlineParser<MD::LinkImageParser>(inlineParsers);

    MD::Parser::appendInlineParser<MD::AutolinkParser>(inlineParsers);
    MD::Parser::appendInlineParser<MD::InlineHtmlParser>(inlineParsers);
    MD::Parser::appendInlineParser<MD::InlineMathParser>(inlineParsers);
    MD::Parser::appendInlineParser<MD::AsteriskEmphasisParser>(inlineParsers);
    MD::Parser::appendInlineParser<MD::UnderlineEmphasisParser>(inlineParsers);
    MD::Parser::appendInlineParser<MD::StrikethroughEmphasisParser>(inlineParsers);

    addInlinePlugins<Plugins...>(inlineParsers);

    inlineParsers.append(QSharedPointer<MD::GfmAutolinkParser>::create(linkParser));
    MD::Parser::appendInlineParser<MD::HardLineBreakParser>(inlineParsers);

    return inlineParsers;
}