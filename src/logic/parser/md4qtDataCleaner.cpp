/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "md4qtDataCleaner.hpp"

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

void addSpace(MDTextItemPtr item, const long long int paraIdx, const bool spaceBefore, MDParagraphPtr p, MDParsingOpts &po)
{
    const auto itemLocalPos = MD::localPosFromVirgin(po.fr, item->startColumn(), item->startLine());
    const long long int rawIdx = md4qtHelperFunc::rawIdxFromPos(itemLocalPos.first, itemLocalPos.second, po);

    const auto lineInfo = po.fr.data.at(itemLocalPos.second);
    const QString lineStr = lineInfo.first.asString();
    static const QString space = QStringLiteral(" ");

    MDTextItemPtr textItem = md4qtHelperFunc::getSharedTextItem(item);
    if (spaceBefore) {
        const long long int currentTrailingPos = item->startColumn();
        const long long int absoluteTrailingPos = item->openStyles().isEmpty() ? currentTrailingPos : item->openStyles().first().startColumn();

        bool hasSpaceBefore;
        if (paraIdx == 0 || absoluteTrailingPos == 0) {
            hasSpaceBefore = true;
        } else {
            const auto previousItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx - 1));
            const long long int previousTailingPos = previousItem->endColumn();

            const long long int spaceIdx = lineStr.indexOf(space, previousTailingPos - 1);
            hasSpaceBefore = spaceIdx != -1 && spaceIdx <= currentTrailingPos;
        }
        textItem->setSpaceBefore(hasSpaceBefore);
        po.rawTextData[rawIdx].spaceBefore = hasSpaceBefore;
    } else {
        const long long int currentTailingPos = item->endColumn();
        const long long int absoluteTailingPos = item->closeStyles().isEmpty() ? currentTailingPos : item->closeStyles().last().endColumn();

        bool hasSpaceAfter;
        if (paraIdx == p->items().length() - 1 || absoluteTailingPos == lineStr.length() - 1) {
            hasSpaceAfter = true;
        } else {
            const auto nextItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx + 1));
            const long long int nextTrailingPos = nextItem->startColumn();

            const long long int spaceIdx = lineStr.indexOf(space, currentTailingPos - 1);
            hasSpaceAfter = spaceIdx != -1 && spaceIdx <= nextTrailingPos;
        }
        textItem->setSpaceAfter(hasSpaceAfter);
        po.rawTextData[rawIdx].spaceAfter = hasSpaceAfter;
    }
}

void setSpacesBack(MDParagraphPtr p, MDParsingOpts &po)
{
    // Not the most optimal but this way we don't miss anything
    for (long long int i = 0; i < p->items().length(); ++i) {
        const MDItemWithOptsPtr item = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(i));
        if (item->type() == MD::ItemType::Text) {
            MDTextItemPtr textItem = md4qtHelperFunc::getSharedTextItem(item);

            addSpace(textItem, i, true, p, po);
            addSpace(textItem, i, false, p, po);
        }
    }
}

void dataCleaningFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options)
{
    Q_UNUSED(options);
    setSpacesBack(p, po);

    removeEmpty(p, po);

    /* MD::optimizeParagraph(p, po); */
}
}
