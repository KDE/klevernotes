/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QList>

#define MD4QT_QT_SUPPORT
#include "md4qt/doc.h"
#include "md4qt/parser.h"

using MDParagraphPtr = std::shared_ptr<MD::Paragraph<MD::QStringTrait>>;
using MDParsingOpts = MD::TextParsingOpts<MD::QStringTrait>;

using MDRawDataPtr = std::shared_ptr<MDParsingOpts::TextData>;

using MDItemSharedPointer = MD::Block<MD::QStringTrait>::ItemSharedPointer;
using MDItemWithOptsPtr = std::shared_ptr<MD::ItemWithOpts<MD::QStringTrait>>;
using MDTextItemPtr = std::shared_ptr<MD::Text<MD::QStringTrait>>;

namespace md4qtHelperFunc
{
/**
 * @brief Get a MD::ItemWithOpts shared_ptr from an ItemSharedPointer.
 *
 * @param item The ItemSharedPointer.
 * @return A MD::ItemWithOpts shared_ptr.
 */
MDItemWithOptsPtr getSharedItemWithOpts(const MDItemSharedPointer &item);

/**
 * @brief Get a MD::Text shared_ptr from a MD::ItemWithOpts shared_ptr.
 *
 * @param item The ItemWithOpts.
 * @return A MD::Text shared_ptr.
 */
MDTextItemPtr getSharedTextItem(const MDItemWithOptsPtr &item);

/**
 * @brief Get the index of the item possessing the given position inside the MD::Paragraph.
 *
 * @param startColumn The column position.
 * @param startLine The line position.
 * @param p The current MD::Paragraph being treated.
 * @return The index of the item possessing the given position.
 */
long long int paraIdxFromPos(const long long int startColumn, const long long int startLine, MDParagraphPtr p);

/**
 * @brief Get the index of the rawTextData possessing the given position.
 *
 * @param startColumn The column position.
 * @param startLine The line position.
 * @param po The current MD::TextParsingOpts used for `rawTextData`.
 * @return The index of the rawTextData possessing the given position.
 */
long long int rawIdxFromPos(const long long int startColumn, const long long int startLine, MDParsingOpts &po);

/**
 * @brief Get the index of the rawTextData corresponding to the given MD::ItemWithOpts.
 *
 * @param item The MD::ItemWithOpts.
 * @param po The current MD::TextParsingOpts used for `rawTextData`
 * @return The index of the rawTextData corresponding to the given MD::ItemWithOpts.
 */
long long int rawIdxFromItem(const MDItemWithOptsPtr item, MDParsingOpts &po);

/**
 * @brief Check if the given `value` position is between the given start and end delim positions.
 *
 * @param value The position which we want to check.
 * @param start The position of the starting delim.
 * @param end The position of the ending delim.
 * @param isCursor Whether the value being check is coming from the cursor.
 * @return True if the given value is between the delims, false otherwise.
 */
bool isBetweenDelims(const MD::WithPosition value, const MD::WithPosition start, const MD::WithPosition end, const bool isCursor = false);

/**
 * @class StartColumnOrder
 * @brief Class providing sorting abilities to `md4qt` items with MD::WithPosition traits.
 */
struct StartColumnOrder {
    template<typename T>
    bool operator()(const T &lhs, const T &rhs) const
    {
        return lhs.startLine() == rhs.startLine() ? lhs.startColumn() < rhs.startColumn() : lhs.startLine() < rhs.startLine();
    }
};
}
