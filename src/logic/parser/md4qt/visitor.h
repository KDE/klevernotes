/*
    SPDX-FileCopyrightText: 2022-2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: MIT
*/

#ifndef MD4QT_MD_VISITOR_HPP_INCLUDED
#define MD4QT_MD_VISITOR_HPP_INCLUDED

// md4qt include.
#include "doc.h"
#include "utils.h"

// C++ include.
#include <string>
#include <utility>

namespace MD
{

//
// Visitor
//

/*!
 * \class MD::Visitor
 * \inmodule md4qt
 * \inheaderfile md4qt/visitor.h
 *
 * \brief Visitor interface to walk through the MD::Document.
 *
 * This is a very basic implementation of walking through the document.
 *
 * \sa MD::details::AlgoVisitor, MD::details::HtmlVisitor, MD::PosCache.
 */
template<class Trait>
class Visitor
{
public:
    /*!
     * Default constructor.
     */
    Visitor() = default;
    virtual ~Visitor() = default;

    /*!
     * Walk through the document.
     *
     * \a d Markdown document.
     */
    void process(std::shared_ptr<Document<Trait>> d)
    {
        m_anchors.clear();
        m_doc = d;

        for (auto it = m_doc->items().cbegin(), last = m_doc->items().cend(); it != last; ++it) {
            switch ((*it)->type()) {
            case ItemType::Anchor:
                m_anchors.push_back(static_cast<Anchor<Trait> *>(it->get())->label());
                break;

            default:
                break;
            }
        }

        for (auto it = m_doc->items().cbegin(), last = m_doc->items().cend(); it != last; ++it) {
            if (static_cast<int>((*it)->type()) >= static_cast<int>(ItemType::UserDefined)) {
                onUserDefined(it->get());
            } else {
                switch ((*it)->type()) {
                case ItemType::Heading:
                    onHeading(static_cast<Heading<Trait> *>(it->get()));
                    break;

                case ItemType::Paragraph:
                    onParagraph(static_cast<Paragraph<Trait> *>(it->get()), true);
                    break;

                case ItemType::Code:
                    onCode(static_cast<Code<Trait> *>(it->get()));
                    break;

                case ItemType::Blockquote:
                    onBlockquote(static_cast<Blockquote<Trait> *>(it->get()));
                    break;

                case ItemType::List:
                    onList(static_cast<List<Trait> *>(it->get()));
                    break;

                case ItemType::Table:
                    onTable(static_cast<Table<Trait> *>(it->get()));
                    break;

                case ItemType::Anchor:
                    onAnchor(static_cast<Anchor<Trait> *>(it->get()));
                    break;

                case ItemType::RawHtml:
                    onRawHtml(static_cast<RawHtml<Trait> *>(it->get()));
                    break;

                case ItemType::HorizontalLine:
                    onHorizontalLine(static_cast<HorizontalLine<Trait> *>(it->get()));
                    break;

                default:
                    break;
                }
            }
        }
    }

protected:
    /*!
     * For some generator it's important to keep line endings like they were in Markdown.
     * So onParagraph() method invokes this method when necessary to add line ending.
     */
    virtual void onAddLineEnding() = 0;

    /*!
     * Handle user-defined item.
     *
     * \a item Item.
     */
    virtual void onUserDefined(Item<Trait> *item)
    {
        MD_UNUSED(item)
    }

    /*!
     * Handle text item.
     *
     * \a t Text.
     */
    virtual void onText(Text<Trait> *t) = 0;

    /*!
     * Handle LaTeX math expression.
     *
     * \a m Math.
     */
    virtual void onMath(Math<Trait> *m) = 0;

    /*!
     * Handle line break.
     *
     * \a b Line break.
     */
    virtual void onLineBreak(LineBreak<Trait> *b) = 0;

