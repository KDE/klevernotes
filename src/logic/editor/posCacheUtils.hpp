/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

// md-editor include.
#define MD4QT_QT_SUPPORT
#include "logic/parser/md4qt/doc.h"
#include "logic/parser/md4qt/traits.h"

using ItemSharedPointer = std::shared_ptr<MD::Item<MD::QStringTrait>>;
using SharedItems = typename MD::QStringTrait::template Vector<ItemSharedPointer>;
using Items = typename MD::QStringTrait::template Vector<MD::Item<MD::QStringTrait> *>;

namespace posCacheUtils
{

/**
 * @class DelimsInfo
 * @brief Structure holding specific information about styles delims.
 */
struct DelimsInfo {
    int headingLevel = 0;
    int delimType = MD::TextOption::TextWithoutFormat;
    MD::WithPosition opening = {};
    MD::WithPosition closing = {};
};

// Use negative values since positive are for style delims
enum BlockDelimTypes {
    Heading1 = -10,
    Heading2,
    Heading3,
    Heading4,
    Heading5,
    Heading6,
    CodeBlock,
    BlockQuote,
    OrderedList,
    UnorderedList,
};

/**
 * @brief Check if both DelimsInfo are equals based on there opening and closing.
 *
 * @param d1 A DelimsInfo.
 * @param d2 Another DelimsInfo.
 * @return True if both are equals, false otherwise.
 */
bool operator==(const DelimsInfo &d1, const DelimsInfo &d2);

/**
 * @brief Add all the delims found inside the items that surround the `selectStartPos` and the `selectEndPos` to the `delims` list.
 *
 * @param delims A list of DelimsInfo where all the new DelimsInfo will be added.
 * @param items A vector of items in which we will look for delims.
 * @param pos The current cursor position.
 * @param selectStartPos The position where the selection starts. If there's no selection, this is equal to `pos`.
 * @param selectEndPos The position where the selection ends. If there's no selection, this is equal to `pos`.
 */
void addDelimsFromItems(QList<DelimsInfo> &delims,
                        const Items &items,
                        const MD::WithPosition &pos,
                        const MD::WithPosition &selectStartPos = {},
                        const MD::WithPosition &selectEndPos = {});
}
