/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "extendedSyntaxMaker.hpp"
#include "extendedSyntaxHelper.hpp"

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

        const long long int startColumn = localPos.first + delimIdx;
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
        const int styleOpeningPos = stylePair.first.delim.startColumn();
        const int styleClosingPos = stylePair.second.delim.startColumn();
        const int styleClosingLine = stylePair.second.delim.startLine();

        if (styleClosingPos < openDelim.startColumn() || styleClosingLine != openDelim.startLine()) {
            ++i;
            continue;
        }

        const bool openInsideStyle = styleOpeningPos < openDelim.startColumn() && openDelim.startColumn() < styleClosingPos;
        const bool closeInsideStyle = styleOpeningPos < closeDelim.startColumn() && closeDelim.startColumn() < styleClosingPos;

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

                if (opening.endColumn + 1 != info.startColumn() && validDelimsPairs(openCloseStyles, badStyles, opening, info)) {
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

                    if (opening.endColumn + 1 != info.startColumn() && validDelimsPairs(openCloseStyles, badStyles, opening, info)) {
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

        const bool opening = badStyleInfo.type == TagType::Opening;
        const long long int delimStartPos = badStyleInfo.startColumn;
        const long long int delimEndPos = badStyleInfo.delim.endColumn();
        const QString delimText = MD::virginSubstr(po.fr, badStyleInfo.delim);

        bool reattached = false;
        const bool atStart = (delimEndPos + 1 == itemStartPos);
        if (atStart || (itemEndPos == delimStartPos - 1)) {
            reattached = ExtendedSyntaxHelper::addStringTo(currentItemPtr, atStart, delimText, po);
        }

        const long long int previousParaIdx = paraIdx - 1;
        MDItemWithOptsPtr previousItem = (0 <= previousParaIdx) ? md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(previousParaIdx)) : nullptr;
        if (opening && previousItem && (previousItem->endColumn() == delimStartPos - 1)) {
            if (reattached) {
                ExtendedSyntaxHelper::mergeFromIndex(previousParaIdx, p, po);
                continue;
            }
            reattached = ExtendedSyntaxHelper::addStringTo(previousItem, false, delimText, po);
        }

        const long long int nextParaIdx = paraIdx + 1;
        MDItemWithOptsPtr nextItem = (nextParaIdx < p->items().length()) ? md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(nextParaIdx)) : nullptr;
        if (!opening && nextItem && (delimEndPos + 1 == nextItem->startColumn())) {
            if (reattached) {
                ExtendedSyntaxHelper::mergeFromIndex(paraIdx, p, po);
                continue;
            }
            reattached = ExtendedSyntaxHelper::addStringTo(nextItem, true, delimText, po);
        }

        if (!reattached) { // create new one
            const auto localPos = MD::localPosFromVirgin(po.fr, delimStartPos, badStyleInfo.startLine);
            const long long int nextRawDataIdx = md4qtHelperFunc::rawIdxFromPos(localPos.first, localPos.second, po) + 1;

            MDParsingOpts::TextData newTextData;
            newTextData.str = delimText;
            newTextData.pos = delimStartPos;
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
        const auto &delim = pairs[i];
        const long long int startPos = delim.startColumn();

        // Can't use indexOf, the itemPtr could have been changed to much/merged
        const long long int paraIdx = md4qtHelperFunc::paraIdxFromPos(startPos, delim.startLine(), p);

        ExtendedSyntaxHelper::splitItem(p, po, paraIdx, startPos, delimLength, (delim.type == TagType::Opening), newStyleOpt);
    }
}

void addNewStyleOpt(MDParagraphPtr p, const QList<DelimInfo> &pairs, const int newStyleOpt)
{
    for (int i = 0; i < pairs.length(); i += 2) {
        const auto &opening = pairs[i];
        const auto &closing = pairs[i + 1];

        const long long int openParaIdx = md4qtHelperFunc::paraIdxFromPos(opening.startColumn(), opening.startLine(), p) + 1;
        const long long int closeParaIdx = md4qtHelperFunc::paraIdxFromPos(closing.startColumn(), closing.startLine(), p);

        for (long long int i = openParaIdx; i < closeParaIdx; ++i) {
            auto item = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(i));
            int opts = item->opts();
            if (!(opts & newStyleOpt)) {
                item->setOpts(opts + newStyleOpt);
            }
        }
    }
}

