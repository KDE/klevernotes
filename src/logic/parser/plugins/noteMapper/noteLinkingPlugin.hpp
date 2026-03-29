/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

// md4qt include.
#include <md4qt/src/inline_parser.h>

namespace NoteLinkingPlugin
{

class NoteLinkingParser : public MD::InlineParser
{
public:
    NoteLinkingParser() = default;
    ~NoteLinkingParser() override = default;

    bool check(MD::Line &line,
               MD::ParagraphStream &stream,
               MD::InlineContext &ctx,
               QSharedPointer<MD::Document> doc,
               const QString &path,
               const QString &fileName,
               QStringList &linksToParse,
               MD::Parser &parser,
               const MD::ReverseSolidusHandler &rs) override;

    QString startDelimiterSymbols() const override;
};

}
