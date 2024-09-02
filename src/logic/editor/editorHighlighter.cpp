// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "editorHighlighter.hpp"

// KleverNotes includes
#include "logic/editor/editorHandler.hpp"

// md-editor include.
#include "logic/parser/md4qt/doc.hpp"
#include "logic/parser/md4qt/traits.hpp"
#include "logic/parser/md4qtDataGetter.hpp"

// Qt include.
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextEdit>

// C++ include.
#include <algorithm>
#include <memory>
#include <qlogging.h>

using namespace Qt::Literals::StringLiterals;

static const int USERDEFINEDINT = static_cast<int>(MD::ItemType::UserDefined);

namespace MdEditor
{
// EditorHighlighter
EditorHighlighter::EditorHighlighter(EditorHandler *editor)
    : d(new EditorHighlighterPrivate(editor))
{
}

EditorHighlighter::~EditorHighlighter()
{
}

void EditorHighlighter::setFont(const QFont &f)
{
    d->font = f;
}

void EditorHighlighter::clearHighlighting()
{
    d->clearFormats();
}

void EditorHighlighter::highlight(std::shared_ptr<MD::Document<MD::QStringTrait>> doc, const Colors &colors)
{
    auto c = d->editor->textCursor();
    c.beginEditBlock();
    d->clearFormats();

    d->doc = doc;
    d->colors = colors;

    MD::PosCache<MD::QStringTrait>::initialize(d->doc);
    c.endEditBlock();
    showDelimAroundCursor();
}

void EditorHighlighter::addExtendedSyntax(const long long int opts, const QStringList &info)
{
    d->modifications[opts] = info;
}

void EditorHighlighter::changeAdaptiveTagSize(const bool adaptive)
{
    d->adaptiveTagSize = adaptive;
}

void EditorHighlighter::changeTagScale(const int tagSizeScale)
{
    if (tagSizeScale < 1 || 100 < tagSizeScale) {
        return;
    }
    d->tagSizeScale = tagSizeScale;
}

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
    default:
        // Find a better way to do this with futur user defined
        const int itemType = static_cast<int>(item->type());

        if (itemType == USERDEFINEDINT + 1) {
            const auto emojiItem = static_cast<EmojiPlugin::EmojiItem *>(item);
            openCloseDelims.append({emojiItem->startDelim(), emojiItem->endDelim()});
            return makePairs(emojiItem, waitingOpeningDelims, openCloseDelims);
        } else {
            qDebug() << "Item not handled" << static_cast<int>(item->type());
            return;
        }
    }
}

using Items = typename MD::QStringTrait::template Vector<std::shared_ptr<MD::Item<MD::QStringTrait>>>;
Items getInnerItems(MD::Item<MD::QStringTrait> *item)
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

QList<std::pair<MD::WithPosition, MD::WithPosition>> EditorHighlighter::getSurroundingDelimsPairs(MD::Item<MD::QStringTrait> *item,
                                                                                                  const MD::WithPosition &cursorPos)
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
        return {};
    }

    QList<std::pair<MD::WithPosition, MD::WithPosition>> surroundingDelimsPairs;
    for (const auto &pair : openCloseDelims) {
        const auto &open = pair.first;
        const auto &close = pair.second;

        if (md4qtHelperFunc::isBetweenDelims(cursorPos, open, close)) {
            surroundingDelimsPairs.append(pair);
        }
    }

    return surroundingDelimsPairs;
}

void EditorHighlighter::revertDelimsStyle()
{
    const auto c = d->editor->textCursor();
    const auto line = c.blockNumber();
    const auto column = c.positionInBlock();

    const auto startColumn = column ? column - 1 : column;
    const auto endColumn = column;

    const auto blockItems = findFirstInCache({startColumn, line, endColumn, line});

    if (blockItems.isEmpty()) {
        return;
    }

    const auto &first = blockItems.first(); // Usefull for heading

    const int cacheLen = blockItems.length();
    if (2 <= cacheLen) {
        const auto &secondToLast = blockItems.at(cacheLen - 2);

        const MD::WithPosition cursorPos = {startColumn, line, endColumn, line};
        const auto surroundingDelimsPairs = getSurroundingDelimsPairs(secondToLast, cursorPos);

        if (first->type() == MD::ItemType::Heading) {
            const auto &h = static_cast<MD::Heading<MD::QStringTrait> *>(first);
            d->headingLevel = h->level();

            for (const auto &delim : h->delims()) {
                d->revertFormat(delim);
            }
        }

        d->revertFormats(surroundingDelimsPairs);

        d->headingLevel = 0;

        return;
    }

    switch (first->type()) {
    case MD::ItemType::Code: {
        const auto codeItem = static_cast<MD::Code<MD::QStringTrait> *>(first);
        if (codeItem->isFensedCode()) {
            d->revertFormat(codeItem->startDelim());
            d->revertFormat(codeItem->endDelim());
        }
        break;
    }
    case MD::ItemType::Heading:
    case MD::ItemType::Blockquote:
    case MD::ItemType::List:
    case MD::ItemType::ListItem: {
        // Those block are "useless" in this case, but no need for an error message,
        // we already know that they're not supported, not a bug
        break;
    }
    default:
        qWarning() << "revertDelimsStyle: Unsupported block item" << static_cast<int>(first->type());
    }
}

