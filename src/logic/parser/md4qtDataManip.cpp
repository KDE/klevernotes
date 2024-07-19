/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "md4qtDataManip.hpp"

namespace md4qtHelperFunc
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
        const auto aEnd = a->endColumn();
        const auto bEnd = b->endColumn();
        MD::QStringTrait::Vector<MD::StyleDelim> aNewCloses;
        MD::QStringTrait::Vector<MD::StyleDelim> bNewCloses;

        auto closingStyles = a->closeStyles();
        while (closingStyles.length()) {
            const auto &closingStyle = closingStyles.takeLast();
            const auto delimStartPos = closingStyle.startColumn();
            const auto aDelta = delimStartPos - aEnd;
            const auto bDelta = delimStartPos - bEnd;

            if (bDelta < 0) {
                aNewCloses.append(closingStyle);
                continue;
            }
            if (aDelta < 0) {
                bNewCloses.append(closingStyle);
                break;
            }
            if (bDelta < aDelta) {
                bNewCloses.append(closingStyle);
            } else {
                aNewCloses.append(closingStyle);
            }
        }
        a->closeStyles() = aNewCloses;
        b->closeStyles() = bNewCloses;
        std::sort(a->closeStyles().begin(), a->closeStyles().end(), md4qtHelperFunc::StartColumnOrder{});
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
        const auto aStart = a->startColumn();
        const auto bStart = b->startColumn();
        MD::QStringTrait::Vector<MD::StyleDelim> aNewOpens;
        MD::QStringTrait::Vector<MD::StyleDelim> bNewOpens;

        auto openingStyles = a->openStyles();
        while (openingStyles.length()) {
            const auto &openingStyle = openingStyles.takeLast();
            const auto delimEndPos = openingStyle.endColumn();
            const auto aDelta = aStart - delimEndPos;
            const auto bDelta = bStart - delimEndPos;

            if (bDelta < 0) {
                aNewOpens.append(openingStyle);
                continue;
            }
            if (aDelta < 0) {
                bNewOpens.append(openingStyle);
                break;
            }
            if (bDelta < aDelta) {
                bNewOpens.append(openingStyle);
            } else {
                aNewOpens.append(openingStyle);
            }
        }
        a->openStyles() = aNewOpens;
        b->openStyles() = bNewOpens;
        std::sort(a->openStyles().begin(), a->openStyles().end(), md4qtHelperFunc::StartColumnOrder{});
        std::sort(b->openStyles().begin(), b->openStyles().end(), md4qtHelperFunc::StartColumnOrder{});

        long long int opts = b->opts();
        for (const auto &styleDelim : b->openStyles()) {
            int style = styleDelim.style();
            if (!(opts & style)) {
                opts += style;
            }
        }
        b->setOpts(opts);
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
    po.rawTextData.erase(po.rawTextData.cbegin() + currentRawIdx + 1);
    p->removeItemAt(from + 1);
}

int splitItem(MDParagraphPtr p,
              MDParsingOpts &po,
              const long long int paraIdx,
              const long long int rawIdx,
              const long long int from,
              const long long int length)
{
    MDItemWithOptsPtr currentItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx));

    if (currentItem->type() != MD::ItemType::Text) {
        return 0;
    }

    // Item info
    MDTextItemPtr textItem = md4qtHelperFunc::getSharedTextItem(currentItem);
    const long long int virginStartPos = textItem->startColumn();
    const long long int virginEndPos = textItem->endColumn();

    // Raw Data info
    auto rawData = po.rawTextData[rawIdx];
    const long long int localStartPos = rawData.pos;
    const QString src = rawData.str;

    // Relative to the rawData.str
    const long long int virginToLocalDelta = virginStartPos - localStartPos;
    const long long int relativeStartSplit = from - virginToLocalDelta - localStartPos;
    const long long int relativeEndSplit = relativeStartSplit + length - 1;

    const long long int virginEndSplit = virginStartPos + relativeEndSplit;

    // Item == delim
    if (length == rawData.str.length()) {
        return -1;
    }

    // Item split in half
    if (from != virginStartPos && virginEndSplit != virginEndPos) {
        const QString leftRawText = src.mid(0, relativeStartSplit);
        rawData.str = leftRawText;
        po.rawTextData[rawIdx] = rawData;

        auto simplifiedLeftText = MD::replaceEntity<MD::QStringTrait>(leftRawText.simplified());
        simplifiedLeftText = MD::removeBackslashes<MD::QStringTrait>(simplifiedLeftText).asString();
        textItem->setText(simplifiedLeftText);
        textItem->setEndColumn(from - 1);

        // Add new things
        const long long int newStartingPos = virginEndSplit + 1;
        const QString rightRawText = src.mid(relativeEndSplit + 1);
        MDParsingOpts::TextData newTextData;
        newTextData.str = rightRawText;
        newTextData.pos = newStartingPos - virginToLocalDelta;
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

        return 1;
    }

    QString newText;
    if (from == virginStartPos) { // delim at the start
        newText = src.mid(length);
        rawData.pos += length;
        textItem->setStartColumn(currentItem->startColumn() + length);
    }

    if (virginEndSplit == virginEndPos) { // delim at the end
        newText = src.chopped(length);
        textItem->setEndColumn(textItem->endColumn() - length);
    }

    rawData.str = newText;
    po.rawTextData[rawIdx] = rawData;

    auto finalSimplifiedText = MD::replaceEntity<MD::QStringTrait>(newText.simplified());
    finalSimplifiedText = MD::removeBackslashes<MD::QStringTrait>(finalSimplifiedText).asString();
    textItem->setText(finalSimplifiedText);

    return 0;
}
// !String manip
} // md4qtHelperFunc!
