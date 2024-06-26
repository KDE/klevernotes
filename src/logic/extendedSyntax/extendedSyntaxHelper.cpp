/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "extendedSyntaxHelper.hpp"

namespace ExtendedSyntaxHelper
{
// Style manip
void mergeStyle(MDItemWithOptsPtr a, MDItemWithOptsPtr b)
{
    a->openStyles() << b->openStyles();
    a->closeStyles() << b->closeStyles();

    std::sort(a->openStyles().begin(), a->openStyles().end(), md4qtHelperFunc::StartColumnOrder{});
    std::sort(a->closeStyles().begin(), a->closeStyles().end(), md4qtHelperFunc::StartColumnOrder{});

    long long int opts = 0;
    for (const auto &styleDelim : a->openStyles()) {
        int style = styleDelim.style();
        if (!(opts & style)) {
            opts += style;
        }
    }
    for (const auto &styleDelim : a->openStyles()) {
        int style = styleDelim.style();
        if (!(opts & style)) {
            opts += style;
        }
    }
    a->setOpts(opts);
}

void transferStyle(MDItemWithOptsPtr a, MDItemWithOptsPtr b, const bool transferClose)
{
    if (a->startColumn() == b->startColumn()) {
        return;
    }
    if (transferClose) {
        auto closingStyles = a->closeStyles();
        const long long int len = closingStyles.length();
        long long int idx = len;
        for (; 0 < idx; --idx) {
            if (closingStyles[idx - 1].endColumn() < b->startColumn()) {
                break;
            }
        }
        a->closeStyles() = closingStyles.mid(0, idx);
        b->closeStyles() << closingStyles.mid(idx);
        std::sort(b->closeStyles().begin(), b->closeStyles().end(), md4qtHelperFunc::StartColumnOrder{});

        long long int opts = b->opts();
        for (const auto &styleDelim : b->closeStyles()) {
            int style = styleDelim.style();
            if (!(opts & style)) {
                opts += style;
            }
        }
        b->setOpts(opts);
    } else {
        auto openingStyles = b->openStyles();
        long long int idx = 0;
        for (; idx < openingStyles.length(); ++idx) {
            if (a->endColumn() < openingStyles[idx].startColumn()) {
                break;
            }
        }
        a->openStyles() << openingStyles.mid(0, idx);
        std::sort(a->openStyles().begin(), a->openStyles().end(), md4qtHelperFunc::StartColumnOrder{});
        b->openStyles() = openingStyles.mid(idx);

        long long int opts = a->opts();
        for (const auto &styleDelim : a->openStyles()) {
            int style = styleDelim.style();
            if (!(opts & style)) {
                opts += style;
            }
        }
        a->setOpts(opts);
    }
}
// !Style manip

// String manip
bool addStringTo(const MDItemWithOptsPtr item, const bool atStart, const QString &str, MDParsingOpts &po)
{
    if (item->type() != MD::ItemType::Text) {
        return false;
    }
    const long long int rawIdx = md4qtHelperFunc::rawIdxFromItem(item, po);
    Q_ASSERT(rawIdx != -1);

    MDTextItemPtr textItem = md4qtHelperFunc::getSharedTextItem(item);
    auto rawData = po.rawTextData[rawIdx];

    const QString text = rawData.str;
    QString newText;
    if (atStart) {
        newText = str + text;
        const long long int newStart = textItem->startColumn() - str.length();
        textItem->setStartColumn(newStart);
        rawData.pos = newStart;
    } else {
        newText = text + str;
        textItem->setEndColumn(textItem->endColumn() + str.length());
    }
    rawData.str = newText;

    auto newSimplifiedText = MD::replaceEntity<MD::QStringTrait>(newText.simplified());
    newSimplifiedText = MD::removeBackslashes<MD::QStringTrait>(newSimplifiedText).asString();
    textItem->setText(newSimplifiedText);

    po.rawTextData[rawIdx] = rawData;

    return true;
}

void mergeFromIndex(const long long int from, MDParagraphPtr p, MDParsingOpts &po)
{
    MDItemWithOptsPtr currentItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(from));
    MDItemWithOptsPtr nextItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(from + 1));

    if (currentItem->type() != MD::ItemType::Text || nextItem->type() != MD::ItemType::Text) {
        return;
    }

    MDTextItemPtr currentTextItem = md4qtHelperFunc::getSharedTextItem(currentItem);
    MDTextItemPtr nextTextItem = md4qtHelperFunc::getSharedTextItem(nextItem);

    // Merge styles
    mergeStyle(currentTextItem, nextTextItem);

    // Merge text
    const long long int currentRawIdx = md4qtHelperFunc::rawIdxFromItem(currentItem, po);
    Q_ASSERT(currentRawIdx < static_cast<long long int>(po.rawTextData.size()) - 1);

    auto currentRawData = po.rawTextData[currentRawIdx];
    auto nextRawData = po.rawTextData[currentRawIdx + 1];

    const QString finalText = currentRawData.str + nextRawData.str;
    currentRawData.str = finalText;

    auto finalSimplifiedText = MD::replaceEntity<MD::QStringTrait>(finalText.simplified());
    finalSimplifiedText = MD::removeBackslashes<MD::QStringTrait>(finalSimplifiedText).asString();
    currentTextItem->setText(finalSimplifiedText);

    // Others var
    currentTextItem->setEndColumn(nextTextItem->endColumn());

    // Set rawData back
    po.rawTextData[currentRawIdx] = currentRawData;

    // Clean
    po.rawTextData.erase(po.rawTextData.cbegin() + from + 1);
    p->removeItemAt(from + 1);
}

