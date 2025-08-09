// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "editorHighlighter.hpp"

// KleverNotes includes
#include "logic/editor/editorHandler.hpp"
#include "logic/editor/editorTextManipulation.hpp"

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

void EditorHighlighter::setColors(const Colors &colors)
{
    d->colors = colors;
}

void EditorHighlighter::cacheAndHighlight(std::shared_ptr<MD::Document<MD::QStringTrait>> doc, const bool highlight)
{
    m_highlightEnabled = highlight;
    auto c = d->editor->textCursor();
    c.beginEditBlock();
    d->clearFormats();

    d->doc = doc;

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

void EditorHighlighter::addBlockDelims(QList<posCacheUtils::DelimsInfo> &delims,
                                       const QTextBlock &block,
                                       const MD::WithPosition &pos,
                                       const MD::WithPosition &selectStartPos,
                                       const MD::WithPosition &selectEndPos)
{
    const auto blockLen = editorTextManipulation::rstrip(block.text()).length();
    const auto blockEnd = 1 < blockLen ? blockLen - 1 : blockLen;
    if (blockEnd) {
        const auto line = block.blockNumber();
        Items blockItems = findFirstInCache({blockEnd, line, blockEnd, line});

        if (!blockItems.isEmpty()) {
            posCacheUtils::addDelimsFromItems(delims, blockItems, pos, selectStartPos, selectEndPos);
        }
    }
}

QList<posCacheUtils::DelimsInfo> EditorHighlighter::getDelimsFromCursor()
{
    QList<posCacheUtils::DelimsInfo> delims;

    const auto selectionStart = d->editor->selectionStart();
    const auto selectionEnd = d->editor->selectionEnd();
    auto cursor = d->editor->textCursor();
    if (cursor.position() != selectionStart) {
        cursor.setPosition(selectionStart);
    }

    auto block = cursor.block();
    const auto startingBlock = block;
    if (!block.contains(selectionEnd)) {
        block = block.next();
        while (block.isValid() && !block.contains(selectionEnd)) {
            const auto blockLen = editorTextManipulation::rstrip(block.text()).length();
            const auto blockEnd = 1 < blockLen ? blockLen - 1 : blockLen;
            const MD::WithPosition blockPos(blockEnd, block.blockNumber(), blockEnd, block.blockNumber());
            addBlockDelims(delims, block, blockPos);

            block = block.next();
        }
    }

    const int startLine = startingBlock.blockNumber();
    const int inBlockStart = selectionStart - startingBlock.position();
    const MD::WithPosition startPos(inBlockStart, startLine, inBlockStart, startLine);

    const int endLine = block.blockNumber();
    const int inBlockEnd = selectionEnd - block.position();
    const MD::WithPosition endPos(inBlockEnd, endLine, inBlockEnd, endLine);

    addBlockDelims(delims, startingBlock, startPos, startPos, endPos);
    addBlockDelims(delims, block, endPos, startPos, endPos);

    return delims;
}

void EditorHighlighter::revertDelimsStyle(const QList<posCacheUtils::DelimsInfo> &delims)
{
    for (const auto &delimInfo : delims) {
        d->revertFormats(delimInfo);
    }
}

QList<posCacheUtils::DelimsInfo> EditorHighlighter::showDelimAroundCursor(const bool clearCache)
{
    if (clearCache) {
        d->cachedFormats.clear();
    } else {
        d->restoreCachedFormats();
    }

    const auto delims = getDelimsFromCursor();
    if (m_highlightEnabled) {
        auto c = d->editor->textCursor();
        c.joinPreviousEditBlock();
        revertDelimsStyle(delims);
        d->applyFormats();
        d->preventAutoScroll();
        c.endEditBlock();
    }

    return delims;
}

MD::ListItem<MD::QStringTrait> *EditorHighlighter::searchListItem(const int line, const int pos)
{
    const Items blockItems = findFirstInCache({pos, line, pos, line});

    for (int i = blockItems.size() - 1; -1 != i; --i) {
        const auto item = blockItems.at(i);
        if (item->type() == MD::ItemType::ListItem) {
            return static_cast<MD::ListItem<MD::QStringTrait> *>(item);
        }
    }

    return nullptr;
}

void EditorHighlighter::onItemWithOpts(MD::ItemWithOpts<MD::QStringTrait> *i)
{
    if (m_highlightEnabled) {
        QTextCharFormat special;
        special.setForeground(d->colors.specialColor);
        special.setFont(d->styleFont(d->additionalStyle, true));

        for (const auto &s : i->openStyles())
            d->setFormat(special, s);

        for (const auto &s : i->closeStyles())
            d->setFormat(special, s);
    }
}

void EditorHighlighter::onText(MD::Text<MD::QStringTrait> *t)
{
    if (m_highlightEnabled) {
        QTextCharFormat format = d->makeFormat(t->opts());
        d->setFormat(format, t->startLine(), t->startColumn(), t->endLine(), t->endColumn());
    }
    onItemWithOpts(t);

    MD::PosCache<MD::QStringTrait>::onText(t);
}

void EditorHighlighter::onMath(MD::Math<MD::QStringTrait> *m)
{
    if (m_highlightEnabled) {
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
    }
    onItemWithOpts(m);

    MD::PosCache<MD::QStringTrait>::onMath(m);
}

void EditorHighlighter::onHeading(MD::Heading<MD::QStringTrait> *h)
{
    const int inititalHeadingLevel = d->headingLevel;
    const int inititalAdditionalStyle = d->additionalStyle;
    if (m_highlightEnabled) {
        d->headingLevel = h->level();
        d->additionalStyle = d->additionalStyle | MD::BoldText;

        QTextCharFormat baseFormat;
        baseFormat.setForeground(d->colors.titleColor);
        baseFormat.setFont(d->styleFont(MD::BoldText));
        const long long int formatStart = 0 <= h->text()->startColumn() ? h->text()->startColumn() : h->startColumn();
        d->setFormat(baseFormat, h->startLine(), formatStart, h->endLine(), h->endColumn());
    }

    MD::PosCache<MD::QStringTrait>::onHeading(h);

    d->headingLevel = inititalHeadingLevel;
    d->additionalStyle = inititalAdditionalStyle;

    if (m_highlightEnabled) {
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
}

void EditorHighlighter::onCode(MD::Code<MD::QStringTrait> *c)
{
    if (m_highlightEnabled) {
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
    }
    onItemWithOpts(c);

    MD::PosCache<MD::QStringTrait>::onCode(c);
}

void EditorHighlighter::onInlineCode(MD::Code<MD::QStringTrait> *c)
{
    if (m_highlightEnabled) {
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
    }
    onItemWithOpts(c);

    MD::PosCache<MD::QStringTrait>::onInlineCode(c);
}

void EditorHighlighter::onBlockquote(MD::Blockquote<MD::QStringTrait> *b)
{
    MD::PosCache<MD::QStringTrait>::onBlockquote(b);
    if (m_highlightEnabled) {
        QTextCharFormat special;
        special.setForeground(d->colors.linkColor);
        special.setFont(d->styleFont(d->additionalStyle));

        for (const auto &dd : b->delims())
            d->setFormat(special, dd);
    }
}

void EditorHighlighter::onListItem(MD::ListItem<MD::QStringTrait> *l, bool first, bool skipOpeningWrap)
{
    MD::PosCache<MD::QStringTrait>::onListItem(l, first, skipOpeningWrap);
    if (m_highlightEnabled) {
        QTextCharFormat special;
        special.setForeground(d->colors.highlightColor);
        special.setFont(d->styleFont(d->additionalStyle));

        d->setFormat(special, l->delim());

        if (l->taskDelim().startColumn() != -1)
            d->setFormat(special, l->taskDelim());
    }
}

void EditorHighlighter::onTable(MD::Table<MD::QStringTrait> *t)
{
    if (m_highlightEnabled) {
        QTextCharFormat format;
        format.setForeground(d->colors.highlightColor);
        format.setFont(d->styleFont(d->additionalStyle));

        d->setFormat(format, t->startLine(), t->startColumn(), t->endLine(), t->endColumn());
    }
    MD::PosCache<MD::QStringTrait>::onTable(t);
}

void EditorHighlighter::onRawHtml(MD::RawHtml<MD::QStringTrait> *h)
{
    if (m_highlightEnabled) {
        QTextCharFormat format;
        format.setForeground(d->colors.highlightColor);
        format.setFont(d->styleFont(d->additionalStyle));

        d->setFormat(format, h->startLine(), h->startColumn(), h->endLine(), h->endColumn());
    }
    onItemWithOpts(h);

    MD::PosCache<MD::QStringTrait>::onRawHtml(h);
}

void EditorHighlighter::onHorizontalLine(MD::HorizontalLine<MD::QStringTrait> *l)
{
    if (m_highlightEnabled) {
        QTextCharFormat special;
        special.setForeground(d->colors.linkColor);
        special.setFont(d->styleFont(d->additionalStyle));

        d->setFormat(special, l->startLine(), l->startColumn(), l->endLine(), l->endColumn());
    }
    MD::PosCache<MD::QStringTrait>::onHorizontalLine(l);
}

void EditorHighlighter::onLink(MD::Link<MD::QStringTrait> *l)
{
    const int inititalAdditionalStyle = d->additionalStyle;
    if (m_highlightEnabled) {
        QTextCharFormat generalFormat;
        generalFormat.setForeground(d->colors.specialColor);
        generalFormat.setFont(d->styleFont(l->opts() | d->additionalStyle));

        d->setFormat(generalFormat, l->startLine(), l->startColumn(), l->endLine(), l->endColumn());

        d->additionalStyle = d->additionalStyle | l->opts();

        QTextCharFormat textFormat;
        textFormat.setForeground(d->colors.textColor);
        textFormat.setFont(d->styleFont(l->opts() | d->additionalStyle));
        d->setFormat(textFormat, l->textPos());

        QTextCharFormat urlFormat;
        urlFormat.setForeground(d->colors.linkColor);
        urlFormat.setFont(d->styleFont(l->opts() | d->additionalStyle));
        urlFormat.setFontUnderline(true);
        d->setFormat(urlFormat, l->urlPos());
    }
    MD::PosCache<MD::QStringTrait>::onLink(l);

    onItemWithOpts(l);

    d->additionalStyle = inititalAdditionalStyle;
}

void EditorHighlighter::onReferenceLink(MD::Link<MD::QStringTrait> *l)
{
    if (m_highlightEnabled) {
        QTextCharFormat generalFormat;
        generalFormat.setForeground(d->colors.specialColor);
        generalFormat.setFont(d->styleFont(d->additionalStyle));

        d->setFormat(generalFormat, l->startLine(), l->startColumn(), l->endLine(), l->endColumn());

        QTextCharFormat urlFormat;
        urlFormat.setForeground(d->colors.linkColor);
        urlFormat.setFont(d->styleFont(d->additionalStyle));
        urlFormat.setFontUnderline(true);
        d->setFormat(urlFormat, l->urlPos());
    }
    MD::PosCache<MD::QStringTrait>::onReferenceLink(l);
}

void EditorHighlighter::onImage(MD::Image<MD::QStringTrait> *i)
{
    if (m_highlightEnabled) {
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
    }
    MD::PosCache<MD::QStringTrait>::onImage(i);

    onItemWithOpts(i);
}

void EditorHighlighter::onFootnoteRef(MD::FootnoteRef<MD::QStringTrait> *ref)
{
    if (m_highlightEnabled) {
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
    }
    MD::PosCache<MD::QStringTrait>::onFootnoteRef(ref);

    onItemWithOpts(ref);
}

void EditorHighlighter::onFootnote(MD::Footnote<MD::QStringTrait> *f)
{
    if (m_highlightEnabled) {
        QTextCharFormat format;
        format.setForeground(d->colors.linkColor);
        format.setFont(d->styleFont(d->additionalStyle));

        d->setFormat(format, f->startLine(), f->startColumn(), f->endLine(), f->endColumn());
    }
    MD::PosCache<MD::QStringTrait>::onFootnote(f);
}

void EditorHighlighter::onEmoji(EmojiPlugin::EmojiItem *e)
{
    const int inititalAdditionalStyle = d->additionalStyle;
    if (m_highlightEnabled) {
        QTextCharFormat generalFormat;
        generalFormat.setForeground(d->colors.specialColor);
        generalFormat.setFont(d->styleFont(e->opts() | d->additionalStyle, true));

        d->setFormat(generalFormat, e->startLine(), e->startColumn(), e->endLine(), e->endColumn());

        d->additionalStyle = d->additionalStyle | e->opts();

        QTextCharFormat emojiFormat;
        emojiFormat.setForeground(d->colors.linkColor);
        emojiFormat.setFont(d->styleFont(e->opts() | d->additionalStyle));
        d->setFormat(emojiFormat, e->emojiNamePos());

        QTextCharFormat optionsFormat;
        optionsFormat.setForeground(d->colors.highlightColor);
        optionsFormat.setFont(d->styleFont(e->opts() | d->additionalStyle));

        d->setFormat(optionsFormat, e->optionsPos());
    }
    onItemWithOpts(e);

    d->additionalStyle = inititalAdditionalStyle;
}

void EditorHighlighter::onUserDefined(MD::Item<MD::QStringTrait> *item)
{
    if (m_highlightEnabled) {
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
    }
    MD::PosCache<MD::QStringTrait>::onUserDefined(item);
}
// !EditorHighlighter
} // !namespace M
