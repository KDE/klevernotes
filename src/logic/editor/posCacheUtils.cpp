/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024-2025 Louis Schul <schul9louis@gmail.com>
*/

#include "posCacheUtils.hpp"

// KleverNotes includes.
#include "logic/parser/md4qtDataGetter.hpp"
#include "logic/parser/plugins/emoji/emojiPlugin.hpp"
#include "logic/parser/plugins/noteMapper/headerLinkingPlugin.h"
#include "logic/parser/plugins/pluginsSharedValues.h"

// C++ include.
#include <memory>

void makePairs(MD::ItemWithOpts<MD::QStringTrait> *item,
               QList<MD::WithPosition> &waitingOpeningDelims,
               QList<posCacheUtils::DelimsInfo> &openCloseDelims,
               const int headingLevel)
{
    for (const MD::WithPosition &openStyle : item->openStyles()) { // Need the cast into MD::WithPosition
        waitingOpeningDelims.append(openStyle);
    }

    for (const auto &closeStyle : item->closeStyles()) {
        const posCacheUtils::DelimsInfo pairInfo = {headingLevel, closeStyle.style(), waitingOpeningDelims.takeLast(), closeStyle};
        openCloseDelims.append(pairInfo);
    }
}

SharedItems getInnerItems(MD::Item<MD::QStringTrait> *item)
{
    switch (item->type()) {
    case MD::ItemType::Heading: {
        const auto h = static_cast<MD::Heading<MD::QStringTrait> *>(item);
        return h->text().get()->items();
    }
    case MD::ItemType::Paragraph: {
        return static_cast<MD::Paragraph<MD::QStringTrait> *>(item)->items();
    }
    case MD::ItemType::Blockquote:
    case MD::ItemType::List:
    case MD::ItemType::ListItem: {
        // Those block are "useless" in this case, but no need for an error message,
        // we already know that they're not supported, not a bug
        break;
    }
    default:
        qWarning() << "getInnerItems: Unsupported block item" << static_cast<int>(item->type());
    }
    return {};
}

void addSurroundingDelimsPairs(QList<posCacheUtils::DelimsInfo> &delims,
                               QList<MD::WithPosition> &waitingOpeningDelims,
                               QList<posCacheUtils::DelimsInfo> &openCloseDelims,
                               const MD::WithPosition &cursorPos,
                               const MD::WithPosition &selectStartPos,
                               const MD::WithPosition &selectEndPos)
{
    if (!waitingOpeningDelims.isEmpty()) {
        qWarning() << "Error in processing open/close style for unhighlight, remaining open delims:";
        for (const auto &delim : waitingOpeningDelims) {
            qWarning() << delim.startColumn() << delim.startLine() << delim.endColumn() << delim.endLine();
        }
        return;
    }

    for (const auto &delimInfo : openCloseDelims) {
        const auto &openDelim = delimInfo.opening;
        const auto &closeDelim = delimInfo.closing;

        bool addPair = true;
        if (selectStartPos.startColumn() != -1 && selectEndPos.startColumn() != -1) {
            addPair = md4qtHelperFunc::isBetweenDelims(cursorPos, openDelim, closeDelim, true)
                || md4qtHelperFunc::isBetweenDelims(openDelim, selectStartPos, selectEndPos)
                || md4qtHelperFunc::isBetweenDelims(closeDelim, selectStartPos, selectEndPos);
        }

        if (addPair && !delims.contains(delimInfo)) {
            delims.append(delimInfo);
        }
    }
}

void addCustomSurroundingDelims(QList<posCacheUtils::DelimsInfo> &delims,
                                ItemSharedPointer item,
                                QList<MD::WithPosition> &waitingOpeningDelims,
                                QList<posCacheUtils::DelimsInfo> &openCloseDelims,
                                const MD::WithPosition &cursorPos,
                                const MD::WithPosition &selectStartPos,
                                const MD::WithPosition &selectEndPos,
                                const int headingLevel)
{
    const int itemType = static_cast<int>(item->type());
    switch (itemType) {
    case PluginsSharedValues::CustomType::Emoji: {
        const auto emojiItem = static_cast<EmojiPlugin::EmojiItem *>(item.get());
        const posCacheUtils::DelimsInfo outerDelims = {headingLevel, 0, emojiItem->startDelim(), emojiItem->endDelim()};
        openCloseDelims.append(outerDelims);

        makePairs(emojiItem, waitingOpeningDelims, openCloseDelims, headingLevel);
        addSurroundingDelimsPairs(delims, waitingOpeningDelims, openCloseDelims, cursorPos, selectStartPos, selectEndPos);
        break;
    }
    case PluginsSharedValues::CustomType::HeaderLinking: {
        const auto header = static_cast<HeaderLinkingPlugin::HeaderLinkingItem *>(item.get());

        const int delimType = (posCacheUtils::BlockDelimTypes::Heading1 - 1) + header->level();
        delims.append({header->level(), delimType, header->delim()});
        break;
    }
    default: {
        qWarning() << "Item not handled" << static_cast<int>(item->type());
        return;
    }
    }
}