    /*!
     * Handle paragraph.
     *
     * \a p Paragraph.
     *
     * \a wrap Wrap this paragraph with something or no? It's useful to not wrap standalone
     *         paragraph in list item, for example.
     *
     * \a skipOpeningWrap Indicates that opening wrap should be added or no.
     */
    virtual void onParagraph(Paragraph<Trait> *p, bool wrap, bool skipOpeningWrap = false)
    {
        MD_UNUSED(wrap)
        MD_UNUSED(skipOpeningWrap)

        long long int l = (!p->items().empty() ? p->items().at(0)->startLine() : -1);

        for (auto it = p->items().begin(), last = p->items().end(); it != last; ++it) {
            if ((*it)->startLine() != l) {
                onAddLineEnding();
            }

            l = (*it)->endLine();

            if (static_cast<int>((*it)->type()) >= static_cast<int>(ItemType::UserDefined)) {
                onUserDefined(it->get());
            } else {
                switch ((*it)->type()) {
                case ItemType::Text:
                    onText(static_cast<Text<Trait> *>(it->get()));
                    break;

                case ItemType::Code:
                    onInlineCode(static_cast<Code<Trait> *>(it->get()));
                    break;

                case ItemType::Link:
                    onLink(static_cast<Link<Trait> *>(it->get()));
                    break;

                case ItemType::Image:
                    onImage(static_cast<Image<Trait> *>(it->get()));
                    break;

                case ItemType::Math:
                    onMath(static_cast<Math<Trait> *>(it->get()));
                    break;

                case ItemType::LineBreak:
                    onLineBreak(static_cast<LineBreak<Trait> *>(it->get()));
                    break;

                case ItemType::FootnoteRef:
                    onFootnoteRef(static_cast<FootnoteRef<Trait> *>(it->get()));
                    break;

                case ItemType::RawHtml:
                    onRawHtml(static_cast<RawHtml<Trait> *>(it->get()));
                    break;

                default:
                    break;
                }
            }
        }
    }

    /*!
     * Handle heading.
     *
     * \a h Heading.
     */
    virtual void onHeading(Heading<Trait> *h) = 0;

    /*!
     * Handle code.
     *
     * \a c Code.
     */
    virtual void onCode(Code<Trait> *c) = 0;

    /*!
     * Handle inline code.
     *
     * \a c Code
     */
    virtual void onInlineCode(Code<Trait> *c) = 0;

    /*!
     * Handle blockquote.
     *
     * \a b Blockquote.
     */
    virtual void onBlockquote(Blockquote<Trait> *b)
    {
        for (auto it = b->items().cbegin(), last = b->items().cend(); it != last; ++it) {
            if (static_cast<int>((*it)->type()) >= static_cast<int>(ItemType::UserDefined)) {
                onUserDefined(it->get());
            } else {
                switch ((*it)->type()) {
                case ItemType::Heading:
                    onHeading(static_cast<Heading<Trait> *>(it->get()));
                    break;

                case ItemType::Paragraph:
                    onParagraph(static_cast<Paragraph<Trait> *>(it->get()), true);
                    break;

                case ItemType::Code:
                    onCode(static_cast<Code<Trait> *>(it->get()));
                    break;

                case ItemType::Blockquote:
                    onBlockquote(static_cast<Blockquote<Trait> *>(it->get()));
                    break;

                case ItemType::List:
                    onList(static_cast<List<Trait> *>(it->get()));
                    break;

                case ItemType::Table:
                    onTable(static_cast<Table<Trait> *>(it->get()));
                    break;

                case ItemType::HorizontalLine:
                    onHorizontalLine(static_cast<HorizontalLine<Trait> *>(it->get()));
                    break;

                case ItemType::RawHtml:
                    onRawHtml(static_cast<RawHtml<Trait> *>(it->get()));
                    break;

                default:
                    break;
                }
            }
        }
    }

    /*!
     * Handle list.
     *
     * \a l List.
     */
    virtual void onList(List<Trait> *l) = 0;

    /*!
     * Handle table.
     *
     * \a t Table.
     */
    virtual void onTable(Table<Trait> *t) = 0;

    /*!
     * Handle anchor.
     *
     * \a a Anchor.
     */
    virtual void onAnchor(Anchor<Trait> *a) = 0;

    /*!
     * Handle raw HTML.
     *
     * \a h Raw HTML.
     */
    virtual void onRawHtml(RawHtml<Trait> *h) = 0;

    /*!
     * Handle horizontal line.
     *
     * \a l Horizontal line.
     */
    virtual void onHorizontalLine(HorizontalLine<Trait> *l) = 0;

    /*!
     * Handle link.
     *
     * \a l Link.
     */
    virtual void onLink(Link<Trait> *l) = 0;

    /*!
     * Handle image.
     *
     * \a i Image.
     */
    virtual void onImage(Image<Trait> *i) = 0;

    /*!
     * Handle footnote reference.
     *
     * \a ref Footnote reference.
     */
    virtual void onFootnoteRef(FootnoteRef<Trait> *ref) = 0;

