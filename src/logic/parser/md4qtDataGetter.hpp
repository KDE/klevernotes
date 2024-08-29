/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QList>

#define MD4QT_QT_SUPPORT
#include "md4qt/doc.hpp"
#include "md4qt/parser.hpp"

using MDParagraphPtr = std::shared_ptr<MD::Paragraph<MD::QStringTrait>>;
using MDParsingOpts = MD::TextParsingOpts<MD::QStringTrait>;

using MDRawDataPtr = std::shared_ptr<MDParsingOpts::TextData>;

using MDItemSharedPointer = MD::Block<MD::QStringTrait>::ItemSharedPointer;
using MDItemWithOptsPtr = std::shared_ptr<MD::ItemWithOpts<MD::QStringTrait>>;
using MDTextItemPtr = std::shared_ptr<MD::Text<MD::QStringTrait>>;

namespace md4qtHelperFunc
{
MDItemWithOptsPtr getSharedItemWithOpts(const MDItemSharedPointer &item);

MDTextItemPtr getSharedTextItem(const MDItemWithOptsPtr &item);

long long int paraIdxFromPos(const long long int startColumn, const long long int startLine, MDParagraphPtr p);

long long int rawIdxFromPos(const long long int startColumn, const long long int startLine, MDParsingOpts &po);

long long int rawIdxFromItem(const MDItemWithOptsPtr item, MDParsingOpts &po);

bool isBetweenDelims(const MD::WithPosition value, const MD::WithPosition start, const MD::WithPosition end);

struct StartColumnOrder {
    template<typename T>
    bool operator()(const T &lhs, const T &rhs) const
    {
        return lhs.startLine() == rhs.startLine() ? lhs.startColumn() < rhs.startColumn() : lhs.startLine() < rhs.startLine();
    }
};
}
