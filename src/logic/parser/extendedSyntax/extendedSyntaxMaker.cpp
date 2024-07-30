/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "extendedSyntaxMaker.hpp"
#include "../md4qtDataManip.hpp"
#include "logic/parser/md4qtDataGetter.hpp"

namespace ExtendedSyntaxMaker
{
void getDelims(MDParagraphPtr p,
               MDParsingOpts &po,
               const long long int idx,
               QList<DelimInfo> &delimInfos,
               QList<StyleDelimInfo> &waitingOpeningsStyles,
               QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
               const QString &searchedDelim)
{
    /* Collect the opening/closing style of the paragraph */
    MDItemWithOptsPtr currentItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(idx));

    const auto openingStyles = currentItem->openStyles();
    const auto closingStyles = currentItem->closeStyles();

    for (const auto &styleDelim : openingStyles) {
        waitingOpeningsStyles.append({idx, styleDelim, TagType::Opening, styleDelim.startColumn(), styleDelim.startLine()});
    }
    for (const auto &styleDelim : closingStyles) {
        openCloseStyles.append({waitingOpeningsStyles.takeLast(), {idx, styleDelim, TagType::Closing, styleDelim.startColumn(), styleDelim.startLine()}});
    }
    /* ================================================== */

    if (currentItem->type() != MD::ItemType::Text) {
        return;
    }

    const auto localPos = MD::localPosFromVirgin(po.fr, currentItem->startColumn(), currentItem->startLine());
    const auto lineInfo = po.fr.data.at(localPos.second);

    const long long int rawIdx = md4qtHelperFunc::rawIdxFromPos(localPos.first, localPos.second, po);
    auto s = po.rawTextData[rawIdx];
    const QString src = s.str;

    int delimIdx = src.indexOf(searchedDelim);

    const int delimLength = searchedDelim.length();

    while (-1 < delimIdx) {
        const QChar charBeforeDelim = (localPos.first == 0 && delimIdx == 0) ? QChar::fromLatin1(' ') : lineInfo.first[localPos.first + delimIdx - 1];
        if (charBeforeDelim == QChar::fromLatin1('\\')) {
            delimIdx = src.indexOf(searchedDelim, delimIdx + delimLength);
            continue;
        }
        const bool spaceBeforeDelim = charBeforeDelim.isSpace();
        const bool charAfterDelimExist = localPos.first + delimIdx + delimLength <= lineInfo.first.length() - 1;
        const QChar charAfterDelim = !charAfterDelimExist ? QChar::fromLatin1(' ') : lineInfo.first[localPos.first + delimIdx + delimLength];
        const bool spaceAfterDelim = charAfterDelim.isSpace();

        TagType type;
        if (!spaceBeforeDelim && !spaceAfterDelim) {
            type = TagType::Both;
        } else if (spaceBeforeDelim && !spaceAfterDelim) {
            type = TagType::Opening;
        } else if (!spaceBeforeDelim && spaceAfterDelim) {
            type = TagType::Closing;
        } else {
            delimIdx = src.indexOf(searchedDelim, delimIdx + delimLength);
            continue;
        }

        const long long int startColumn = currentItem->startColumn() + delimIdx;
        const long long int endColumn = startColumn + delimLength - 1;
        DelimInfo info = {idx, startColumn, currentItem->startLine(), endColumn, type};
        delimInfos.append(info);
        delimIdx = src.indexOf(searchedDelim, delimIdx + delimLength);
    }
}

bool validDelimsPairs(QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                      QList<StyleDelimInfo> &badStyles,
                      DelimInfo &openDelim,
                      DelimInfo &closeDelim)
{
    long long int i = 0;
    while (i < openCloseStyles.size()) {
        const auto stylePair = openCloseStyles[i];

        const MD::WithPosition openDelimPos = {openDelim.startColumn(), openDelim.startLine(), openDelim.endColumn, openDelim.startLine()};
        const MD::WithPosition closeDelimPos = {closeDelim.startColumn(), closeDelim.startLine(), closeDelim.endColumn, closeDelim.startLine()};

        const bool openInsideStyle = md4qtHelperFunc::isBetweenDelims(openDelimPos, stylePair.first.delim, stylePair.second.delim);
        const bool closeInsideStyle = md4qtHelperFunc::isBetweenDelims(closeDelimPos, stylePair.first.delim, stylePair.second.delim);

        if (openInsideStyle && !closeInsideStyle) {
            // This opening is not good
            return false;
        } else if (!openInsideStyle && closeInsideStyle) {
            badStyles.append(stylePair.first);
            badStyles.append(stylePair.second);
            openCloseStyles.removeAt(i);
            continue;
        }

        ++i;
    }

    openDelim.type = TagType::Opening;
    closeDelim.type = TagType::Closing;
    return true;
}

