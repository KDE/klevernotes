/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "textHighlightExtension.hpp"

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
inline std::shared_ptr<MD::Text<MD::QStringTrait>> getSharedTextItem(const std::shared_ptr<T> &item)
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

        DelimInfo info = {idx, localPos.first + delimIdx, localPos.second, type, lineInfo.first.asString()};
        delimInfos.append(info);
        delimIdx = src.indexOf(searchedDelim, delimIdx + 2);
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

        for (long long int styleIdx = 0; styleIdx < openingStyles.length(); i++) {
            if (openingStyles[styleIdx] == badOpening.delim) {
                openingStyles.remove(styleIdx);
                break;
            }
        }

        const auto &badClosing = badStyles[i + 1];
        auto closingItem = getSharedItemWithOpts(p->getItemAt(badClosing.paraIdx));
        auto &closingStyles = closingItem->closeStyles();

        for (long long int styleIdx = 0; styleIdx < closingStyles.length(); i++) {
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

        const auto currentGenericItem = p->getItemAt(paraIdx);
        const QString delimText = MD::virginSubstr(po.fr, styleInfo.delim);

        QString currentItemText = {};
        QString currentRawText = {};
        long long int currentRawIdx = -1;
        bool reattached = false;

        if (currentGenericItem->type() == MD::ItemType::Text) {
            currentRawIdx = paraIdxToRawIdx.indexOf(paraIdx);
            previousStyleRawIdx = currentRawIdx;

            auto currentTextItem = getSharedTextItem(currentGenericItem);
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
                auto nextTextItem = getSharedTextItem(nextGenericItem);
                QString nextItemText = nextTextItem->text();
                QString nextRawText = po.rawTextData[nextRawIdx].str;

                if (reattached) { // merge nextTextItem into currentItem
                    currentItemText = currentItemText + nextItemText;
                    currentRawText = currentRawText + nextRawText;

                    auto currentItem = getSharedTextItem(currentGenericItem);
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
                auto previousTextItem = getSharedTextItem(previousGenericItem);
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
            MDParsingOpts::TextData newTextData;
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

void addNewStyles(MDParagraphPtr p, MDParsingOpts &po, QList<DelimInfo> delimInfos, QList<long long int> &paraIdxToRawIdx)
{
    Q_UNUSED(p);
    Q_UNUSED(paraIdxToRawIdx);
    for (const auto &delim : delimInfos) {
        const auto localPos = MD::localPosFromVirgin(po.fr, delim.startColumn, delim.startLine);
        const auto lineInfo = po.fr.data.at(localPos.second);
    }
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

        // Can't use std::foreach, we need the index
        for (long long int i = 0; i < p->items().size(); i++) {
            getDelim(p, po, i, delimInfos, waitingOpeningsStyles, openCloseStyles, paraIdxToRawIdx, QStringLiteral("=="));
        };

        while (!delimInfos.isEmpty() && delimInfos.at(0).type == TagType::Closing) {
            delimInfos.pop_front();
        }

        QList<StyleDelimInfo> badStyles;
        auto pairs = pairDelims(p, openCloseStyles, badStyles, delimInfos);

        removeBadStyles(p, po, openCloseStyles, badStyles, paraIdxToRawIdx);

        std::sort(pairs.begin(), pairs.end(), CompareByStartColumn{});

        for (const auto &delim : pairs) {
            qDebug() << delim.startColumn;
        }
        qDebug() << "\n";
    }
}
} // TextHighlightFunc
