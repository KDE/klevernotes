/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "posCacheUtils.hpp"

// KleverNotes includes.
#include "logic/parser/md4qtDataGetter.hpp"
#include "logic/parser/plugins/emoji/emojiPlugin.hpp"

// C++ include.
#include <algorithm>
#include <memory>

static const int USERDEFINEDINT = static_cast<int>(MD::ItemType::UserDefined);

void makePairs(MD::ItemWithOpts<MD::QStringTrait> *item,
               QList<MD::WithPosition> &waitingOpeningDelims,
               QList<std::pair<MD::WithPosition, MD::WithPosition>> &openCloseDelims)
{
    for (const MD::WithPosition &openStyle : item->openStyles()) { // Need the cast into MD::WithPosition
        waitingOpeningDelims.append(openStyle);
    }

    for (const MD::WithPosition &closeStyle : item->closeStyles()) {
        std::pair<MD::WithPosition, MD::WithPosition> newPair = std::make_pair(waitingOpeningDelims.takeLast(), closeStyle);
        openCloseDelims.append(newPair);
    }
}

void getOpenCloseDelims(MD::Item<MD::QStringTrait> *item,
                        QList<MD::WithPosition> &waitingOpeningDelims,
                        QList<std::pair<MD::WithPosition, MD::WithPosition>> &openCloseDelims)
{
    switch (item->type()) {
    case MD::ItemType::Text:
    case MD::ItemType::Link:
    case MD::ItemType::Image: {
        const auto itemWithOpts = static_cast<MD::ItemWithOpts<MD::QStringTrait> *>(item);
        return makePairs(itemWithOpts, waitingOpeningDelims, openCloseDelims);
    }
    case MD::ItemType::Code: {
        const auto codeItem = static_cast<MD::Code<MD::QStringTrait> *>(item);
        openCloseDelims.append({codeItem->startDelim(), codeItem->endDelim()});
        return makePairs(codeItem, waitingOpeningDelims, openCloseDelims);
    }
    case MD::ItemType::LineBreak: {
        // "useless" in this case, but no need for an error message,
        // we already it is not supported, not a bug
        break;
    }
    default:
        // Find a better way to do this with futur user defined
        const int itemType = static_cast<int>(item->type());

        if (itemType == USERDEFINEDINT + 1) {
            const auto emojiItem = static_cast<EmojiPlugin::EmojiItem *>(item);
            openCloseDelims.append({emojiItem->startDelim(), emojiItem->endDelim()});
            return makePairs(emojiItem, waitingOpeningDelims, openCloseDelims);
        } else {
            qWarning() << "Item not handled" << static_cast<int>(item->type());
            return;
        }
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

void addHeadingDelims(QList<posCacheUtils::DelimsInfo> &delims, MD::Item<MD::QStringTrait> *item, int &headingLevel)
{
    const auto &h = static_cast<MD::Heading<MD::QStringTrait> *>(item);
    headingLevel = h->level();

    for (const auto &delim : h->delims()) {
        const posCacheUtils::DelimsInfo delimInfo = {headingLevel, delim};
        if (!delims.contains(delimInfo)) {
            delims.append(delimInfo);
        }
    }
}

void addSurroundingDelimsPairs(QList<posCacheUtils::DelimsInfo> &delims,
                               MD::Item<MD::QStringTrait> *item,
                               const MD::WithPosition &cursorPos,
                               const MD::WithPosition &selectStartPos,
                               const MD::WithPosition &selectEndPos,
                               const int headingLevel)
{
    auto items = getInnerItems(item);

    QList<MD::WithPosition> waitingOpeningDelims;
    QList<std::pair<MD::WithPosition, MD::WithPosition>> openCloseDelims;

    for (const auto &item : items) {
        getOpenCloseDelims(item.get(), waitingOpeningDelims, openCloseDelims);
    }

    if (!waitingOpeningDelims.isEmpty()) {
        qWarning() << "Error in processing open/close style for unhighlight, remaining open delims:";
        for (const auto &delim : waitingOpeningDelims) {
            qWarning() << delim.startColumn() << delim.startLine() << delim.endColumn() << delim.endLine();
        }
        return;
    }

    for (const auto &pair : openCloseDelims) {
        const auto &openDelim = pair.first;
        const auto &closeDelim = pair.second;

        bool addPair = md4qtHelperFunc::isBetweenDelims(cursorPos, openDelim, closeDelim, true);
        if (!addPair && selectStartPos.startColumn() != -1 && selectEndPos.startColumn() != -1) {
            addPair = md4qtHelperFunc::isBetweenDelims(openDelim, selectStartPos, selectEndPos)
                || md4qtHelperFunc::isBetweenDelims(closeDelim, selectStartPos, selectEndPos);
        }

        if (addPair) {
            const posCacheUtils::DelimsInfo delimInfo = {headingLevel, openDelim, closeDelim};
            if (!delims.contains(delimInfo)) {
                delims.append(delimInfo);
            }
        }
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
    const auto &first = items.first();
    const int cacheLen = items.length();
    if (2 <= cacheLen) {
        const auto &secondToLast = items.at(cacheLen - 2);

        int headingLevel = 0;
        if (first->type() == MD::ItemType::Heading) {
            addHeadingDelims(delims, first, headingLevel);
        }

        addSurroundingDelimsPairs(delims, secondToLast, pos, selectStartPos, selectEndPos, headingLevel);

        return;
    }

    switch (first->type()) {
    case MD::ItemType::Heading: {
        int headingLevel = 0;
        addHeadingDelims(delims, first, headingLevel);
        return;
    }
    case MD::ItemType::Code: {
        const auto codeItem = static_cast<MD::Code<MD::QStringTrait> *>(first);
        if (codeItem->isFensedCode()) {
            const posCacheUtils::DelimsInfo delimInfo = {0, codeItem->startDelim(), codeItem->endDelim()};
            if (!delims.contains(delimInfo)) {
                delims.append(delimInfo);
            }
        }
        return;
    }
    case MD::ItemType::Paragraph:
    case MD::ItemType::Blockquote:
    case MD::ItemType::List:
    case MD::ItemType::ListItem:
    case MD::ItemType::HorizontalLine:
    case MD::ItemType::RawHtml: {
        // Those block are "useless" in this case, but no need for an error message,
        // we already know that they're not supported, not a bug
        break;
    }
    default:
        qWarning() << "revertDelimsStyle: Unsupported block item" << static_cast<int>(first->type());
    }
}
}