void EditorHighlighter::showDelimAroundCursor(const bool clearCache)
{
    if (clearCache) {
        d->cachedFormats.clear();
    } else {
        d->restoreCachedFormats();
    }
    auto c = d->editor->textCursor();

    c.joinPreviousEditBlock();
    revertDelimsStyle();
    d->applyFormats();
    d->preventAutoScroll();
    c.endEditBlock();
}

void EditorHighlighter::onItemWithOpts(MD::ItemWithOpts<MD::QStringTrait> *i)
{
    QTextCharFormat special;
    special.setForeground(d->colors.specialColor);
    special.setFont(d->styleFont(d->additionalStyle, true));

    for (const auto &s : i->openStyles())
        d->setFormat(special, s);

    for (const auto &s : i->closeStyles())
        d->setFormat(special, s);
}

void EditorHighlighter::onText(MD::Text<MD::QStringTrait> *t)
{
    QTextCharFormat format = d->makeFormat(t->opts());

    d->setFormat(format, t->startLine(), t->startColumn(), t->endLine(), t->endColumn());

    onItemWithOpts(t);

    MD::PosCache<MD::QStringTrait>::onText(t);
}

void EditorHighlighter::onMath(MD::Math<MD::QStringTrait> *m)
{
    QTextCharFormat format;
    format.setForeground(d->colors.mathColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, m->startLine(), m->startColumn(), m->endLine(), m->endColumn());

    QTextCharFormat special;
    special.setForeground(d->colors.specialColor);
    special.setFont(d->styleFont(d->additionalStyle, true));

    if (m->startDelim().startColumn() != -1)
        d->setFormat(special, m->startDelim());

    if (m->endDelim().startColumn() != -1)
        d->setFormat(special, m->endDelim());

    if (m->syntaxPos().startColumn() != -1)
        d->setFormat(special, m->syntaxPos());

    onItemWithOpts(m);

    MD::PosCache<MD::QStringTrait>::onMath(m);
}

void EditorHighlighter::onHeading(MD::Heading<MD::QStringTrait> *h)
{
    QScopedValueRollback headingLevel(d->headingLevel, h->level());
    QScopedValueRollback style(d->additionalStyle, d->additionalStyle | MD::BoldText);

    QTextCharFormat baseFormat;
    baseFormat.setForeground(d->colors.titleColor);
    baseFormat.setFont(d->styleFont(MD::BoldText));
    const long long int formatStart = 0 <= h->text()->startColumn() ? h->text()->startColumn() : h->startColumn();
    d->setFormat(baseFormat, h->startLine(), formatStart, h->endLine(), h->endColumn());

    MD::PosCache<MD::QStringTrait>::onHeading(h);

    QTextCharFormat special;
    special.setForeground(d->colors.specialColor);
    special.setFont(d->styleFont(MD::TextWithoutFormat, true));

    if (!h->delims().empty()) {
        for (const auto &delim : h->delims())
            d->setFormat(special, delim);
    }

    if (h->labelPos().startColumn() != -1) {
        d->setFormat(special, h->labelPos());
    }
}