void splitItem(MDParagraphPtr p,
               MDParsingOpts &po,
               long long int paraIdx,
               const long long int from,
               const long long int length,
               const bool opening,
               const long long int newStyleOpt)
{
    MDItemWithOptsPtr currentItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx));

    if (currentItem->type() != MD::ItemType::Text) {
        return;
    }

    const long long int rawIdx = md4qtHelperFunc::rawIdxFromItem(currentItem, po);
    Q_ASSERT(rawIdx != -1);

    auto rawData = po.rawTextData[rawIdx];
    MDTextItemPtr textItem = md4qtHelperFunc::getSharedTextItem(currentItem);

    const long long int startPos = rawData.pos;
    const long long int endPos = textItem->endColumn();

    const long long int relativeStartSplit = from - startPos;
    const long long int relativeEndSplit = relativeStartSplit + length - 1;
    const long long int endSplit = startPos + relativeEndSplit;

    MD::StyleDelim styleDelim = MD::StyleDelim(newStyleOpt, from, currentItem->startLine(), (from + length - 1), currentItem->endLine());

    const QString src = rawData.str;
    const bool removeCurrent = length == rawData.str.length();
    bool newAdded = false;
    if (!removeCurrent && from != startPos && endSplit != endPos) {
        const QString leftRawText = src.mid(0, relativeStartSplit);
        rawData.str = leftRawText;
        po.rawTextData[rawIdx] = rawData;

        auto simplifiedLeftText = MD::replaceEntity<MD::QStringTrait>(leftRawText.simplified());
        simplifiedLeftText = MD::removeBackslashes<MD::QStringTrait>(simplifiedLeftText).asString();
        textItem->setText(simplifiedLeftText);
        textItem->setEndColumn(from - 1);

        // Add new things
        const long long int newStartingPos = endSplit + 1;
        const QString rightRawText = src.mid(relativeEndSplit + 1);
        MDParsingOpts::TextData newTextData;
        newTextData.str = rightRawText;
        newTextData.pos = newStartingPos;
        newTextData.line = rawData.line;

        auto newTextItem = std::make_shared<MD::Text<MD::QStringTrait>>();
        newTextItem->setStartLine(textItem->startLine());
        newTextItem->setEndLine(textItem->endLine());
        newTextItem->setStartColumn(newStartingPos);
        newTextItem->setEndColumn(newStartingPos + rightRawText.length() - 1);

        auto finalSimplifiedText = MD::replaceEntity<MD::QStringTrait>(rightRawText.simplified());
        finalSimplifiedText = MD::removeBackslashes<MD::QStringTrait>(finalSimplifiedText).asString();
        newTextItem->setText(finalSimplifiedText);

        po.rawTextData.insert(po.rawTextData.cbegin() + rawIdx + 1, newTextData);
        p->insertItem(paraIdx + 1, newTextItem);

        newAdded = true;
    } else if (!removeCurrent) {
        QString newText;
        if (from == startPos) {
            newText = src.mid(length);

            rawData.pos += length;
            textItem->setStartColumn(rawData.pos);
        }

        if (endSplit == endPos) {
            newText = src.chopped(length);

            textItem->setEndColumn(textItem->endColumn() - length);
        }

        rawData.str = newText;
        po.rawTextData[rawIdx] = rawData;

        auto finalSimplifiedText = MD::replaceEntity<MD::QStringTrait>(newText.simplified());
        finalSimplifiedText = MD::removeBackslashes<MD::QStringTrait>(finalSimplifiedText).asString();
        textItem->setText(finalSimplifiedText);
    }

    // Add new styles
    if (opening) {
        MDItemWithOptsPtr item = (newAdded || endSplit == endPos) ? md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx + 1))
                                                                  : md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx));

        item->openStyles() << styleDelim;
        std::sort(item->openStyles().begin(), item->openStyles().end(), md4qtHelperFunc::StartColumnOrder{});
    } else {
        MDItemWithOptsPtr item = (from == startPos) ? md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx - 1))
                                                    : md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx));
        item->closeStyles() << styleDelim;
        std::sort(item->closeStyles().begin(), item->closeStyles().end(), md4qtHelperFunc::StartColumnOrder{});
    }

    // Transfer styles
    if (newAdded) {
        MDItemWithOptsPtr nextItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx + 1));
        transferStyle(currentItem, nextItem, true);
    }

    // Clean
    if (removeCurrent) {
        p->removeItemAt(paraIdx);
        po.rawTextData.erase(po.rawTextData.cbegin() + rawIdx);
    }
}
// !String manip
} // !ExtendedSyntaxHelper
