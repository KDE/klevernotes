/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "md4qtDataCleaner.hpp"
#include "logic/parser/md4qtDataGetter.hpp"

namespace md4qtDataCleaner
{
void removeEmpty(MDParagraphPtr p, MDParsingOpts &po)
{
    long long int rawIdx = 0;
    long long int paraIdx = 0;
    while (paraIdx < p->items().length()) {
        if (p->getItemAt(paraIdx)->type() == MD::ItemType::Text) {
            if (po.rawTextData[rawIdx].str.trimmed().isEmpty()) {
                po.rawTextData.erase(po.rawTextData.cbegin() + rawIdx);
                p->removeItemAt(paraIdx);
                continue;
            }
            ++rawIdx;
        }
        ++paraIdx;
    }
}

std::pair<long long, long long> getAbsoluteColumns(MDItemWithOptsPtr item)
{
    long long int absoluteStart;
    long long int absoluteEnd;
    switch (item->type()) {
    case MD::ItemType::Math: {
        const auto mathItem = std::static_pointer_cast<MD::Math<MD::QStringTrait>>(item);

        const long long int delimStart = mathItem->startDelim().startColumn();
        absoluteStart = delimStart != -1 ? delimStart : mathItem->startColumn();

        const long long int delimEnd = mathItem->endDelim().endColumn();
        absoluteEnd = delimEnd != -1 ? delimEnd : mathItem->endColumn();
        break;
    }
    case MD::ItemType::Code: {
        const auto codeItem = std::static_pointer_cast<MD::Code<MD::QStringTrait>>(item);
        const long long int delimStart = codeItem->startDelim().startColumn();
        absoluteStart = delimStart != -1 ? delimStart : codeItem->startColumn();

        const long long int delimEnd = codeItem->endDelim().endColumn();
        absoluteEnd = delimEnd != -1 ? delimEnd : codeItem->endColumn();
        break;
    }
    default: {
        absoluteStart = item->startColumn();
        absoluteEnd = item->endColumn();
    } break;
    }

    absoluteStart = item->openStyles().isEmpty() ? absoluteStart : item->openStyles().first().startColumn();
    absoluteEnd = item->closeStyles().isEmpty() ? absoluteEnd : item->closeStyles().last().endColumn();

    return std::make_pair(absoluteStart, absoluteEnd);
}

void addSpace(MDItemWithOptsPtr item, const long long int paraIdx, MDParagraphPtr p, MDParsingOpts &po)
{
    MDTextItemPtr textItem = md4qtHelperFunc::getSharedTextItem(item);
    const auto itemLocalPos = MD::localPosFromVirgin(po.fr, item->startColumn(), item->startLine());
    const long long int rawIdx = md4qtHelperFunc::rawIdxFromPos(itemLocalPos.first, itemLocalPos.second, po);
    const auto [absoluteStart, absoluteEnd] = getAbsoluteColumns(item);

    // Space before
    bool hasSpaceBefore;
    if (paraIdx == 0) {
        hasSpaceBefore = true;
    } else {
        const auto previousItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx - 1));
        const auto [previousAbsoluteStart, previousAbsoluteEnd] = getAbsoluteColumns(previousItem);

        hasSpaceBefore = previousItem->startLine() != item->startLine();
        if (!hasSpaceBefore) {
            hasSpaceBefore = previousAbsoluteEnd + 1 < absoluteStart;
        }
        if (!hasSpaceBefore) {
            hasSpaceBefore = po.rawTextData[rawIdx].str.front().isSpace();
        }
        if (!hasSpaceBefore && previousItem->type() == MD::ItemType::Text) {
            hasSpaceBefore = po.rawTextData[rawIdx - 1].str.back().isSpace();
        }
    }
    textItem->setSpaceBefore(hasSpaceBefore);
    po.rawTextData[rawIdx].spaceBefore = hasSpaceBefore;

    // Space after
    bool hasSpaceAfter;
    if (paraIdx == p->items().length() - 1) {
        hasSpaceAfter = true;
    } else {
        const auto nextItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx + 1));

        const auto [nextAbsoluteStart, nextAbsoluteEnd] = getAbsoluteColumns(nextItem);

        hasSpaceAfter = item->startLine() != nextItem->startLine();
        if (!hasSpaceAfter) {
            hasSpaceAfter = absoluteEnd + 1 < nextAbsoluteStart;
        }
        if (!hasSpaceAfter) {
            hasSpaceAfter = po.rawTextData[rawIdx].str.back().isSpace();
        }
        if (!hasSpaceAfter && nextItem->type() == MD::ItemType::Text) {
            hasSpaceAfter = po.rawTextData[rawIdx + 1].str.front().isSpace();
        }
    }
    textItem->setSpaceAfter(hasSpaceAfter);
    po.rawTextData[rawIdx].spaceAfter = hasSpaceAfter;
}

void setSpacesBack(MDParagraphPtr p, MDParsingOpts &po)
{
    // Not the most optimal but this way we don't miss anything
    for (long long int i = 0; i < p->items().length(); ++i) {
        const MDItemWithOptsPtr item = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(i));
        if (item->type() == MD::ItemType::Text) {
            addSpace(item, i, p, po);
        }
    }
}

void dataCleaningFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options)
{
    Q_UNUSED(options);
    setSpacesBack(p, po);

    removeEmpty(p, po);
}
}