QList<DelimInfo> pairDelims(QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles, QList<StyleDelimInfo> &badStyles, QList<DelimInfo> &delimInfos)
{
    QList<DelimInfo> pairs;
    QList<DelimInfo> waitingOpenings;
    for (auto &info : delimInfos) {
        switch (info.type) {
        case TagType::Opening:
            waitingOpenings.append(info);
            break;
        case TagType::Closing: {
            int i = waitingOpenings.length() - 1;
            while (0 <= i) {
                auto opening = waitingOpenings.takeAt(i);

                if ((opening.startLine() < info.startLine() || (opening.startLine() == info.startLine() && opening.endColumn + 1 != info.startColumn()))
                    && validDelimsPairs(openCloseStyles, badStyles, opening, info)) {
                    opening.paired = true;
                    info.paired = true;

                    pairs << opening << info;
                    break;
                }
                --i;
            }
            break;
        }
        case TagType::Both: {
            if (!waitingOpenings.isEmpty()) {
                int i = waitingOpenings.length() - 1;
                while (0 <= i) {
                    auto opening = waitingOpenings[i]; // We can't consume it right away

                    if ((opening.startLine() < info.startLine() || (opening.startLine() == info.startLine() && opening.endColumn + 1 != info.startColumn()))
                        && validDelimsPairs(openCloseStyles, badStyles, opening, info)) {
                        opening.paired = true;
                        info.paired = true;
                        waitingOpenings.removeAt(i);

                        pairs << opening << info;
                        break;
                    }
                    --i;
                }
            }
            if (!info.paired) {
                waitingOpenings.append(info);
            }
        }
        }
    }

    return pairs;
}

void removeBadStylesOptsAndDelims(MDParagraphPtr p, const QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles, const QList<StyleDelimInfo> &badStyles)
{
    for (int i = 0; i < badStyles.length(); i += 2) {
        const auto &badOpening = badStyles[i];
        auto &openingStyles = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(badOpening.paraIdx))->openStyles();

        const auto openStylesIt = std::find(openingStyles.cbegin(), openingStyles.cend(), badOpening.delim);
        if (openStylesIt != openingStyles.cend()) {
            openingStyles.removeAt(std::distance(openingStyles.cbegin(), openStylesIt));
        }

        const auto &badClosing = badStyles[i + 1];
        auto &closingStyles = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(badClosing.paraIdx))->closeStyles();

        const auto closeStylesIt = std::find(closingStyles.cbegin(), closingStyles.cend(), badClosing.delim);
        if (closeStylesIt != closingStyles.cend()) {
            closingStyles.removeAt(std::distance(closingStyles.cbegin(), closeStylesIt));
        }

        const auto style = badOpening.delim.style();

        for (long long int idx = badOpening.paraIdx; idx <= badClosing.paraIdx; ++idx) {
            auto currentItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(idx));

            const auto opts = currentItem->opts();

            if (opts & style) {
                bool inGoodStyle = false;
                // All the bad ones have been removed in validDelims
                for (const auto &goodStylePair : openCloseStyles) {
                    if (goodStylePair.first.delim.style() == style && goodStylePair.first.delim.startColumn() < currentItem->startColumn()
                        && currentItem->endColumn() < goodStylePair.second.delim.endColumn()) {
                        inGoodStyle = true;
                        break;
                    }
                }
                if (!inGoodStyle) {
                    currentItem->setOpts(opts - style);
                }
            }
        }
    }
}

