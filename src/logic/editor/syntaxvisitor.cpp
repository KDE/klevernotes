/*
    SPDX-FileCopyrightText: 2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

// KleverNotes includes
#include "logic/editor/editorHandler.hpp"

// md-editor include.
#include "logic/parser/md4qt/doc.hpp"
#include "syntaxvisitor.hpp"

// Qt include.
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextEdit>

// C++ include.
#include <algorithm>

namespace MdEditor
{
//
// SyntaxVisitorPrivate
//

struct SyntaxVisitorPrivate {
    SyntaxVisitorPrivate(EditorHandler *e)
        : editor(e)
    {
    }

    void clearFormats()
    {
        auto c = editor->textCursor();
        c.select(QTextCursor::Document);
        c.setCharFormat({});
    }

    void setFormat(const QTextCharFormat &format, const MD::WithPosition &pos)
    {
        setFormat(format, pos.startLine(), pos.startColumn(), pos.endLine(), pos.endColumn());
    }

    void setFormat(const QTextCharFormat &format, long long int startLine, long long int startColumn, long long int endLine, long long int endColumn)
    {
        if (colors.enabled) {
            for (auto i = startLine; i <= endLine; ++i) {
                auto b = editor->document()->findBlockByNumber(i);
                auto c = QTextCursor(b);
                auto oldFormat = c.blockCharFormat();

                c.setPosition(c.position() + (i == startLine ? startColumn : 0), QTextCursor::MoveAnchor);
                c.setPosition(c.position()
                                  + (i == startLine ? (i == endLine ? endColumn - startColumn + 1 : b.length() - startColumn)
                                                    : (i == endLine ? endColumn + 1 : b.length())),
                              QTextCursor::KeepAnchor);
                c.setCharFormat(format);
            }
        }
    }

    QFont styleFont(int opts) const
    {
        auto f = font;

        if (opts & MD::ItalicText)
            f.setItalic(true);

        if (opts & MD::BoldText)
            f.setBold(true);

        if (opts & MD::StrikethroughText)
            f.setStrikeOut(true);

        return f;
    }

    //! Editor.
    EditorHandler *editor = nullptr;
    //! Document.
    std::shared_ptr<MD::Document<MD::QStringTrait>> doc;
    //! Colors.
    Colors colors;
    //! Default font.
    QFont font;
    //! Additional style that should be applied for any item.
    int additionalStyle = 0;
}; // struct SyntaxVisitorPrivate
; // struct SyntaxVisitorPrivate

//
// SyntaxVisitor
//

SyntaxVisitor::SyntaxVisitor(EditorHandler *editor)
    : d(new SyntaxVisitorPrivate(editor))
{
}

SyntaxVisitor::~SyntaxVisitor()
{
}

void SyntaxVisitor::setFont(const QFont &f)
{
    d->font = f;
}

void SyntaxVisitor::clearHighlighting()
{
    d->clearFormats();
}

void SyntaxVisitor::highlight(std::shared_ptr<MD::Document<MD::QStringTrait>> doc, const Colors &colors)
{
    auto c = d->editor->textCursor();
    c.beginEditBlock();
    d->clearFormats();

    d->doc = doc;
    d->colors = colors;

    MD::PosCache<MD::QStringTrait>::initialize(d->doc);
    c.endEditBlock();
}

void SyntaxVisitor::onItemWithOpts(MD::ItemWithOpts<MD::QStringTrait> *i)
{
    QTextCharFormat special;
    special.setForeground(d->colors.specialColor);
    special.setFont(d->styleFont(d->additionalStyle));

    for (const auto &s : i->openStyles())
        d->setFormat(special, s);

    for (const auto &s : i->closeStyles())
        d->setFormat(special, s);
}

void SyntaxVisitor::onReferenceLink(MD::Link<MD::QStringTrait> *l)
{
    QTextCharFormat format;
    format.setForeground(d->colors.linkColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, l->startLine(), l->startColumn(), l->endLine(), l->endColumn());

    MD::PosCache<MD::QStringTrait>::onReferenceLink(l);
}

void SyntaxVisitor::onText(MD::Text<MD::QStringTrait> *t)
{
    QTextCharFormat format;
    format.setForeground(d->colors.textColor);
    format.setFont(d->styleFont(t->opts() | d->additionalStyle));

    d->setFormat(format, t->startLine(), t->startColumn(), t->endLine(), t->endColumn());

    onItemWithOpts(t);

    MD::PosCache<MD::QStringTrait>::onText(t);
}

void SyntaxVisitor::onMath(MD::Math<MD::QStringTrait> *m)
{
    QTextCharFormat format;
    format.setForeground(d->colors.mathColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, m->startLine(), m->startColumn(), m->endLine(), m->endColumn());

    QTextCharFormat special;
    special.setForeground(d->colors.specialColor);
    special.setFont(d->styleFont(d->additionalStyle));

    if (m->startDelim().startColumn() != -1)
        d->setFormat(special, m->startDelim());

    if (m->endDelim().startColumn() != -1)
        d->setFormat(special, m->endDelim());

    if (m->syntaxPos().startColumn() != -1)
        d->setFormat(special, m->syntaxPos());

    onItemWithOpts(m);

    MD::PosCache<MD::QStringTrait>::onMath(m);
}

void SyntaxVisitor::onHeading(MD::Heading<MD::QStringTrait> *h)
{
    const auto tmp = d->additionalStyle;

    d->additionalStyle |= MD::BoldText;

    MD::PosCache<MD::QStringTrait>::onHeading(h);

    d->additionalStyle = tmp;

    QTextCharFormat special;
    special.setForeground(d->colors.titleColor);
    special.setFont(d->styleFont(MD::TextWithoutFormat));

    if (!h->delims().empty()) {
        for (const auto &delim : h->delims())
            d->setFormat(special, delim);
    }

    if (h->labelPos().startColumn() != -1)
        d->setFormat(special, h->labelPos());

    QTextCharFormat baseFormat;
    baseFormat.setForeground(d->colors.titleColor);
    QFont baseFont = d->styleFont(MD::BoldText);
    // Base on KleverStyle.css
    auto size = baseFont.pointSize();
    switch (h->level()) {
    case 1:
        size += 12;
        break;
    case 2:
        size += 8;
        break;
    case 3:
        size += 2;
        break;
    case 5:
        size -= 2;
        break;
    case 6:
        size -= 3;
        break;
    }

    baseFont.setPointSize(size);
    baseFormat.setFont(baseFont);

    d->setFormat(baseFormat, h->startLine(), h->startColumn() + h->level() + 1, h->endLine(), h->endColumn());
}

void SyntaxVisitor::onCode(MD::Code<MD::QStringTrait> *c)
{
    QTextCharFormat format;
    format.setBackground(d->colors.codeColor);
    format.setForeground(d->colors.titleColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, c->startLine(), c->startColumn(), c->endLine(), c->endColumn());

    QTextCharFormat special;
    special.setForeground(d->colors.specialColor);
    special.setFont(d->styleFont(d->additionalStyle));

    if (c->startDelim().startColumn() != -1)
        d->setFormat(special, c->startDelim());

    if (c->endDelim().startColumn() != -1)
        d->setFormat(special, c->endDelim());

    if (c->syntaxPos().startColumn() != -1)
        d->setFormat(special, c->syntaxPos());

    onItemWithOpts(c);

    MD::PosCache<MD::QStringTrait>::onCode(c);
}

void SyntaxVisitor::onInlineCode(MD::Code<MD::QStringTrait> *c)
{
    QTextCharFormat format;
    format.setBackground(d->colors.codeColor);
    format.setForeground(d->colors.titleColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, c->startLine(), c->startColumn(), c->endLine(), c->endColumn());

    QTextCharFormat special;
    special.setForeground(d->colors.specialColor);
    special.setFont(d->styleFont(d->additionalStyle));

    if (c->startDelim().startColumn() != -1)
        d->setFormat(special, c->startDelim());

    if (c->endDelim().startColumn() != -1)
        d->setFormat(special, c->endDelim());

    onItemWithOpts(c);

    MD::PosCache<MD::QStringTrait>::onInlineCode(c);
}

void SyntaxVisitor::onBlockquote(MD::Blockquote<MD::QStringTrait> *b)
{
    MD::PosCache<MD::QStringTrait>::onBlockquote(b);

    QTextCharFormat special;
    special.setForeground(d->colors.linkColor);
    special.setFont(d->styleFont(d->additionalStyle));

    for (const auto &dd : b->delims())
        d->setFormat(special, dd);
}

void SyntaxVisitor::onListItem(MD::ListItem<MD::QStringTrait> *l, bool first)
{
    MD::PosCache<MD::QStringTrait>::onListItem(l, first);

    QTextCharFormat special;
    special.setForeground(d->colors.highlightColor);
    special.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(special, l->delim());

    if (l->taskDelim().startColumn() != -1)
        d->setFormat(special, l->taskDelim());
}

void SyntaxVisitor::onTable(MD::Table<MD::QStringTrait> *t)
{
    QTextCharFormat format;
    format.setForeground(d->colors.codeColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, t->startLine(), t->startColumn(), t->endLine(), t->endColumn());

    MD::PosCache<MD::QStringTrait>::onTable(t);
}

void SyntaxVisitor::onRawHtml(MD::RawHtml<MD::QStringTrait> *h)
{
    QTextCharFormat format;
    format.setForeground(d->colors.highlightColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, h->startLine(), h->startColumn(), h->endLine(), h->endColumn());

    onItemWithOpts(h);

    MD::PosCache<MD::QStringTrait>::onRawHtml(h);
}

void SyntaxVisitor::onHorizontalLine(MD::HorizontalLine<MD::QStringTrait> *l)
{
    QTextCharFormat special;
    special.setForeground(d->colors.linkColor);
    special.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(special, l->startLine(), l->startColumn(), l->endLine(), l->endColumn());

    MD::PosCache<MD::QStringTrait>::onHorizontalLine(l);
}

void SyntaxVisitor::onLink(MD::Link<MD::QStringTrait> *l)
{
    QTextCharFormat format;
    format.setForeground(d->colors.linkColor);
    format.setFont(d->styleFont(l->opts() | d->additionalStyle));

    d->setFormat(format, l->startLine(), l->startColumn(), l->endLine(), l->endColumn());

    const auto tmp = d->additionalStyle;
    d->additionalStyle |= l->opts();

    MD::PosCache<MD::QStringTrait>::onLink(l);

    onItemWithOpts(l);

    d->additionalStyle = tmp;
}

void SyntaxVisitor::onImage(MD::Image<MD::QStringTrait> *i)
{
    QTextCharFormat format;
    format.setForeground(d->colors.linkColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, i->startLine(), i->startColumn(), i->endLine(), i->endColumn());

    MD::PosCache<MD::QStringTrait>::onImage(i);

    onItemWithOpts(i);
}

void SyntaxVisitor::onFootnoteRef(MD::FootnoteRef<MD::QStringTrait> *ref)
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

void SyntaxVisitor::onFootnote(MD::Footnote<MD::QStringTrait> *f)
{
    QTextCharFormat format;
    format.setForeground(d->colors.referenceColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, f->startLine(), f->startColumn(), f->endLine(), f->endColumn());

    MD::PosCache<MD::QStringTrait>::onFootnote(f);
}

} /* namespace MdEditor */
