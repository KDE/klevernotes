/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "textHighlightExtension.hpp"
#include "logic/md4qt/doc.hpp"
#include "logic/md4qt/parser.hpp"

#include <qlogging.h>

struct CompareByStartColumn {
    template<typename T>
    bool operator()(const T &lhs, const T &rhs) const
    {
        return lhs.startColumn > rhs.startColumn;
    }
};

template<typename T>
inline std::shared_ptr<MD::ItemWithOpts<MD::QStringTrait>> getSharedItemWithOpts(const std::shared_ptr<T> &item)
{
    return std::static_pointer_cast<MD::ItemWithOpts<MD::QStringTrait>>(item);
}

template<typename T>
inline TextHighlightFunc::MDTextItem getSharedTextItem(const std::shared_ptr<T> &item)
{
    return std::static_pointer_cast<MD::Text<MD::QStringTrait>>(item);
}

namespace TextHighlightFunc
{

void getDelim(MDParagraphPtr p,
              MDParsingOpts &po,
              long long int idx,
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
        waitingOpeningsStyles.append({idx, styleDelim, TagType::Opening, styleDelim.startColumn()});
    }
    for (const auto &styleDelim : closingStyles) {
        openCloseStyles.append({waitingOpeningsStyles.takeLast(), {idx, styleDelim, TagType::Closing, styleDelim.startColumn()}});
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

        DelimInfo info = {idx, localPos.first + delimIdx, localPos.second, type};
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

        if (styleOpeningPos < openDelim.startColumn) {
            i++;
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

        i++;
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
                i--;
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
                    i--;
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

void removeBadStylesOpts(MDParagraphPtr p, QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles, QList<StyleDelimInfo> &badStyles)
{
    for (int i = 0; i < badStyles.length(); i += 2) {
        const auto &badOpening = badStyles[i];
        auto openingItem = getSharedItemWithOpts(p->getItemAt(badOpening.paraIdx));
        auto &openingStyles = openingItem->openStyles();

        for (long long int styleIdx = 0; styleIdx < openingStyles.length(); styleIdx++) {
            if (openingStyles[styleIdx] == badOpening.delim) {
                openingStyles.remove(styleIdx);
                break;
            }
        }

        const auto &badClosing = badStyles[i + 1];
        auto closingItem = getSharedItemWithOpts(p->getItemAt(badClosing.paraIdx));
        auto &closingStyles = closingItem->closeStyles();

        for (long long int styleIdx = 0; styleIdx < closingStyles.length(); styleIdx++) {
            if (closingStyles[styleIdx] == badClosing.delim) {
                closingStyles.remove(styleIdx);
                break;
            }
        }

        const auto style = badOpening.delim.style();

        for (long long int idx = badOpening.paraIdx; idx <= badClosing.paraIdx; idx++) {
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

void transferStyles(const StyleDelimInfo &styleInfo, MDTextItem &existingItem, MDTextItem &newItem, const bool newBeforeExisting)
{
    auto &existingItemStyles = newBeforeExisting ? existingItem->openStyles() : existingItem->closeStyles();

    int styleLimitIdx = newBeforeExisting ? 0 : existingItemStyles.length() - 1;
    int validStyleIdx = styleLimitIdx;

    if (newBeforeExisting) {
        for (; validStyleIdx < existingItemStyles.length(); validStyleIdx++) {
            if (styleInfo.startColumn < existingItemStyles[validStyleIdx].startColumn()) {
                break;
            }
        }
    } else {
        for (; 0 <= validStyleIdx; validStyleIdx--) {
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
            existingItem->setSpaceBefore(false);
        } else {
            existingItemStyles = leftStyles;
            newItemStyles = rightStyles;

            existingItem->setSpaceAfter(false);
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

void restoreBadStyleText(MDParagraphPtr p, MDParsingOpts &po, QList<StyleDelimInfo> &badStyles, QList<long long int> &paraIdxToRawIdx)
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
        const auto currentGenericItem = p->getItemAt(paraIdx);

        MDTextItem currentTextItem = nullptr;

        long long int currentTrailPos = currentGenericItem->startColumn();
        long long int currentTailPos = currentGenericItem->endColumn();
        QString currentItemText = {};
        QString currentRawText = {};
        long long int currentRawIdx = -1;
        bool reattached = false;

        if (currentGenericItem->type() == MD::ItemType::Text) {
            currentRawIdx = paraIdxToRawIdx.indexOf(paraIdx);
            previousStyleRawIdx = currentRawIdx;

            currentTextItem = getSharedTextItem(currentGenericItem);

            const auto &openingStyles = currentTextItem->openStyles();
            const auto &closingStyles = currentTextItem->closeStyles();
            currentTrailPos = openingStyles.isEmpty() ? currentTrailPos : openingStyles.front().startColumn();
            currentTailPos = closingStyles.isEmpty() ? currentTailPos : closingStyles.back().endColumn();

            currentItemText = currentTextItem->text();
            currentRawText = po.rawTextData[currentRawIdx].str;

            if (styleInfo.delim.endColumn() + 1 == currentGenericItem->startColumn()) {
                currentItemText = delimText + currentItemText;
                currentRawText = delimText + currentRawText;

                auto newStartColumn = currentTextItem->startColumn() - delimText.length();
                currentTextItem->setStartColumn(newStartColumn);
                po.rawTextData[currentRawIdx].pos = newStartColumn;
                reattached = true;
            } else if (currentGenericItem->endColumn() + 1 == styleInfo.delim.startColumn()) {
                currentItemText = currentItemText + delimText;
                currentRawText = currentRawText + delimText;

                auto newEndColumn = currentTextItem->endColumn() + delimText.length();
                currentTextItem->setEndColumn(newEndColumn);
                reattached = true;
            }
            currentTextItem->setText(currentItemText);
            po.rawTextData[currentRawIdx].str = currentRawText;
        }

        MDTextItem nextTextItem = nullptr;
        if (styleInfo.paraIdx != p->items().length() - 1) {
            const long long int nextItemIdx = paraIdx + 1;
            const auto nextGenericItem = p->getItemAt(nextItemIdx);

            if (nextGenericItem->type() == MD::ItemType::Text && styleInfo.delim.endColumn() + 1 == nextGenericItem->startColumn()) {
                const long long int nextRawIdx = paraIdxToRawIdx.indexOf(nextItemIdx);
                nextTextItem = getSharedTextItem(nextGenericItem);

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

        MDTextItem previousTextItem = nullptr;
        if (styleInfo.paraIdx != 0) {
            const auto previousGenericItem = p->getItemAt(paraIdx - 1);

            if (previousGenericItem->type() == MD::ItemType::Text && previousGenericItem->startColumn() + 1 == styleInfo.delim.startColumn()) {
                const long long int previousRawIdx = paraIdxToRawIdx.indexOf(paraIdx - 1);
                previousTextItem = getSharedTextItem(previousGenericItem);

                QString previousItemText = previousTextItem->text();
                QString previousRawText = po.rawTextData[previousRawIdx].str;

                if (reattached) { // merge currentItem into previousItem
                    previousRawText = previousRawText + currentRawText;

                    previousItemText = MD::replaceEntity<MD::QStringTrait>(previousRawText.simplified());
                    previousItemText = MD::removeBackslashes<MD::QStringTrait>(previousRawText).asString();

                    previousTextItem->setEndColumn(currentGenericItem->endColumn());

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

                    auto newEndColumn = currentGenericItem->endColumn() + delimText.length();
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

            MDTextItem newTextItem = std::make_shared<MD::Text<MD::QStringTrait>>();
            newTextItem->setText(delimText);
            nextTextItem->setSpaceBefore(true);
            nextTextItem->setSpaceAfter(true);
            newTextItem->setStartLine(styleInfo.delim.startLine());
            newTextItem->setStartColumn(styleInfo.delim.startColumn());
            newTextItem->setEndLine(styleInfo.delim.endLine());
            newTextItem->setEndColumn(styleInfo.delim.endColumn());

            if (styleInfo.type == TagType::Opening) {
                if (previousTextItem) {
                    transferStyles(styleInfo, previousTextItem, newTextItem, false);
                }

                if (currentTextItem) {
                    transferStyles(styleInfo, currentTextItem, newTextItem, true);
                }

                paraIdxToRawIdx.insert(previousStyleRawIdx, paraIdx);

                po.rawTextData.insert(po.rawTextData.cbegin() + previousStyleRawIdx, newTextData);
                p->insertItem(paraIdx, newTextItem);
            } else {
                if (currentTextItem) {
                    transferStyles(styleInfo, currentTextItem, newTextItem, false);
                }

                if (nextTextItem) {
                    transferStyles(styleInfo, nextTextItem, newTextItem, true);
                }

                paraIdxToRawIdx.insert(previousStyleRawIdx + 1, paraIdx);

                po.rawTextData.insert(po.rawTextData.cbegin() + previousStyleRawIdx + 1, newTextData);
                p->insertItem(paraIdx + 1, newTextItem);
            }
        }
    }
}

void removeBadStyles(MDParagraphPtr p,
                     MDParsingOpts &po,
                     QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                     QList<StyleDelimInfo> &badStyles,
                     QList<long long int> &paraIdxToRawIdx)
{
    removeBadStylesOpts(p, openCloseStyles, badStyles);

    std::sort(badStyles.begin(), badStyles.end(), CompareByStartColumn{});

    restoreBadStyleText(p, po, badStyles, paraIdxToRawIdx);
}

void addNewStyleOpt(MDParagraphPtr p, QList<DelimInfo> &pairs, const int newStyleOpt, const int delimLength)
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

        for (long long int i = opening.paraIdx + 1; i < closing.paraIdx; i++) {
            auto item = getSharedItemWithOpts(p->getItemAt(i));
            int opts = item->opts();
            if (!(opts & newStyleOpt)) {
                item->setOpts(opts + newStyleOpt);
            }
        }
    }
}

void removeDelimText(MDParagraphPtr p,
                     MDParsingOpts &po,
                     QList<DelimInfo> &pairs,
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
        Q_ASSERT(delim.startColumn < currentTextItem->endColumn());
        while (delim.startColumn < currentTextItem->startColumn()) {
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
            continue;
        }

        if (delimRawOffSet == 0) {
            rawText.remove(0, delimLength);
            currentRawTextData.str = rawText;
            currentRawTextData.pos = currentRawTextData.pos + delimLength;

            auto text = MD::replaceEntity<MD::QStringTrait>(rawText.simplified());
            text = MD::removeBackslashes<MD::QStringTrait>(text).asString();
            currentTextItem->setText(text);
            currentTextItem->setStartColumn(currentRawTextData.pos);
            currentTextItem->setSpaceBefore(rawText[0].isSpace());
            continue;
        }

        if (rawText.length() == delimRawOffSet + delimLength) {
            rawText.chop(delimLength);
            currentRawTextData.str = rawText;

            auto text = MD::replaceEntity<MD::QStringTrait>(rawText.simplified());
            text = MD::removeBackslashes<MD::QStringTrait>(text).asString();
            currentTextItem->setText(text);
            currentTextItem->setEndColumn(currentTextItem->endColumn() - delimLength);
            currentTextItem->setSpaceAfter(rawText.back().isSpace());
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
        currentTextItem->setText(leftText);
        currentTextItem->setEndColumn(delim.startColumn - 1);
        currentTextItem->setSpaceAfter(rawText[delimRawOffSet - 1].isSpace());

        if (delim.type == TagType::Opening) {
            currentTextItem->setOpts(currentTextItem->opts() - newStyleOpt);

            const auto &openingStyle = currentTextItem->openStyles().takeLast();
            newTextItem->openStyles().append(openingStyle);
        } else {
            newTextItem->setOpts(newTextItem->opts() - newStyleOpt);
        }
    }
}

void addNewStyles(MDParagraphPtr p, MDParsingOpts &po, QList<DelimInfo> &pairs, QList<long long int> &paraIdxToRawIdx, const int delimLength)
{
    static const int TEMPOPTVALUE = 0;
    addNewStyleOpt(p, pairs, TEMPOPTVALUE, delimLength);

    std::sort(pairs.begin(), pairs.end(), CompareByStartColumn{});

    removeDelimText(p, po, pairs, paraIdxToRawIdx, TEMPOPTVALUE, delimLength);
}

void textHighlightExtension(MDParagraphPtr p, MDParsingOpts &po)
{
    if (!po.collectRefLinks) {
        if (po.rawTextData.empty())
            return;

        QList<long long int> paraIdxToRawIdx;
        QList<DelimInfo> delimInfos;
        QList<StyleDelimInfo> waitingOpeningsStyles;
        QList<QPair<StyleDelimInfo, StyleDelimInfo>> openCloseStyles;
        static const QString searchedDelim = QStringLiteral("==");

        // Can't use std::foreach, we need the index
        for (long long int i = 0; i < p->items().size(); i++) {
            getDelim(p, po, i, delimInfos, waitingOpeningsStyles, openCloseStyles, paraIdxToRawIdx, searchedDelim);
        };

        while (!delimInfos.isEmpty() && delimInfos.at(0).type == TagType::Closing) {
            delimInfos.pop_front();
        }

        if (delimInfos.isEmpty()) {
            return;
        }

        QList<StyleDelimInfo> badStyles;
        auto pairs = pairDelims(p, openCloseStyles, badStyles, delimInfos);

        addNewStyles(p, po, pairs, paraIdxToRawIdx, searchedDelim.length());

        removeBadStyles(p, po, openCloseStyles, badStyles, paraIdxToRawIdx);

        MD::optimizeParagraph(p, po);
    }
}
} // TextHighlightFunc
