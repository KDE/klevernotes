/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "extendedSyntaxHelper.hpp"
#include "logic/md4qt/doc.hpp"
#include "logic/md4qt/parser.hpp"

struct CompareByStartColumn {
    template<typename T>
    bool operator()(const T &lhs, const T &rhs) const
    {
        return lhs.startLine == rhs.startLine ? lhs.startColumn > rhs.startColumn : lhs.startLine > rhs.startLine;
    }
};

template<typename T>
inline ExtendedSyntaxHelper::MDItemWithOptsPtr getSharedItemWithOpts(const std::shared_ptr<T> &item)
{
    return std::static_pointer_cast<MD::ItemWithOpts<MD::QStringTrait>>(item);
}

template<typename T>
inline ExtendedSyntaxHelper::MDTextItemPtr getSharedTextItem(const std::shared_ptr<T> &item)
{
    return std::static_pointer_cast<MD::Text<MD::QStringTrait>>(item);
}

namespace ExtendedSyntaxHelper
{

void getDelim(MDParagraphPtr p,
              MDParsingOpts &po,
              const long long int idx,
              QList<DelimInfo> &delimInfos,
              QList<StyleDelimInfo> &waitingOpeningsStyles,
              QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
              QList<long long int> &paraIdxToRawIdx,
              const QString &searchedDelim)
{
    /* Collect the opening/closing style of the paragraph */
    auto currentItem = getSharedItemWithOpts(p->getItemAt(idx));
    const auto openingStyles = currentItem->openStyles();
    const auto closingStyles = currentItem->closeStyles();

    for (const auto &styleDelim : openingStyles) {
        waitingOpeningsStyles.append({idx, currentItem, styleDelim, TagType::Opening, styleDelim.startColumn(), styleDelim.startLine()});
    }
    for (const auto &styleDelim : closingStyles) {
        openCloseStyles.append(
            {waitingOpeningsStyles.takeLast(), {idx, currentItem, styleDelim, TagType::Closing, styleDelim.startColumn(), styleDelim.startLine()}});
    }
    /* ================================================== */

    if (currentItem->type() != MD::ItemType::Text) {
        return;
    }

    const auto localPos = MD::localPosFromVirgin(po.fr, currentItem->startColumn(), currentItem->startLine());
    const auto lineInfo = po.fr.data.at(localPos.second);

    const QString src = po.rawTextData[paraIdxToRawIdx.length()].str;
    paraIdxToRawIdx.append(idx);

    int delimIdx = src.indexOf(searchedDelim);

    const int delimLength = searchedDelim.length();

    while (-1 < delimIdx) {
        const QChar charBeforeDelim = (localPos.first == 0 && delimIdx == 0) ? QChar() : lineInfo.first[localPos.first + delimIdx - 1];
        if (charBeforeDelim == QChar::fromLatin1('\\')) {
            delimIdx = src.indexOf(searchedDelim, delimIdx + 2);
            continue;
        }
        const bool spaceBeforeDelim = charBeforeDelim.isSpace();
        const bool charAfterDelimExist = localPos.first + delimIdx + 3 <= lineInfo.first.length() - 1;
        const QChar charAfterDelim = !charAfterDelimExist ? QChar() : lineInfo.first[localPos.first + delimIdx + delimLength];
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

        DelimInfo info = {idx, currentItem, localPos.first + delimIdx, currentItem->startLine(), type};
        delimInfos.append(info);
        delimIdx = src.indexOf(searchedDelim, delimIdx + delimLength);
    }
}

bool validDelimsPairs(MDParagraphPtr p,
                      QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                      QList<StyleDelimInfo> &badStyles,
                      DelimInfo &openDelim,
                      DelimInfo &closeDelim)
{
    const auto openingItem = getSharedTextItem(p->getItemAt(openDelim.paraIdx));
    const auto openingStyles = openingItem->openStyles();
    const auto closingStyles = openingItem->closeStyles();

    long long int i = 0;
    while (i < openCloseStyles.size()) {
        const auto stylePair = openCloseStyles[i];
        const int styleOpeningPos = stylePair.first.delim.startColumn();
        const int styleClosingPos = stylePair.second.delim.startColumn();
        const int styleClosingLine = stylePair.second.delim.startLine();

        if (styleClosingPos < openDelim.startColumn || styleClosingLine != openDelim.startLine) {
            ++i;
            continue;
        }

        const bool openInsideStyle = styleOpeningPos < openDelim.startColumn;
        const bool closeInsideStyle = closeDelim.startColumn < styleClosingPos;

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

QList<DelimInfo>
pairDelims(MDParagraphPtr p, QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles, QList<StyleDelimInfo> &badStyles, QList<DelimInfo> &delimInfos)
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

                if (validDelimsPairs(p, openCloseStyles, badStyles, opening, info)) {
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

                    if (validDelimsPairs(p, openCloseStyles, badStyles, opening, info)) {
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

void removeBadStylesOpts(MDParagraphPtr p, const QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles, const QList<StyleDelimInfo> &badStyles)
{
    for (int i = 0; i < badStyles.length(); i += 2) {
        const auto &badOpening = badStyles[i];
        auto &openingStyles = badOpening.itemPtr->openStyles();

        const auto openStylesIt = std::find(openingStyles.cbegin(), openingStyles.cend(), badOpening.delim);
        if (openStylesIt != openingStyles.cend()) {
            openingStyles.removeAt(std::distance(openingStyles.cbegin(), openStylesIt));
        }

        const auto &badClosing = badStyles[i + 1];
        auto &closingStyles = badClosing.itemPtr->closeStyles();

        const auto closeStylesIt = std::find(closingStyles.cbegin(), closingStyles.cend(), badClosing.delim);
        if (closeStylesIt != closingStyles.cend()) {
            closingStyles.removeAt(std::distance(closingStyles.cbegin(), closeStylesIt));
        }

        const auto style = badOpening.delim.style();

        for (long long int idx = badOpening.paraIdx; idx <= badClosing.paraIdx; ++idx) {
            auto currentItem = getSharedItemWithOpts(p->getItemAt(idx));

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

void addNewStyleOpt(MDParagraphPtr p, const QList<DelimInfo> &pairs, const int newStyleOpt, const int delimLength)
{
    for (int i = 0; i < pairs.length(); i += 2) {
        const auto &opening = pairs[i];

        auto openingTextItem = getSharedTextItem(p->getItemAt(opening.paraIdx));
        const MD::StyleDelim openingStyle =
            MD::StyleDelim(newStyleOpt, opening.startColumn, opening.startLine, opening.startColumn + delimLength - 1, opening.startLine);
        openingTextItem->openStyles().append(openingStyle);

        int openingOpts = openingTextItem->opts();
        if (!(openingOpts & newStyleOpt)) {
            openingTextItem->setOpts(openingOpts + newStyleOpt);
        }

        const auto &closing = pairs[i + 1];
        auto closingTextItem = getSharedTextItem(p->getItemAt(closing.paraIdx));
        const MD::StyleDelim closingStyle =
            MD::StyleDelim(newStyleOpt, closing.startColumn, closing.startLine, closing.startColumn + delimLength - 1, closing.startLine);
        closingTextItem->closeStyles().append(closingStyle);

        int closingOpts = closingTextItem->opts();
        if (!(closingOpts & newStyleOpt)) {
            closingTextItem->setOpts(closingOpts + newStyleOpt);
        }

        for (long long int i = opening.paraIdx + 1; i < closing.paraIdx; ++i) {
            auto item = getSharedItemWithOpts(p->getItemAt(i));
            int opts = item->opts();
            if (!(opts & newStyleOpt)) {
                item->setOpts(opts + newStyleOpt);
            }
        }
    }
}

void handleOptsAndStyles(MDParagraphPtr p,
                         const QList<DelimInfo> &pairs,
                         const int newStyleOpt,
                         const int delimLength,
                         const QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                         const QList<StyleDelimInfo> &badStyles)
{
    addNewStyleOpt(p, pairs, newStyleOpt, delimLength);

    removeBadStylesOpts(p, openCloseStyles, badStyles);
}

void transferStyles(const StyleDelimInfo &styleInfo, MDItemWithOptsPtr &existingItem, MDTextItemPtr &newItem, const bool newBeforeExisting)
{
    auto &existingItemStyles = newBeforeExisting ? existingItem->openStyles() : existingItem->closeStyles();

    int styleLimitIdx = newBeforeExisting ? 0 : existingItemStyles.length() - 1;
    int validStyleIdx = styleLimitIdx;

    if (newBeforeExisting) {
        for (; validStyleIdx < existingItemStyles.length(); ++validStyleIdx) {
            if (styleInfo.startColumn < existingItemStyles[validStyleIdx].startColumn()) {
                break;
            }
        }
    } else {
        for (; 0 <= validStyleIdx; --validStyleIdx) {
            if (existingItemStyles[validStyleIdx].endColumn() < styleInfo.startColumn) {
                break;
            }
        }
    }

    if (validStyleIdx != styleLimitIdx) {
        const auto leftStyles = existingItemStyles.mid(0, validStyleIdx + 1);
        const auto rightStyles = existingItemStyles.mid(validStyleIdx + 1);

        auto &newItemStyles = newBeforeExisting ? newItem->openStyles() : newItem->closeStyles();

        if (newBeforeExisting) {
            newItemStyles = leftStyles;
            existingItemStyles = rightStyles;

            newItem->setSpaceAfter(false);

            if (existingItem->type() == MD::ItemType::Text) {
                getSharedTextItem(existingItem)->setSpaceBefore(false);
            }
        } else {
            existingItemStyles = leftStyles;
            newItemStyles = rightStyles;

            if (existingItem->type() == MD::ItemType::Text) {
                getSharedTextItem(existingItem)->setSpaceAfter(false);
            }

            newItem->setSpaceBefore(false);
        }

        int opts = newItem->opts();
        for (const auto &styleDelim : newItemStyles) {
            int style = styleDelim.style();
            if (!(opts & style)) {
                opts += style;
            }
        }
    }
}

void restoreBadStyleText(MDParagraphPtr p, MDParsingOpts &po, const QList<StyleDelimInfo> &badStyles, QList<long long int> &paraIdxToRawIdx)
{
    long long int initialPreviousStyleParaIdx = -1;
    long long int modifiedPreviousStyleParaIdx = -1;
    long long int previousStyleRawIdx = po.rawTextData.size();

    for (const auto &styleInfo : badStyles) {
        // Deals with the offset coming from adding/removing Items from the Paragraph list
        long long int paraIdx;
        if (styleInfo.paraIdx == initialPreviousStyleParaIdx) {
            paraIdx = modifiedPreviousStyleParaIdx;
        } else {
            initialPreviousStyleParaIdx = styleInfo.paraIdx;
            modifiedPreviousStyleParaIdx = initialPreviousStyleParaIdx;

            paraIdx = initialPreviousStyleParaIdx;
        }

        const QString delimText = MD::virginSubstr(po.fr, styleInfo.delim);

        MDTextItemPtr currentTextItem = nullptr;
        MDItemWithOptsPtr currentItem = styleInfo.itemPtr;

        long long int currentTrailPos = currentItem->startColumn();
        long long int currentTailPos = currentItem->endColumn();
        QString currentItemText = {};
        QString currentRawText = {};
        long long int currentRawIdx = -1;
        bool reattached = false;

        if (currentItem->type() == MD::ItemType::Text) {
            currentRawIdx = paraIdxToRawIdx.indexOf(paraIdx);
            previousStyleRawIdx = currentRawIdx;

            currentTextItem = getSharedTextItem(currentItem);

            const auto &openingStyles = currentTextItem->openStyles();
            const auto &closingStyles = currentTextItem->closeStyles();
            currentTrailPos = openingStyles.isEmpty() ? currentTrailPos : openingStyles.front().startColumn();
            currentTailPos = closingStyles.isEmpty() ? currentTailPos : closingStyles.back().endColumn();

            currentItemText = currentTextItem->text();
            currentRawText = po.rawTextData[currentRawIdx].str;

            if (styleInfo.delim.endColumn() + 1 == currentItem->startColumn()) {
                currentItemText = delimText + currentItemText;
                currentRawText = delimText + currentRawText;

                auto newStartColumn = currentTextItem->startColumn() - delimText.length();
                currentTextItem->setStartColumn(newStartColumn);
                po.rawTextData[currentRawIdx].pos = newStartColumn;
                reattached = true;
            } else if (currentItem->endColumn() + 1 == styleInfo.delim.startColumn()) {
                currentItemText = currentItemText + delimText;
                currentRawText = currentRawText + delimText;

                auto newEndColumn = currentTextItem->endColumn() + delimText.length();
                currentTextItem->setEndColumn(newEndColumn);
                reattached = true;
            }
            currentTextItem->setText(currentItemText);
            po.rawTextData[currentRawIdx].str = currentRawText;
        }

        MDItemWithOptsPtr nextItem = nullptr;
        if (styleInfo.paraIdx != p->items().length() - 1) {
            const long long int nextItemIdx = paraIdx + 1;
            const auto nextItemSharedPtr = p->getItemAt(nextItemIdx);
            nextItem = getSharedItemWithOpts(nextItemSharedPtr);

            if (nextItemSharedPtr->type() == MD::ItemType::Text && styleInfo.delim.endColumn() + 1 == nextItemSharedPtr->startColumn()) {
                const long long int nextRawIdx = paraIdxToRawIdx.indexOf(nextItemIdx);
                MDTextItemPtr nextTextItem = getSharedTextItem(nextItemSharedPtr);

                QString nextItemText = nextTextItem->text();
                QString nextRawText = po.rawTextData[nextRawIdx].str;

                if (reattached) { // merge nextTextItem into currentItem
                    currentRawText = currentRawText + nextRawText;

                    currentItemText = MD::replaceEntity<MD::QStringTrait>(currentRawText.simplified());
                    currentItemText = MD::removeBackslashes<MD::QStringTrait>(currentRawText).asString();

                    currentTextItem->setText(currentItemText);
                    po.rawTextData[currentRawIdx].str = currentRawText;
                    currentTextItem->setEndColumn(nextTextItem->endColumn());
                    currentTextItem->closeStyles() << nextTextItem->closeStyles();

                    po.rawTextData.erase(po.rawTextData.cbegin() + nextRawIdx);
                    p->removeItemAt(nextItemIdx);
                    paraIdxToRawIdx.remove(nextRawIdx);
                } else {
                    if (currentTextItem && currentTailPos + 1 == styleInfo.startColumn) {
                        currentTextItem->setSpaceAfter(false);
                        nextTextItem->setSpaceBefore(false);
                    }

                    nextRawText = delimText + nextRawText;

                    nextItemText = MD::replaceEntity<MD::QStringTrait>(nextRawText.simplified());
                    nextItemText = MD::removeBackslashes<MD::QStringTrait>(nextRawText).asString();

                    nextTextItem->setText(nextItemText);
                    po.rawTextData[nextRawIdx].str = nextRawText;

                    auto newStartColumn = nextTextItem->startColumn() - delimText.length();
                    nextTextItem->setStartColumn(newStartColumn);
                    po.rawTextData[nextRawIdx].pos = newStartColumn;

                    reattached = true;
                }
            }
        }

        MDItemWithOptsPtr previousItem = nullptr;
        if (styleInfo.paraIdx != 0) {
            const auto previousItemSharedPtr = p->getItemAt(paraIdx - 1);
            previousItem = getSharedItemWithOpts(previousItemSharedPtr);

            if (previousItemSharedPtr->type() == MD::ItemType::Text && previousItemSharedPtr->startColumn() + 1 == styleInfo.delim.startColumn()) {
                const long long int previousRawIdx = paraIdxToRawIdx.indexOf(paraIdx - 1);
                MDTextItemPtr previousTextItem = getSharedTextItem(previousItemSharedPtr);

                QString previousItemText = previousTextItem->text();
                QString previousRawText = po.rawTextData[previousRawIdx].str;

                if (reattached) { // merge currentItem into previousItem
                    previousRawText = previousRawText + currentRawText;

                    previousItemText = MD::replaceEntity<MD::QStringTrait>(previousRawText.simplified());
                    previousItemText = MD::removeBackslashes<MD::QStringTrait>(previousRawText).asString();

                    previousTextItem->setEndColumn(currentItem->endColumn());

                    previousTextItem->closeStyles() << currentTextItem->closeStyles();

                    po.rawTextData.erase(po.rawTextData.cbegin() + currentRawIdx);
                    p->removeItemAt(paraIdx);

                    paraIdxToRawIdx.remove(currentRawIdx);
                    modifiedPreviousStyleParaIdx--;
                } else {
                    if (currentTextItem && styleInfo.delim.endColumn() == currentTrailPos - 1) {
                        previousTextItem->setSpaceAfter(false);
                        currentTextItem->setSpaceBefore(false);
                    }

                    previousRawText = previousRawText + delimText;

                    previousItemText = MD::replaceEntity<MD::QStringTrait>(previousRawText.simplified());
                    previousItemText = MD::removeBackslashes<MD::QStringTrait>(previousRawText).asString();

                    auto newEndColumn = currentItem->endColumn() + delimText.length();
                    previousTextItem->setEndColumn(newEndColumn);

                    reattached = true;
                }

                previousTextItem->setText(previousItemText);
                po.rawTextData[previousRawIdx].str = previousRawText;
            }
        }

        if (!reattached) {
            MDParsingOpts::TextData newTextData;
            newTextData.str = delimText;
            newTextData.pos = styleInfo.delim.startColumn();
            newTextData.line = styleInfo.delim.startLine();

            MDTextItemPtr newTextItem = std::make_shared<MD::Text<MD::QStringTrait>>();
            newTextItem->setText(delimText);
            newTextItem->setSpaceBefore(true);
            newTextItem->setSpaceAfter(true);
            newTextItem->setStartLine(styleInfo.delim.startLine());
            newTextItem->setStartColumn(styleInfo.delim.startColumn());
            newTextItem->setEndLine(styleInfo.delim.endLine());
            newTextItem->setEndColumn(styleInfo.delim.endColumn());

            if (styleInfo.type == TagType::Opening) {
                if (previousItem) {
                    transferStyles(styleInfo, previousItem, newTextItem, false);
                }

                transferStyles(styleInfo, currentItem, newTextItem, true);

                paraIdxToRawIdx.insert(previousStyleRawIdx, paraIdx);

                po.rawTextData.insert(po.rawTextData.cbegin() + previousStyleRawIdx, newTextData);
                p->insertItem(paraIdx, newTextItem);
            } else {
                transferStyles(styleInfo, currentItem, newTextItem, false);

                if (nextItem) {
                    transferStyles(styleInfo, nextItem, newTextItem, true);
                }

                paraIdxToRawIdx.insert(previousStyleRawIdx + 1, paraIdx);

                po.rawTextData.insert(po.rawTextData.cbegin() + previousStyleRawIdx + 1, newTextData);
                p->insertItem(paraIdx + 1, newTextItem);
            }
        }
    }
}

void removeDelimText(MDParagraphPtr p,
                     MDParsingOpts &po,
                     const QList<DelimInfo> &pairs,
                     QList<long long int> &paraIdxToRawIdx,
                     const int newStyleOpt,
                     const int delimLength)
{
    // delim.paraIdx may not be correct anymore due to adding/removing items, can't use it
    long long int rawIdx = paraIdxToRawIdx.length() - 1;
    long long int paraIdx = paraIdxToRawIdx.last();
    auto currentTextItem = getSharedTextItem(p->getItemAt(paraIdx));
    auto currentRawTextData = po.rawTextData[rawIdx];

    for (const auto &delim : pairs) {
        while (delim.startLine < currentTextItem->startLine() || delim.startColumn < currentTextItem->startColumn()) {
            --rawIdx;
            Q_ASSERT(0 <= rawIdx);
            paraIdx = paraIdxToRawIdx[rawIdx];
            currentTextItem = getSharedTextItem(p->getItemAt(paraIdx));
            currentRawTextData = po.rawTextData[rawIdx];
        }
        const auto delimRawOffSet = delim.startColumn - currentRawTextData.pos;

        QString itemText = currentTextItem->text();
        QString rawText = currentRawTextData.str;

        if (rawText.length() == delimLength) {
            if (delim.type == TagType::Opening) {
                auto nextItem = getSharedItemWithOpts(p->getItemAt(paraIdx + 1));
                nextItem->openStyles() << currentTextItem->openStyles();
            } else {
                auto previousItem = getSharedItemWithOpts(p->getItemAt(paraIdx - 1));
                previousItem->closeStyles() << currentTextItem->closeStyles();
            }
            po.rawTextData.erase(po.rawTextData.cbegin() + rawIdx);
            p->removeItemAt(paraIdx);
            paraIdxToRawIdx.remove(rawIdx);
            continue;
        }

        if (delimRawOffSet == 0) {
            rawText.remove(0, delimLength);
            currentRawTextData.str = rawText;
            currentRawTextData.pos = currentRawTextData.pos + delimLength;
            po.rawTextData[rawIdx] = currentRawTextData;

            auto text = MD::replaceEntity<MD::QStringTrait>(rawText.simplified());
            text = MD::removeBackslashes<MD::QStringTrait>(text).asString();
            currentTextItem->setText(text);
            currentTextItem->setStartColumn(currentRawTextData.pos);
            currentTextItem->setSpaceBefore(rawText[0].isSpace());

            if (delim.type == TagType::Closing) {
                const int opts = currentTextItem->opts();
                if (opts & newStyleOpt) {
                    currentTextItem->setOpts(opts - newStyleOpt);
                }

                Q_ASSERT(!currentTextItem->closeStyles().isEmpty()); // The item should have received it's close style in the last method
                const auto &styleDelim = currentTextItem->closeStyles().takeLast();

                MDItemWithOptsPtr previousItem = getSharedItemWithOpts(p->getItemAt(paraIdx - 1));
                previousItem->closeStyles().append(styleDelim);
            }
            continue;
        }

        if (rawText.length() == delimRawOffSet + delimLength) {
            rawText.chop(delimLength);
            currentRawTextData.str = rawText;
            po.rawTextData[rawIdx] = currentRawTextData;

            auto text = MD::replaceEntity<MD::QStringTrait>(rawText.simplified());
            text = MD::removeBackslashes<MD::QStringTrait>(text).asString();
            currentTextItem->setText(text);
            currentTextItem->setEndColumn(currentTextItem->endColumn() - delimLength);
            currentTextItem->setSpaceAfter(rawText.back().isSpace());

            if (delim.type == TagType::Opening) {
                const int opts = currentTextItem->opts();
                if (opts & newStyleOpt) {
                    currentTextItem->setOpts(opts - newStyleOpt);
                }

                Q_ASSERT(!currentTextItem->openStyles().isEmpty()); // The item should have received it's open style in the last method
                const auto &styleDelim = currentTextItem->openStyles().takeLast();
                MDItemWithOptsPtr nextItem = getSharedItemWithOpts(p->getItemAt(paraIdx + 1));
                nextItem->openStyles().append(styleDelim);
            }
            continue;
        }

        const long long int rightStartPos = delimRawOffSet + delimLength;
        const QString rightRawText = rawText.mid(rightStartPos);
        auto newTextItem = std::make_shared<MD::Text<MD::QStringTrait>>();
        auto rightText = MD::replaceEntity<MD::QStringTrait>(rightRawText.simplified());
        rightText = MD::removeBackslashes<MD::QStringTrait>(rightText).asString();

        MDParsingOpts::TextData newTextData;
        newTextData.str = rightRawText;
        newTextData.pos = rightStartPos;
        newTextData.line = delim.startLine;
        po.rawTextData.insert(po.rawTextData.cbegin() + rawIdx + 1, newTextData);
        paraIdxToRawIdx.insert(rawIdx + 1, paraIdx + 1);

        newTextItem->setText(rightText);
        newTextItem->setStartLine(currentTextItem->startLine());
        newTextItem->setStartColumn(rightStartPos);
        newTextItem->setEndLine(currentTextItem->endLine());
        newTextItem->setEndColumn(currentTextItem->endColumn());
        newTextItem->setSpaceAfter(currentTextItem->isSpaceAfter());
        newTextItem->setSpaceBefore(rawText[rightStartPos].isSpace());
        newTextItem->setOpts(currentTextItem->opts());
        p->insertItem(paraIdx + 1, newTextItem);

        const QString leftRawText = rawText.left(delimRawOffSet);
        auto leftText = MD::replaceEntity<MD::QStringTrait>(leftRawText.simplified());
        leftText = MD::removeBackslashes<MD::QStringTrait>(leftText).asString();

        currentRawTextData.str = leftRawText;
        po.rawTextData[rawIdx] = currentRawTextData;
        currentTextItem->setText(leftText);
        currentTextItem->setEndColumn(delim.startColumn - 1);
        currentTextItem->setSpaceAfter(rawText[delimRawOffSet - 1].isSpace());

        if (delim.type == TagType::Opening) {
            const int opts = currentTextItem->opts();
            if (opts & newStyleOpt) {
                currentTextItem->setOpts(opts - newStyleOpt);
            }

            const auto &openingStyle = currentTextItem->openStyles().takeLast();
            newTextItem->openStyles().append(openingStyle);

            newTextItem->closeStyles() << currentTextItem->closeStyles();
            currentTextItem->closeStyles().clear();
        } else {
            const int opts = newTextItem->opts();
            if (opts & newStyleOpt) {
                newTextItem->setOpts(opts - newStyleOpt);
            }
        }
    }
}

void handleText(MDParagraphPtr p,
                MDParsingOpts &po,
                QList<DelimInfo> &pairs,
                QList<long long int> &paraIdxToRawIdx,
                const int delimLength,
                const int newStyleOpt,
                QList<StyleDelimInfo> &badStyles)
{
    std::sort(badStyles.begin(), badStyles.end(), CompareByStartColumn{});
    restoreBadStyleText(p, po, badStyles, paraIdxToRawIdx);

    std::sort(pairs.begin(), pairs.end(), CompareByStartColumn{});
    removeDelimText(p, po, pairs, paraIdxToRawIdx, newStyleOpt, delimLength);
}

void processExtendedSyntax(MDParagraphPtr p, MDParsingOpts &po, const QString &searchedDelim, const int newStyleOpt)
{
    if (!po.collectRefLinks) {
        if (po.rawTextData.empty())
            return;

        QList<long long int> paraIdxToRawIdx;
        QList<DelimInfo> delimInfos;
        QList<StyleDelimInfo> waitingOpeningsStyles;
        QList<QPair<StyleDelimInfo, StyleDelimInfo>> openCloseStyles;

        for (long long int i = 0; i < p->items().size(); ++i) {
            getDelim(p, po, i, delimInfos, waitingOpeningsStyles, openCloseStyles, paraIdxToRawIdx, searchedDelim);
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
        auto pairs = pairDelims(p, openCloseStyles, badStyles, delimInfos);

        const int delimLength = searchedDelim.length();

        handleOptsAndStyles(p, pairs, newStyleOpt, delimLength, openCloseStyles, badStyles);

        handleText(p, po, pairs, paraIdxToRawIdx, delimLength, newStyleOpt, badStyles);

        MD::optimizeParagraph(p, po);
    }
}

void extendedSyntaxHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options)
{
    processExtendedSyntax(p, po, options[0], options[1].toInt());
}
} // ExtendedSyntaxHelper