void addSurroundingDelims(QList<posCacheUtils::DelimsInfo> &delims,
                          MD::Item<MD::QStringTrait> *baseItem,
                          const MD::WithPosition &cursorPos,
                          const MD::WithPosition &selectStartPos,
                          const MD::WithPosition &selectEndPos,
                          const int headingLevel)
{
    auto items = getInnerItems(baseItem);

    QList<MD::WithPosition> waitingOpeningDelims;
    QList<posCacheUtils::DelimsInfo> openCloseDelims;

    for (const auto &item : items) {
        switch (item->type()) {
        case MD::ItemType::Text:
        case MD::ItemType::Link:
        case MD::ItemType::Image: {
            makePairs(static_cast<MD::ItemWithOpts<MD::QStringTrait> *>(item.get()), waitingOpeningDelims, openCloseDelims, headingLevel);
            addSurroundingDelimsPairs(delims, waitingOpeningDelims, openCloseDelims, cursorPos, selectStartPos, selectEndPos);
            break;
        }
        case MD::ItemType::Code: {
            const auto codeItem = static_cast<MD::Code<MD::QStringTrait> *>(item.get());
            const posCacheUtils::DelimsInfo outerDelims = {headingLevel, 0, codeItem->startDelim(), codeItem->endDelim()};
            openCloseDelims.append(outerDelims);

            makePairs(codeItem, waitingOpeningDelims, openCloseDelims, headingLevel);
            addSurroundingDelimsPairs(delims, waitingOpeningDelims, openCloseDelims, cursorPos, selectStartPos, selectEndPos);
            break;
        }
        case MD::ItemType::RawHtml:
        case MD::ItemType::LineBreak: {
            // "useless" in this case, but no need for an error message,
            // we already it is not supported, not a bug
            break;
        }
        default:
            addCustomSurroundingDelims(delims, item, waitingOpeningDelims, openCloseDelims, cursorPos, selectStartPos, selectEndPos, headingLevel);
        }
    }
}

void addHeadingDelims(QList<posCacheUtils::DelimsInfo> &delims, MD::Item<MD::QStringTrait> *item, int &headingLevel)
{
    const auto &h = static_cast<MD::Heading<MD::QStringTrait> *>(item);
    headingLevel = h->level();

    for (const auto &delim : h->delims()) {
        const int delimType = (posCacheUtils::BlockDelimTypes::Heading1 - 1) + headingLevel;

        const posCacheUtils::DelimsInfo delimInfo = {headingLevel, delimType, delim};
        if (!delims.contains(delimInfo)) {
            delims.append(delimInfo);
        }
    }
}

void addBlockItemDelims(QList<posCacheUtils::DelimsInfo> &delims, MD::Item<MD::QStringTrait> *item, const MD::WithPosition &pos, int &headingLevel)
{
    posCacheUtils::DelimsInfo delimInfo;
    switch (item->type()) {
    case MD::ItemType::Heading: { // Happens when hovering a heading delim
        int headingLevel = 0;
        addHeadingDelims(delims, item, headingLevel);
        return;
    }
    case MD::ItemType::Code: {
        const auto codeItem = static_cast<MD::Code<MD::QStringTrait> *>(item);
        if (codeItem->isFensedCode()) {
            delimInfo = {0, posCacheUtils::BlockDelimTypes::CodeBlock, codeItem->startDelim(), codeItem->endDelim()};
        }
        break;
    }
    case MD::ItemType::Blockquote: {
        const auto quoteItem = static_cast<MD::Blockquote<MD::QStringTrait> *>(item);
        for (const auto &delim : quoteItem->delims()) {
            if (delim.startLine() == pos.startLine()) {
                delimInfo = {headingLevel, posCacheUtils::BlockDelimTypes::BlockQuote, delim};
                break;
            }
        }
        break;
    }
    case MD::ItemType::ListItem: {
        const auto listItem = static_cast<MD::ListItem<MD::QStringTrait> *>(item);
        int delimType = listItem->listType() == MD::ListItem<MD::QStringTrait>::ListType::Ordered ? posCacheUtils::BlockDelimTypes::OrderedList
                                                                                                  : posCacheUtils::BlockDelimTypes::UnorderedList;
        delimInfo = {headingLevel, delimType, listItem->delim()};
        break;
    }
    case MD::ItemType::List:
    case MD::ItemType::Paragraph:
    case MD::ItemType::HorizontalLine:
    case MD::ItemType::Table:
    case MD::ItemType::RawHtml: {
        // Those block are "useless" in this case, but no need for an error message,
        // we already know that they're not supported, not a bug
        break;
    }
    default:
        qWarning() << "addBlockItemDelims: Unsupported block item" << static_cast<int>(item->type());
    }

    if (delimInfo.opening.startLine() != -1 && !delims.contains(delimInfo)) {
        delims.append(delimInfo);
    }
}

namespace posCacheUtils
{
bool operator==(const DelimsInfo &d1, const DelimsInfo &d2)
{
    // We don't check for the heading level since the real deal here is the delims
    return (d1.opening == d2.opening && d1.closing == d2.closing);
}

void addDelimsFromItems(QList<posCacheUtils::DelimsInfo> &delims,
                        const Items &items,
                        const MD::WithPosition &pos,
                        const MD::WithPosition &selectStartPos,
                        const MD::WithPosition &selectEndPos)
{
    int headingLevel = 0;
    const int cacheLen = items.length();
    MD::Item<MD::QStringTrait> *blockItem = 3 <= cacheLen ? items.at(cacheLen - 3) : items.first();

    addBlockItemDelims(delims, blockItem, pos, headingLevel);

    if (2 <= cacheLen) {
        const auto &secondToLast = items.at(cacheLen - 2);

        addSurroundingDelims(delims, secondToLast, pos, selectStartPos, selectEndPos, headingLevel);
    }
}
}
