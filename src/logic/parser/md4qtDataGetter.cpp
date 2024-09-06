/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "md4qtDataGetter.hpp"

namespace md4qtHelperFunc
{
MDItemWithOptsPtr getSharedItemWithOpts(const MDItemSharedPointer &item)
{
    return std::static_pointer_cast<MD::ItemWithOpts<MD::QStringTrait>>(item);
}

MDTextItemPtr getSharedTextItem(const MDItemWithOptsPtr &item)
{
    return std::static_pointer_cast<MD::Text<MD::QStringTrait>>(item);
}

long long int paraIdxFromPos(const long long int startColumn, const long long int startLine, MDParagraphPtr p)
{
    const auto &items = p->items();
    const auto nextItemIt = std::find_if(items.cbegin(), items.cend(), [startColumn, startLine](const auto &d) {
        return (startLine == d->startLine() && startColumn < d->startColumn()) || startLine < d->startLine();
    });

    return std::distance(items.cbegin(), nextItemIt) - 1;
}

long long int rawIdxFromPos(const long long int startColumn, const long long int startLine, MDParsingOpts &po)
{
    const auto &rawDatas = po.rawTextData;
    const auto nextDataIt = std::find_if(rawDatas.cbegin(), rawDatas.cend(), [startColumn, startLine](const auto &d) {
        return (startLine == d.line && startColumn < d.pos) || startLine < d.line;
    });

    return std::distance(rawDatas.cbegin(), nextDataIt) - 1;
}

long long int rawIdxFromItem(const MDItemWithOptsPtr item, MDParsingOpts &po)
{
    if (item->type() != MD::ItemType::Text) {
        return -1;
    }
    const auto localPos = MD::localPosFromVirgin(po.fr, item->startColumn(), item->startLine());
    return rawIdxFromPos(localPos.first, localPos.second, po);
}

bool isBetweenDelims(const MD::WithPosition value, const MD::WithPosition start, const MD::WithPosition end, const bool isCursor)
{
    // Note: since we're dealing with delims, it's fine to assume that, within a delim: startLine() == endLine()
    const bool betweenLine = start.startLine() <= value.startLine() && value.startLine() <= end.startLine();
    if (betweenLine) {
        // To be less strict with cursor
        const long long veryStart = isCursor ? start.startColumn() - 1 : start.startColumn();
        const long long veryEnd = isCursor ? end.endColumn() + 1 : end.endColumn();

        const bool afterStart = start.startLine() < value.startLine() ? true : veryStart <= value.startColumn();
        const bool beforeEnd = value.startLine() < end.startLine() ? true : value.endColumn() <= veryEnd;

        return afterStart && beforeEnd;
    }

    return false;
}
} // !md4qtHelperFunc