    /*!
     * Handle list item.
     *
     * \a i List item.
     *
     * \a first Is this item first in the list?
     *
     * \a skipOpeningWrap Indicates that opening wrap should be added or no.
     */
    virtual void onListItem(ListItem<Trait> *i, bool first, bool skipOpeningWrap = false)
    {
        MD_UNUSED(first)

        for (auto it = i->items().cbegin(), last = i->items().cend(); it != last; ++it) {
            if (static_cast<int>((*it)->type()) >= static_cast<int>(ItemType::UserDefined)) {
                onUserDefined(it->get());
            } else {
                switch ((*it)->type()) {
                case ItemType::Heading:
                    onHeading(static_cast<Heading<Trait> *>(it->get()));
                    break;

                case ItemType::Paragraph:
                    onParagraph(static_cast<Paragraph<Trait> *>(it->get()), wrapFirstParagraphInListItem(i), skipOpeningWrap);
                    break;

                case ItemType::Code:
                    onCode(static_cast<Code<Trait> *>(it->get()));
                    break;

                case ItemType::Blockquote:
                    onBlockquote(static_cast<Blockquote<Trait> *>(it->get()));
                    break;

                case ItemType::List:
                    onList(static_cast<List<Trait> *>(it->get()));
                    break;

                case ItemType::Table:
                    onTable(static_cast<Table<Trait> *>(it->get()));
                    break;

                case ItemType::RawHtml:
                    onRawHtml(static_cast<RawHtml<Trait> *>(it->get()));
                    break;

                case ItemType::HorizontalLine:
                    onHorizontalLine(static_cast<HorizontalLine<Trait> *>(it->get()));
                    break;

                default:
                    break;
                }
            }

            skipOpeningWrap = false;
        }
    }

    /*!
     * Handle table cell.
     *
     * \a c Table cell.
     */
    virtual void onTableCell(TableCell<Trait> *c)
    {
        for (auto it = c->items().cbegin(), last = c->items().cend(); it != last; ++it) {
            if (static_cast<int>((*it)->type()) >= static_cast<int>(ItemType::UserDefined)) {
                onUserDefined(it->get());
            } else {
                switch ((*it)->type()) {
                case ItemType::Text:
                    onText(static_cast<Text<Trait> *>(it->get()));
                    break;

                case ItemType::Code:
                    onInlineCode(static_cast<Code<Trait> *>(it->get()));
                    break;

                case ItemType::Link:
                    onLink(static_cast<Link<Trait> *>(it->get()));
                    break;

                case ItemType::Image:
                    onImage(static_cast<Image<Trait> *>(it->get()));
                    break;

                case ItemType::FootnoteRef:
                    onFootnoteRef(static_cast<FootnoteRef<Trait> *>(it->get()));
                    break;

                case ItemType::RawHtml:
                    onRawHtml(static_cast<RawHtml<Trait> *>(it->get()));
                    break;

                case ItemType::Math:
                    onMath(static_cast<Math<Trait> *>(it->get()));

                default:
                    break;
                }
            }
        }
    }

    /*!
     * Handle footnote.
     *
     * \a f Footnote.
     */
    virtual void onFootnote(Footnote<Trait> *f)
    {
        for (auto it = f->items().cbegin(), last = f->items().cend(); it != last; ++it) {
            if (static_cast<int>((*it)->type()) >= static_cast<int>(ItemType::UserDefined)) {
                onUserDefined(it->get());
            } else {
                switch ((*it)->type()) {
                case ItemType::Heading:
                    onHeading(static_cast<Heading<Trait> *>(it->get()));
                    break;

                case ItemType::Paragraph:
                    onParagraph(static_cast<Paragraph<Trait> *>(it->get()), true);
                    break;

                case ItemType::Code:
                    onCode(static_cast<Code<Trait> *>(it->get()));
                    break;

                case ItemType::Blockquote:
                    onBlockquote(static_cast<Blockquote<Trait> *>(it->get()));
                    break;

                case ItemType::List:
                    onList(static_cast<List<Trait> *>(it->get()));
                    break;

                case ItemType::Table:
                    onTable(static_cast<Table<Trait> *>(it->get()));
                    break;

                case ItemType::RawHtml:
                    onRawHtml(static_cast<RawHtml<Trait> *>(it->get()));
                    break;

                case ItemType::HorizontalLine:
                    onHorizontalLine(static_cast<HorizontalLine<Trait> *>(it->get()));
                    break;

                default:
                    break;
                }
            }
        }
    }

    /*!
     * Returns whether first paragraph in list item should be wrapped.
     *
     * \a i List item.
     */
    virtual bool wrapFirstParagraphInListItem(ListItem<Trait> *i) const
    {
        return (i->items().size() > 1 && i->items().at(1)->type() != ItemType::List);
    }

protected:
    /*!
     * All available m_anchors in the document.
     */
    typename Trait::template Vector<typename Trait::String> m_anchors;
    /*!
     * Document.
     */
    std::shared_ptr<Document<Trait>> m_doc;
}; // class Visitor

} /* namespace MD */

#endif // MD4QT_MD_VISITOR_HPP_INCLUDED
