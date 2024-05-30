/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "kleverconfig.h"
#include "logic/md4qt/doc.hpp"

#include <QList>
#include <QPair>
#include <QRegularExpression>
#include <qlist.h>
#include <qlogging.h>

#define MD4QT_QT_SUPPORT
#include "logic/md4qt/parser.hpp"

namespace TextHighlightFunc
{

enum TagType {
    Opening = 0,
    Closing,
    Both,
};

struct StyleDelimInfo {
    long long int paraIdx;
    MD::StyleDelim delim;
    TagType type;
};

struct DelimInfo {
    long long int paraIdx;
    long long int startColumn;
    TagType type;

    /* WARNING: temp, for debug */
    QString rawLine;
    /* ======================== */

    /* For validDelims */
    bool paired = false;
    bool badOpening = false;
};

/*
 * Get the info of 'searchedDelim' and place them inside 'delimInfos'
 * Get the already applied styles opening/closing pairs and places them inside openCloseStyles
 */
inline void getDelim(std::shared_ptr<MD::Paragraph<MD::QStringTrait>> p,
                     MD::TextParsingOpts<MD::QStringTrait> &po,
                     long long int idx,
                     QList<DelimInfo> &delimInfos,
                     QList<StyleDelimInfo> &waitingOpeningsStyles,
                     QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                     QList<long long int> &paraIdxToRawIdx,
                     const QString &searchedDelim)
{
    /* Collect the opening/closing style of the paragraph */
    auto currentItem = std::static_pointer_cast<MD::ItemWithOpts<MD::QStringTrait>>(p->getItemAt(idx));
    const auto openingStyles = currentItem->openStyles();
    const auto closingStyles = currentItem->closeStyles();

    for (const auto &styleDelim : openingStyles) {
        waitingOpeningsStyles.append({idx, styleDelim, TagType::Opening});
    }
    for (const auto &styleDelim : closingStyles) {
        openCloseStyles.append({waitingOpeningsStyles.takeLast(), {idx, styleDelim, TagType::Closing}});
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

    while (-1 < delimIdx) {
        const QChar charBeforeDelim = (localPos.first == 0 && delimIdx == 0) ? QChar() : lineInfo.first[localPos.first + delimIdx];
        if (charBeforeDelim == QChar::fromLatin1('\\')) {
            delimIdx = src.indexOf(searchedDelim, delimIdx + 2);
            continue;
        }
        const bool spaceBeforeDelim = charBeforeDelim.isSpace();
        const bool charAfterDelimExist = localPos.first + delimIdx + 3 <= lineInfo.first.length() - 1;
        const QChar charAfterDelim = !charAfterDelimExist ? QChar() : lineInfo.first[localPos.first + delimIdx + 3];
        const bool spaceAfterDelim = charAfterDelim.isSpace();

        TagType type;
        if (!spaceBeforeDelim && !spaceAfterDelim) {
            type = TagType::Both;
        } else if (!spaceBeforeDelim) {
            type = TagType::Opening;
        } else if (!spaceAfterDelim) {
            type = TagType::Closing;
        } else {
            delimIdx = src.indexOf(searchedDelim, delimIdx + 2);
            continue;
        }

        DelimInfo info = {idx, localPos.first + delimIdx, type, lineInfo.first.asString()};
        delimInfos.append(info);
        delimIdx = src.indexOf(searchedDelim, delimIdx + 2);
    }
}

/*
 * Check if the openDelim/closeDelim pairs is a valid one
 * Add 'bad' style to the DelimInfo to revert them later
 */
inline bool validDelimsPairs(std::shared_ptr<MD::Paragraph<MD::QStringTrait>> p,
                             QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                             QList<StyleDelimInfo> &badStyles,
                             DelimInfo &openDelim,
                             DelimInfo closeDelim)
{
    const auto openingItem = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(p->getItemAt(openDelim.paraIdx));
    const auto openingStyles = openingItem->openStyles();
    const auto closingStyles = openingItem->closeStyles();

    long long int i = 0;
    while (i < openCloseStyles.size()) {
        const auto stylePair = openCloseStyles[i];
        const int styleOpeningPos = stylePair.first.delim.startColumn();
        const bool openInsideStyle = styleOpeningPos < openDelim.startColumn;

        const int styleClosingPos = stylePair.second.delim.startColumn();

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

    // TODO: remove 'Both', might not be usefull, will see
    /* closeDelim.type = TagType::Opening; */
    /* closeDelim.type = TagType::Closing; */
    return true;
}

/*
 * Transform the list of delimInfo into a list containing pairs of opening/closing delimInfo
 */
inline QList<QPair<DelimInfo, DelimInfo>> makePairs(std::shared_ptr<MD::Paragraph<MD::QStringTrait>> p,
                                                    QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                                                    QList<StyleDelimInfo> &badStyles,
                                                    QList<DelimInfo> delimInfos)
{
    QList<QPair<DelimInfo, DelimInfo>> pairs;
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

                    pairs.append(qMakePair(opening, info));
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

                        pairs.append(qMakePair(opening, info));
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

inline void removeBadStylesOpts(std::shared_ptr<MD::Paragraph<MD::QStringTrait>> p,
                                QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                                QList<StyleDelimInfo> &badStyles)
{
    for (int i = 0; i < badStyles.length(); i += 2) {
        const auto &badOpening = badStyles[i];
        auto openingItem = std::static_pointer_cast<MD::ItemWithOpts<MD::QStringTrait>>(p->getItemAt(badOpening.paraIdx));
        auto &openingStyles = openingItem->openStyles();
        for (long long int styleIdx = 0; styleIdx < openingStyles.length(); i++) {
            if (openingStyles[styleIdx] == badOpening.delim) {
                openingStyles.remove(styleIdx);
                break;
            }
        }

        const auto &badClosing = badStyles[i + 1];
        auto closingItem = std::static_pointer_cast<MD::ItemWithOpts<MD::QStringTrait>>(p->getItemAt(badClosing.paraIdx));
        auto &closingStyles = closingItem->closeStyles();
        for (long long int styleIdx = 0; styleIdx < closingStyles.length(); i++) {
            if (closingStyles[styleIdx] == badClosing.delim) {
                closingStyles.remove(styleIdx);
                break;
            }
        }

        const auto style = badOpening.delim.style();

        for (long long int idx = badOpening.paraIdx; idx <= badClosing.paraIdx; idx++) {
            auto currentItem = std::static_pointer_cast<MD::ItemWithOpts<MD::QStringTrait>>(p->getItemAt(idx));

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

inline void restoreBadStyleText(std::shared_ptr<MD::Paragraph<MD::QStringTrait>> p,
                                MD::TextParsingOpts<MD::QStringTrait> &po,
                                QList<StyleDelimInfo> &badStyles,
                                QList<long long int> &paraIdxToRawIdx)
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

        const auto currentGenericItem = p->getItemAt(paraIdx);
        const QString delimText = MD::virginSubstr(po.fr, styleInfo.delim);

        QString currentItemText = {};
        QString currentRawText = {};
        long long int currentRawIdx = -1;
        bool reattached = false;

        if (currentGenericItem->type() == MD::ItemType::Text) {
            currentRawIdx = paraIdxToRawIdx.indexOf(paraIdx);
            previousStyleRawIdx = currentRawIdx;

            auto currentTextItem = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(currentGenericItem);
            currentItemText = currentTextItem->text();
            currentRawText = po.rawTextData[currentRawIdx].str;

            if (styleInfo.delim.endColumn() + 1 == currentGenericItem->startColumn()) {
                currentItemText = delimText + currentItemText;
                currentRawText = delimText + currentRawText;
                reattached = true;
            } else if (currentGenericItem->endColumn() + 1 == styleInfo.delim.startColumn()) {
                currentItemText = currentItemText + delimText;
                currentRawText = currentRawText + delimText;
                reattached = true;
            }
            currentTextItem->setText(currentItemText);
            po.rawTextData[currentRawIdx].str = currentRawText;
        }

        if (styleInfo.paraIdx != p->items().length() - 1) {
            const long long int nextItemIdx = paraIdx + 1;
            const auto nextGenericItem = p->getItemAt(nextItemIdx);

            if (nextGenericItem->type() == MD::ItemType::Text && styleInfo.delim.endColumn() + 1 == nextGenericItem->startColumn()) {
                const long long int nextRawIdx = paraIdxToRawIdx.indexOf(nextItemIdx);
                auto nextTextItem = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(nextGenericItem);
                QString nextItemText = nextTextItem->text();
                QString nextRawText = po.rawTextData[nextRawIdx].str;

                if (reattached) { // merge nextParagraph into currentItem
                    currentItemText = currentItemText + nextItemText;
                    currentRawText = currentRawText + nextRawText;

                    auto currentItem = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(currentGenericItem);
                    currentItem->setText(currentItemText);
                    po.rawTextData[currentRawIdx].str = currentRawText;

                    po.rawTextData.erase(po.rawTextData.cbegin() + nextRawIdx);
                    p->removeItemAt(nextItemIdx);
                } else {
                    nextItemText = delimText + nextItemText;
                    nextRawText = delimText + nextRawText;

                    nextTextItem->setText(nextItemText);
                    po.rawTextData[nextRawIdx].str = nextRawText;

                    reattached = true;
                }
            }
        }

        if (styleInfo.paraIdx != 0) {
            const auto previousGenericItem = p->getItemAt(paraIdx - 1);

            if (previousGenericItem->type() == MD::ItemType::Text && previousGenericItem->startColumn() + 1 == styleInfo.delim.startColumn()) {
                const long long int previousRawIdx = paraIdxToRawIdx.indexOf(paraIdx - 1);
                auto previousTextItem = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(previousGenericItem);
                QString previousItemText = previousTextItem->text();
                QString previousRawText = po.rawTextData[previousRawIdx].str;

                if (reattached) { // merge currentItem into previousItem
                    previousItemText = previousItemText + currentItemText;
                    previousRawText = previousRawText + currentRawText;

                    po.rawTextData.erase(po.rawTextData.cbegin() + currentRawIdx);
                    p->removeItemAt(paraIdx);

                    paraIdxToRawIdx.remove(currentRawIdx);
                    modifiedPreviousStyleParaIdx--;
                } else {
                    previousItemText = previousItemText + delimText;
                    previousRawText = previousRawText + delimText;

                    reattached = true;
                }

                previousTextItem->setText(previousItemText);
                po.rawTextData[previousRawIdx].str = previousRawText;
            }
        }

        if (!reattached) {
            // TODO: add pos and other things to both
            MD::TextParsingOpts<MD::QStringTrait>::TextData newTextData;
            newTextData.str = delimText;

            auto newTextItem = std::make_shared<MD::Text<MD::QStringTrait>>();
            newTextItem->setText(delimText);

            if (styleInfo.type == TagType::Opening) {
                paraIdxToRawIdx.insert(previousStyleRawIdx, paraIdx);

                po.rawTextData.insert(po.rawTextData.cbegin() + previousStyleRawIdx, newTextData);
                p->insertItem(paraIdx, newTextItem);
            } else {
                paraIdxToRawIdx.insert(previousStyleRawIdx + 1, paraIdx);

                po.rawTextData.insert(po.rawTextData.cbegin() + previousStyleRawIdx + 1, newTextData);
                p->insertItem(paraIdx + 1, newTextItem);
            }
        }
    }
}

// TODO: remove bad style opts for each between opening/closing -> Careful with nested styles
inline void removeBadStyles(std::shared_ptr<MD::Paragraph<MD::QStringTrait>> paragraphsList,
                            MD::TextParsingOpts<MD::QStringTrait> &po,
                            QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                            QList<StyleDelimInfo> &badStyles,
                            QList<long long int> &paraIdxToRawIdx)
{
    removeBadStylesOpts(paragraphsList, openCloseStyles, badStyles);

    std::sort(badStyles.begin(), badStyles.end(), [](const StyleDelimInfo &a, const StyleDelimInfo &b) {
        return a.delim.startColumn() > b.delim.startColumn();
    });

    restoreBadStyleText(paragraphsList, po, badStyles, paraIdxToRawIdx);
}

inline void textHighlightExtension(std::shared_ptr<MD::Paragraph<MD::QStringTrait>> paragraphsList, MD::TextParsingOpts<MD::QStringTrait> &po)
{
    if (!po.collectRefLinks) {
        if (po.rawTextData.empty())
            return;

        QList<long long int> paraIdxToRawIdx;
        QList<DelimInfo> delimInfos;
        QList<StyleDelimInfo> waitingOpeningsStyles;
        QList<QPair<StyleDelimInfo, StyleDelimInfo>> openCloseStyles;

        // Can't use std::foreach, we need the index
        for (long long int i = 0; i < paragraphsList->items().size(); i++) {
            getDelim(paragraphsList, po, i, delimInfos, waitingOpeningsStyles, openCloseStyles, paraIdxToRawIdx, QStringLiteral("=="));
        };

        while (!delimInfos.isEmpty() && delimInfos.at(0).type == TagType::Closing) {
            delimInfos.pop_front();
        }

        QList<StyleDelimInfo> badStyles;
        const auto pairs = makePairs(paragraphsList, openCloseStyles, badStyles, delimInfos);

        removeBadStyles(paragraphsList, po, openCloseStyles, badStyles, paraIdxToRawIdx);

        /* long long int overallIndexOffSet = 0; */
        /* while (!delimInfos.isEmpty()) { */
        /*     DelimInfo opening = delimInfos.takeFirst(); */
        /*     DelimInfo closing = delimInfos.takeFirst(); */
        /*      */
        /*     const long long int openingRawIdx = opening.rawIdx + overallIndexOffSet; */
        /*     long long int i = openingRawIdx; */
        /*     long long int closingRawIdx = closing.rawIdx + overallIndexOffSet; */
        /**/
        /*     while (0 <= i && i < closingRawIdx + 1) { */
        /*         const long long int newOffSet = processTextHighlightExtension(p, po, i, openingRawIdx, closingRawIdx, opening.delimIdx, closing.delimIdx); */
        /*         closingRawIdx += newOffSet; */
        /*         overallIndexOffSet += newOffSet; */
        /*         ++i; */
        /*     } */
        /* } */
    }
}

/* TODO rework this
inline long long int processTextHighlightExtension(
    std::shared_ptr<MD::Paragraph<MD::QStringTrait>> paragraphsList, MD::TextParsingOpts<MD::QStringTrait> &po
    , long long idx, long long leftIdx, long long rightIdx
    , int leftDelimIdx, int rightDelimIdx)
{
    auto textData = po.rawTextData[idx];
    auto lineInfo = po.fr.data.at(textData.line);
    QString src = textData.str;

    auto paragraphIdx = textAtIdx(paragraphsList, idx);
    auto currentParagraph = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraphsList->getItemAt(paragraphIdx));
    const auto opts = currentParagraph->opts() + 8;

        ==============================================================

        int idxIncrement = 0;
        if (idx != leftIdx && idx != rightIdx) {// No delim in here, we just apply the opts
            currentParagraph->setOpts(opts);
            return 0;
        } else {
            if (idx == leftIdx) {
                if (src.length() == 2) {
                    // Remove the rawTextData
                    // Update the paragraph !
                    return 0;
                }
                if (leftDelimIdx == 0 || leftDelimIdx == src.length() - 3) { // length of delim + offset
                    // We just remove the delim and the rest is unaffected
                    src.remove(leftDelimIdx, 2);
                    po.rawTextData[idx].str = src;
                    rightDelimIdx -= 2;
                } else { // The string is split in 2
                    const QString leftPart = src.mid(0, leftDelimIdx);
                    const QString rightPart = src.mid(leftDelimIdx + 2);

                    const int diff = src.length() - rightPart.length();
                    rightDelimIdx -= diff;

                    // Create new items
                    std::shared_ptr<MD::Text<MD::QStringTrait>> leftPara(new MD::Text<MD::QStringTrait>);
                    leftPara->setOpts(opts);
                    auto leftPartText = MD::replaceEntity<MD::QStringTrait>(leftPart.simplified());
                    leftPartText = MD::removeBackslashes<MD::QStringTrait>(leftPartText).asString();
                    leftPara->setText(leftPartText);
                    textData.str = leftPart;

                    paragraphsList->insertItem(paragraphIdx, leftPara);
                    po.rawTextData.insert(po.rawTextData.cbegin() + idx, textData);
                    // ============

                    src = rightPart;
                    auto rightPartText = MD::replaceEntity<MD::QStringTrait>(rightPart.simplified());
                    rightPartText = MD::removeBackslashes<MD::QStringTrait>(rightPartText).asString();
                    currentParagraph->setText(rightPartText);

                    ++idxIncrement;
                }
            }

            if (idx == rightIdx) {
                if (rightDelimIdx == 0 || rightDelimIdx == src.length() - 3) { // length of delim + offset
                    // We just remove the delim and the rest is unaffected
                    src.remove(leftDelimIdx, 2);
                    po.rawTextData[idx].str = src;
                } else {
                    const QString leftPart = src.mid(0, rightDelimIdx);
                    const QString rightPart = src.mid(rightDelimIdx + 2);

                    // Create new items
                    std::shared_ptr<MD::Text<MD::QStringTrait>> leftPara(new MD::Text<MD::QStringTrait>);
                    leftPara->setOpts(opts);
                    auto leftPartText = MD::replaceEntity<MD::QStringTrait>(leftPart.simplified());
                    leftPartText = MD::removeBackslashes<MD::QStringTrait>(leftPartText).asString();
                    leftPara->setText(leftPartText);
                    textData.str = leftPart;

                    paragraphsList->insertItem(paragraphIdx + idxIncrement, leftPara);
                    po.rawTextData.insert(po.rawTextData.cbegin() + idx + idxIncrement, textData);
                    // ============

                    src = rightPart;
                    auto rightPartText = MD::replaceEntity<MD::QStringTrait>(rightPart.simplified());
                    rightPartText = MD::removeBackslashes<MD::QStringTrait>(rightPartText).asString();
                    currentParagraph->setText(rightPartText);

                    ++idxIncrement;
                }
            }
        }
        // We don't need to increment the paragraph index
        const bool samePara = leftIdx == rightIdx;

        ==============================================================
    return 0;
}
*/
} // TextHighlightFunc