void restoreBadStyleText(MDParagraphPtr p, MDParsingOpts &po, const QList<StyleDelimInfo> &badStyles)
{
    for (const StyleDelimInfo &badStyleInfo : badStyles) {
        const long long int paraIdx = md4qtHelperFunc::paraIdxFromPos(badStyleInfo.startColumn, badStyleInfo.startLine, p);
        MDItemWithOptsPtr currentItemPtr = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx));

        const long long int itemStartPos = currentItemPtr->startColumn();
        const long long int itemEndPos = currentItemPtr->endColumn();

        const long long int delimStartPos = badStyleInfo.startColumn;
        const long long int delimEndPos = badStyleInfo.delim.endColumn();
        const QString delimText = MD::virginSubstr(po.fr, badStyleInfo.delim);

        bool reattached = false;
        const bool atStart = (delimEndPos + 1 == itemStartPos);
        if (atStart || (itemEndPos == delimStartPos - 1)) {
            reattached = md4qtHelperFunc::addStringTo(currentItemPtr, atStart, delimText, po);
        }

        const long long int previousParaIdx = paraIdx - 1;
        MDItemWithOptsPtr previousItem = (0 <= previousParaIdx) ? md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(previousParaIdx)) : nullptr;
        if (previousItem && (previousItem->endLine() == badStyleInfo.startLine) && (previousItem->endColumn() == delimStartPos - 1)) {
            if (reattached) {
                md4qtHelperFunc::mergeFromIndex(previousParaIdx, p, po);
                continue;
            }
            reattached = md4qtHelperFunc::addStringTo(previousItem, false, delimText, po);
        }

        const long long int nextParaIdx = paraIdx + 1;
        MDItemWithOptsPtr nextItem = (nextParaIdx < p->items().length()) ? md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(nextParaIdx)) : nullptr;
        if (nextItem && (nextItem->startLine() == badStyleInfo.startLine) && (delimEndPos + 1 == nextItem->startColumn())) {
            if (reattached) {
                md4qtHelperFunc::mergeFromIndex(paraIdx, p, po);
                continue;
            }
            reattached = md4qtHelperFunc::addStringTo(nextItem, true, delimText, po);
        }

        if (!reattached) { // create new one
            const auto localPos = MD::localPosFromVirgin(po.fr, delimStartPos, badStyleInfo.startLine);
            const long long int virginToLocalDelta = delimStartPos - localPos.first;

            const long long int nextRawDataIdx = md4qtHelperFunc::rawIdxFromPos(localPos.first, localPos.second, po) + 1;

            MDParsingOpts::TextData newTextData;
            newTextData.str = delimText;
            newTextData.pos = delimStartPos - virginToLocalDelta;
            newTextData.line = localPos.second;

            auto newTextItem = std::make_shared<MD::Text<MD::QStringTrait>>();
            newTextItem->setStartLine(badStyleInfo.startLine);
            newTextItem->setEndLine(badStyleInfo.startLine);
            newTextItem->setStartColumn(delimStartPos);
            newTextItem->setEndColumn(badStyleInfo.delim.endColumn());

            auto finalSimplifiedText = MD::replaceEntity<MD::QStringTrait>(delimText.simplified());
            finalSimplifiedText = MD::removeBackslashes<MD::QStringTrait>(finalSimplifiedText).asString();
            newTextItem->setText(finalSimplifiedText);

            po.rawTextData.insert(po.rawTextData.cbegin() + nextRawDataIdx, newTextData);
            p->insertItem(nextParaIdx, newTextItem);
        }
    }
}