void EditorHighlighter::onCode(MD::Code<MD::QStringTrait> *c)
{
    QTextCharFormat format;
    QTextCharFormat special;
    format.setBackground(d->colors.codeBgColor);
    format.setFont(d->styleFont(d->additionalStyle));

    QColor foregroundColor = d->colors.codeColor;
    if (c->opts() & 8) {
        special.setBackground(d->colors.highlightColor);
        foregroundColor = d->colors.highlightColor;
    }
    format.setForeground(foregroundColor);

    if (c->startColumn() != c->endColumn() || c->startLine() != c->endLine()) {
        d->setFormat(format, c->startLine(), c->startColumn(), c->endLine(), c->endColumn());
    }

    special.setForeground(d->colors.specialColor);
    special.setFont(d->styleFont(d->additionalStyle, true));

    if (c->startDelim().startColumn() != -1)
        d->setFormat(special, c->startDelim());

    if (c->endDelim().startColumn() != -1)
        d->setFormat(special, c->endDelim());

    QTextCharFormat syntax;
    syntax.setForeground(d->colors.highlightColor);
    syntax.setFont(d->styleFont(d->additionalStyle));
    if (c->syntaxPos().startColumn() != -1)
        d->setFormat(syntax, c->syntaxPos());

    onItemWithOpts(c);

    MD::PosCache<MD::QStringTrait>::onCode(c);
}

void EditorHighlighter::onInlineCode(MD::Code<MD::QStringTrait> *c)
{
    QTextCharFormat format;
    QTextCharFormat special;
    format.setBackground(d->colors.codeBgColor);
    format.setFont(d->styleFont(d->additionalStyle));

    QColor foregroundColor = d->colors.codeColor;
    if (c->opts() & 8) {
        special.setBackground(d->colors.highlightColor);
        foregroundColor = d->colors.highlightColor;
    }
    format.setForeground(foregroundColor);

    d->setFormat(format, c->startLine(), c->startColumn(), c->endLine(), c->endColumn());

    special.setForeground(d->colors.specialColor);
    special.setFont(d->styleFont(d->additionalStyle, true));

    if (c->startDelim().startColumn() != -1)
        d->setFormat(special, c->startDelim());

    if (c->endDelim().startColumn() != -1)
        d->setFormat(special, c->endDelim());

    onItemWithOpts(c);

    MD::PosCache<MD::QStringTrait>::onInlineCode(c);
}

void EditorHighlighter::onBlockquote(MD::Blockquote<MD::QStringTrait> *b)
{
    MD::PosCache<MD::QStringTrait>::onBlockquote(b);

    QTextCharFormat special;
    special.setForeground(d->colors.linkColor);
    special.setFont(d->styleFont(d->additionalStyle));

    for (const auto &dd : b->delims())
        d->setFormat(special, dd);
}

void EditorHighlighter::onListItem(MD::ListItem<MD::QStringTrait> *l, bool first)
{
    MD::PosCache<MD::QStringTrait>::onListItem(l, first);

    QTextCharFormat special;
    special.setForeground(d->colors.highlightColor);
    special.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(special, l->delim());

    if (l->taskDelim().startColumn() != -1)
        d->setFormat(special, l->taskDelim());
}

void EditorHighlighter::onTable(MD::Table<MD::QStringTrait> *t)
{
    QTextCharFormat format;
    format.setForeground(d->colors.highlightColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, t->startLine(), t->startColumn(), t->endLine(), t->endColumn());

    MD::PosCache<MD::QStringTrait>::onTable(t);
}

void EditorHighlighter::onRawHtml(MD::RawHtml<MD::QStringTrait> *h)
{
    QTextCharFormat format;
    format.setForeground(d->colors.highlightColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, h->startLine(), h->startColumn(), h->endLine(), h->endColumn());

    onItemWithOpts(h);

    MD::PosCache<MD::QStringTrait>::onRawHtml(h);
}

void EditorHighlighter::onHorizontalLine(MD::HorizontalLine<MD::QStringTrait> *l)
{
    QTextCharFormat special;
    special.setForeground(d->colors.linkColor);
    special.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(special, l->startLine(), l->startColumn(), l->endLine(), l->endColumn());

    MD::PosCache<MD::QStringTrait>::onHorizontalLine(l);
}

void EditorHighlighter::onLink(MD::Link<MD::QStringTrait> *l)
{
    QTextCharFormat generalFormat;
    generalFormat.setForeground(d->colors.specialColor);
    generalFormat.setFont(d->styleFont(l->opts() | d->additionalStyle));

    d->setFormat(generalFormat, l->startLine(), l->startColumn(), l->endLine(), l->endColumn());

    QScopedValueRollback style(d->additionalStyle, d->additionalStyle | l->opts());

    QTextCharFormat textFormat;
    textFormat.setForeground(d->colors.textColor);
    textFormat.setFont(d->styleFont(l->opts() | d->additionalStyle));
    d->setFormat(textFormat, l->textPos());

    QTextCharFormat urlFormat;
    urlFormat.setForeground(d->colors.linkColor);
    urlFormat.setFont(d->styleFont(l->opts() | d->additionalStyle));
    urlFormat.setFontUnderline(true);
    d->setFormat(urlFormat, l->urlPos());

    MD::PosCache<MD::QStringTrait>::onLink(l);

    onItemWithOpts(l);
}