void addSpace(MDTextItemPtr item, MDParsingOpts &po, const bool spaceBefore)
{
    const auto itemLocalPos = MD::localPosFromVirgin(po.fr, item->startColumn(), item->startLine());
    const long long int rawIdx = md4qtHelperFunc::rawIdxFromPos(itemLocalPos.first, itemLocalPos.second, po);

    const auto lineInfo = po.fr.data.at(itemLocalPos.second);
    const QString lineStr = lineInfo.first.asString();

    MDTextItemPtr textItem = md4qtHelperFunc::getSharedTextItem(item);
    if (spaceBefore) {
        const long long int currentTrailingPos = item->openStyles().isEmpty() ? item->startColumn() : item->openStyles().first().startColumn();
        const auto localTrailingPos = MD::localPosFromVirgin(po.fr, currentTrailingPos, item->startLine());

        const bool hasSpaceBefore = localTrailingPos.first == 0 ? true : lineStr.at(localTrailingPos.first).isSpace();
        textItem->setSpaceBefore(hasSpaceBefore);
        po.rawTextData[rawIdx].spaceBefore = hasSpaceBefore;
    } else {
        const long long int currentTailingPos = item->closeStyles().isEmpty() ? item->endColumn() : item->closeStyles().last().endColumn();
        const auto localTailingPos = MD::localPosFromVirgin(po.fr, currentTailingPos, item->startLine());

        const bool hasSpaceAfter = localTailingPos.first == lineStr.length() - 1 ? true : lineStr.at(localTailingPos.first).isSpace();
        textItem->setSpaceAfter(hasSpaceAfter);
        po.rawTextData[rawIdx].spaceAfter = hasSpaceAfter;
    }
}

void setSpacesBack(const long long int fromParaIdx, const long long int toParaIdx, MDParagraphPtr p, MDParsingOpts &po)
{
    const MDItemWithOptsPtr fromItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(fromParaIdx));
    if (fromItem->type() == MD::ItemType::Text) {
        const long long int rawIdx = md4qtHelperFunc::rawIdxFromItem(fromItem, po);
        MDTextItemPtr textItem = md4qtHelperFunc::getSharedTextItem(fromItem);

        if (fromParaIdx == 0) {
            textItem->setSpaceBefore(true);
            po.rawTextData[rawIdx].spaceBefore = true;
        } else {
            addSpace(textItem, po, true);

            const MDItemWithOptsPtr previousItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(fromParaIdx - 1));
            if (previousItem->type() == MD::ItemType::Text) {
                const MDTextItemPtr previousTextItem = md4qtHelperFunc::getSharedTextItem(previousItem);
                addSpace(previousTextItem, po, false);
            }
        }
    }

    const MDItemWithOptsPtr toItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(toParaIdx));
    if (toItem->type() == MD::ItemType::Text) {
        const long long int rawIdx = md4qtHelperFunc::rawIdxFromItem(toItem, po);
        MDTextItemPtr textItem = md4qtHelperFunc::getSharedTextItem(toItem);

        const long long int lastParaIdx = p->items().length() - 1;
        if (toParaIdx == lastParaIdx) {
            textItem->setSpaceAfter(true);
            po.rawTextData[rawIdx].spaceAfter = true;
        } else {
            addSpace(textItem, po, false);

            const MDItemWithOptsPtr nextItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(toParaIdx + 1));
            if (nextItem->type() == MD::ItemType::Text) {
                const MDTextItemPtr nextTextItem = md4qtHelperFunc::getSharedTextItem(nextItem);
                addSpace(nextTextItem, po, true);
            }
        }
    }

    for (long long int i = fromParaIdx + 1; i < toParaIdx; ++i) {
        const MDItemWithOptsPtr item = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(i));
        if (item->type() == MD::ItemType::Text) {
            MDTextItemPtr textItem = md4qtHelperFunc::getSharedTextItem(item);

            addSpace(textItem, po, true);
            addSpace(textItem, po, false);
        }
    }
}

void setSpacesBack(const QList<DelimInfo> &pairs, const QList<StyleDelimInfo> &badStyles, MDParagraphPtr p, MDParsingOpts &po)
{
    const auto &firstNewDelim = pairs.first();
    const auto &lastNewDelim = pairs.last();

    const long long int firstNewDelimParaIdx = md4qtHelperFunc::paraIdxFromPos(firstNewDelim.startColumn(), firstNewDelim.startLine(), p) + 1;
    const long long int lastNewDelimParaIdx = md4qtHelperFunc::paraIdxFromPos(lastNewDelim.startColumn(), lastNewDelim.startLine(), p);

    long long int firstParaIdx = firstNewDelimParaIdx;
    long long int lastParaIdx = lastNewDelimParaIdx;
    if (!badStyles.isEmpty()) {
        const auto &firstBadStyle = badStyles.first();
        const auto &lastBadStyle = badStyles.last();

        const long long int firstBadStyleParaIdx = md4qtHelperFunc::paraIdxFromPos(firstBadStyle.startColumn, firstBadStyle.startLine, p) + 1;
        const long long int lastBadStyleParaIdx = md4qtHelperFunc::paraIdxFromPos(lastBadStyle.startColumn, lastBadStyle.startLine, p);

        firstParaIdx = firstBadStyleParaIdx < firstParaIdx ? firstNewDelimParaIdx : firstParaIdx;
        lastParaIdx = lastParaIdx < lastBadStyleParaIdx ? lastBadStyleParaIdx : lastParaIdx;
    }

    setSpacesBack(firstParaIdx, lastParaIdx, p, po);
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

        if (po.fr.data.at(0).second.lineNumber == 4) {
            qDebug() << "inspect";
        }
        setSpacesBack(orderedPairs, badStyles, p, po);

        MD::optimizeParagraph(p, po);
    }
}

void extendedSyntaxHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options)
{
    processExtendedSyntax(p, po, options[0], options[1].toInt());
}
} // !ExtendedSyntaxMaker