void removeDelimText(MDParagraphPtr p, MDParsingOpts &po, const QList<DelimInfo> &pairs, const int newStyleOpt, const int delimLength)
{
    for (long long int i = pairs.length() - 1; 0 <= i; --i) {
        // Delim info
        const auto &delim = pairs[i];
        const long long int delimStartPos = delim.startColumn();
        const long long int delimEndPos = delim.endColumn;

        // Item info
        const long long int paraIdx = md4qtHelperFunc::paraIdxFromPos(delimStartPos, delim.startLine(), p);
        const auto currentItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx));
        const long long int itemStartPos = currentItem->startColumn();
        const long long int itemEndPos = currentItem->endColumn();

        // Raw Data info
        const long long int rawIdx = md4qtHelperFunc::rawIdxFromItem(currentItem, po);

        // Data manip
        const int addedItemCount = md4qtHelperFunc::splitItem(p, po, paraIdx, rawIdx, delimStartPos, delimLength);

        // Add style
        MD::StyleDelim styleDelim = MD::StyleDelim(newStyleOpt, delimStartPos, delim.startLine(), delimEndPos, delim.startLine());
        if (delim.type == TagType::Opening) {
            MDItemWithOptsPtr item =
                (addedItemCount == 1 || delimEndPos == itemEndPos) ? md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx + 1)) : currentItem;

            item->openStyles() << styleDelim;
            std::sort(item->openStyles().begin(), item->openStyles().end(), md4qtHelperFunc::StartColumnOrder{});
        } else {
            MDItemWithOptsPtr item = (delimStartPos == itemStartPos) ? md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx - 1)) : currentItem;
            item->closeStyles() << styleDelim;
            std::sort(item->closeStyles().begin(), item->closeStyles().end(), md4qtHelperFunc::StartColumnOrder{});
        }

        // Handle: item split in half
        if (addedItemCount == 1) {
            const auto nextItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx + 1));
            md4qtHelperFunc::transferStyle(currentItem, nextItem, true);
            continue;
        }

        // Handle: item == delim
        if (addedItemCount == -1) {
            if (paraIdx < p->items().length() - 1) {
                MDItemWithOptsPtr nextItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx + 1));
                nextItem->openStyles() << currentItem->openStyles();
                std::sort(nextItem->openStyles().begin(), nextItem->openStyles().end(), md4qtHelperFunc::StartColumnOrder{});
            }
            if (0 < paraIdx) {
                MDItemWithOptsPtr previousItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx - 1));
                previousItem->closeStyles() << currentItem->closeStyles();
                std::sort(previousItem->closeStyles().begin(), previousItem->closeStyles().end(), md4qtHelperFunc::StartColumnOrder{});
            }
            p->removeItemAt(paraIdx);
            po.rawTextData.erase(po.rawTextData.cbegin() + rawIdx);
        }
    }
}

void addNewStyleOpt(MDParagraphPtr p, const QList<DelimInfo> &pairs, const int newStyleOpt)
{
    for (int i = 0; i < pairs.length(); i += 2) {
        const auto &opening = pairs[i];
        const auto &closing = pairs[i + 1];

        const long long int openParaIdx = md4qtHelperFunc::paraIdxFromPos(opening.startColumn(), opening.startLine(), p) + 1;
        const long long int closeParaIdx = md4qtHelperFunc::paraIdxFromPos(closing.startColumn(), closing.startLine(), p) + 1;

        for (long long int i = openParaIdx; i < closeParaIdx; ++i) {
            auto item = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(i));
            int opts = item->opts();
            if (!(opts & newStyleOpt)) {
                item->setOpts(opts + newStyleOpt);
            }
        }
    }
}

void processExtendedSyntax(MDParagraphPtr p, MDParsingOpts &po, const QString &searchedDelim, const int newStyleOpt)
{
    if (!po.collectRefLinks) {
        if (po.rawTextData.empty()) {
            return;
        }

        QList<DelimInfo> delimInfos;
        QList<StyleDelimInfo> waitingOpeningsStyles;
        QList<QPair<StyleDelimInfo, StyleDelimInfo>> openCloseStyles;

        for (long long int i = 0; i < p->items().size(); ++i) {
            getDelims(p, po, i, delimInfos, waitingOpeningsStyles, openCloseStyles, searchedDelim);
        };

        const auto firstNonClosingIt = std::find_if(delimInfos.cbegin(), delimInfos.cend(), [](const auto &d) {
            return d.type != TagType::Closing;
        });
        const long long int firstNonClosingPos = std::distance(delimInfos.cbegin(), firstNonClosingIt);
        delimInfos.remove(0, firstNonClosingPos);

        if (delimInfos.isEmpty()) {
            return;
        }

        QList<StyleDelimInfo> badStyles;
        auto pairs = pairDelims(openCloseStyles, badStyles, delimInfos);

        if (pairs.length() < 2) {
            return;
        }

        const int delimLength = searchedDelim.length();

        removeBadStylesOptsAndDelims(p, openCloseStyles, badStyles);

        restoreBadStyleText(p, po, badStyles);

        QList<DelimInfo> orderedPairs;
        for (const auto delim : pairs) {
            orderedPairs.append(delim);
        }
        std::sort(orderedPairs.begin(), orderedPairs.end(), md4qtHelperFunc::StartColumnOrder{});
        removeDelimText(p, po, orderedPairs, newStyleOpt, delimLength);

        addNewStyleOpt(p, pairs, newStyleOpt);
    }
}

void extendedSyntaxHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options)
{
    processExtendedSyntax(p, po, options[0], options[1].toInt());
}
} // !ExtendedSyntaxMaker