void EditorHighlighter::onReferenceLink(MD::Link<MD::QStringTrait> *l)
{
    QTextCharFormat generalFormat;
    generalFormat.setForeground(d->colors.specialColor);
    generalFormat.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(generalFormat, l->startLine(), l->startColumn(), l->endLine(), l->endColumn());

    QTextCharFormat urlFormat;
    urlFormat.setForeground(d->colors.linkColor);
    urlFormat.setFont(d->styleFont(d->additionalStyle));
    urlFormat.setFontUnderline(true);
    d->setFormat(urlFormat, l->urlPos());

    MD::PosCache<MD::QStringTrait>::onReferenceLink(l);
}

void EditorHighlighter::onImage(MD::Image<MD::QStringTrait> *i)
{
    QTextCharFormat generalFormat;
    generalFormat.setForeground(d->colors.specialColor);
    generalFormat.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(generalFormat, i->startLine(), i->startColumn(), i->endLine(), i->endColumn());

    QTextCharFormat urlFormat;
    urlFormat.setForeground(d->colors.linkColor);
    urlFormat.setFont(d->styleFont(d->additionalStyle));
    urlFormat.setFontUnderline(true);
    d->setFormat(urlFormat, i->urlPos());

    QTextCharFormat textFormat;
    textFormat.setForeground(d->colors.highlightColor);
    textFormat.setFont(d->styleFont(d->additionalStyle));
    d->setFormat(textFormat, i->textPos());

    MD::PosCache<MD::QStringTrait>::onImage(i);

    onItemWithOpts(i);
}

void EditorHighlighter::onFootnoteRef(MD::FootnoteRef<MD::QStringTrait> *ref)
{
    if (d->doc->footnotesMap().find(ref->id()) != d->doc->footnotesMap().cend()) {
        QTextCharFormat format;
        format.setForeground(d->colors.linkColor);
        format.setFont(d->styleFont(ref->opts() | d->additionalStyle));

        d->setFormat(format, ref->startLine(), ref->startColumn(), ref->endLine(), ref->endColumn());
    } else {
        QTextCharFormat format;
        format.setForeground(d->colors.textColor);
        format.setFont(d->styleFont(ref->opts() | d->additionalStyle));

        d->setFormat(format, ref->startLine(), ref->startColumn(), ref->endLine(), ref->endColumn());
    }

    MD::PosCache<MD::QStringTrait>::onFootnoteRef(ref);

    onItemWithOpts(ref);
}

void EditorHighlighter::onFootnote(MD::Footnote<MD::QStringTrait> *f)
{
    QTextCharFormat format;
    format.setForeground(d->colors.linkColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, f->startLine(), f->startColumn(), f->endLine(), f->endColumn());

    MD::PosCache<MD::QStringTrait>::onFootnote(f);
}

void EditorHighlighter::onEmoji(EmojiPlugin::EmojiItem *e)
{
    QTextCharFormat generalFormat;
    generalFormat.setForeground(d->colors.specialColor);
    generalFormat.setFont(d->styleFont(e->opts() | d->additionalStyle, true));

    d->setFormat(generalFormat, e->startLine(), e->startColumn(), e->endLine(), e->endColumn());

    QScopedValueRollback style(d->additionalStyle, d->additionalStyle | e->opts());

    QTextCharFormat emojiFormat;
    emojiFormat.setForeground(d->colors.linkColor);
    emojiFormat.setFont(d->styleFont(e->opts() | d->additionalStyle));
    d->setFormat(emojiFormat, e->emojiNamePos());

    QTextCharFormat optionsFormat;
    optionsFormat.setForeground(d->colors.highlightColor);
    optionsFormat.setFont(d->styleFont(e->opts() | d->additionalStyle));

    d->setFormat(optionsFormat, e->optionsPos());

    onItemWithOpts(e);
}

void EditorHighlighter::onUserDefined(MD::Item<MD::QStringTrait> *item)
{
    const int itemType = static_cast<int>(item->type());
    switch (itemType) {
    case USERDEFINEDINT + 1: {
        auto emoji = static_cast<EmojiPlugin::EmojiItem *>(item);
        onEmoji(emoji);
        break;
    }
    default:
        qWarning() << "Unsupported custom item";
        return;
    }
    MD::PosCache<MD::QStringTrait>::onUserDefined(item);
}
// !EditorHighlighter
} // !namespace M
