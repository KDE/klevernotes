/*
    SPDX-FileCopyrightText: 2022-2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: MIT
*/

#ifndef MD4QT_MD_HTML_HPP_INCLUDED
#define MD4QT_MD_HTML_HPP_INCLUDED

// md4qt include.
#include "doc.h"
#include "visitor.h"

// C++ include.
#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>

namespace MD
{

namespace details
{

//
// IdsMap
//

/*!
 * \typealias MD::details::IdsMap
 * \inmodule md4qt
 * \inheaderfile md4qt/html.h
 *
 * \brief Map of IDs to set them for items in HTML.
 *
 * This is a map of IDs that should be set in HTML for corresponding items.
 */
template<class Trait>
using IdsMap = std::unordered_map<MD::Item<Trait> *, typename Trait::String>;

//
// HtmlVisitor
//

/*!
 * \class MD::details::HtmlVisitor
 * \inmodule md4qt
 * \inheaderfile md4qt/html.h
 *
 * \brief HTML visitor interface to walk through the MD:Document.
 *
 * This is visitor to convert Markdown document to HTML string. This class is highly
 * customizable, absolutelly everything may be overriden, all data members are protected
 * so a developer won't have problems with customizaion.
 *
 * As example of customization have a look at the following code.
 *
 * \code
 * class HtmlVisitor : public MD::details::HtmlVisitor<MD::QStringTrait>
 * {
 * protected:
 *     QString prepareTextForHtml(const QString &t) override
 *     {
 *         auto tmp = MD::details::HtmlVisitor<MD::QStringTrait>::prepareTextForHtml(t);
 *         tmp.replace(QLatin1Char('$'), QStringLiteral("<span>$</span>"));
 *
 *         return tmp;
 *     }
 * };
 * \endcode
 *
 * In this example we convert \c {$} character to \c {<span>$</span>} when inserting string to HTML.
 * This is needed to disable processing of \c {$} character in HTML as start/end of \c {LaTeX} equation
 * by some JavaScript, like \c {KaTeX}, to prevent wrong rendering of standalone \c {$} character.
 *
 * MD::details::HtmlVisitor doesn't do this protection, so if in your case you use rendering of \c {LaTeX}
 * math equation and enframe them by \c {$} character in HTML, you will need something like the above example.
 */
template<class Trait>
class HtmlVisitor : public Visitor<Trait>
{
public:
    /*!
     * Default constructor.
     */
    HtmlVisitor() = default;
    ~HtmlVisitor() override = default;

    /*!
     * Walk through the document.
     *
     * \a doc Document.
     *
     * \a hrefForRefBackImage String that will be applied as URL for image for back link from footnote.
     *
     * \a wrappedInArticle Wrap HTML with <article> tag?
     *
     * \a idsMap Map of IDs to set to corresonding items.
     */
    virtual typename Trait::String toHtml(std::shared_ptr<Document<Trait>> doc,
                                          const typename Trait::String &hrefForRefBackImage,
                                          bool wrappedInArticle = true,
                                          const IdsMap<Trait> *idsMap = nullptr)
    {
        m_isWrappedInArticle = wrappedInArticle;
        m_idsMap = idsMap;

        m_html.clear();
        m_fns.clear();

        this->process(doc);

        onFootnotes(hrefForRefBackImage);

        return m_html;
    }

protected:
    /*!
     * Returns ID of item if it's set.
     *
     * \a item Item.
     */
    virtual typename Trait::String getId(Item<Trait> *item) const
    {
        if (item->type() != ItemType::Heading) {
            if (m_idsMap) {
                const auto it = m_idsMap->find(item);

                if (it != m_idsMap->cend()) {
                    return it->second;
                }
            }
        } else {
            return static_cast<Heading<Trait> *>(item)->label();
        }

        return {};
    }

    /*!
     * Append ID into HTML.
     *
     * \a item Item.
     */
    virtual void printId(Item<Trait> *item)
    {
        const auto id = getId(item);

        if (!id.isEmpty()) {
            m_html.push_back(Trait::latin1ToString(" id=\""));
            m_html.push_back(id);
            m_html.push_back(Trait::latin1ToString("\""));
        }
    }

    /*!
     * Insert into HTML tags for opening styles.
     *
     * \a styles Styles.
     */
    virtual void openStyle(const typename ItemWithOpts<Trait>::Styles &styles)
    {
        for (const auto &s : styles) {
            switch (s.style()) {
            case TextOption::BoldText:
                m_html.push_back(Trait::latin1ToString("<strong>"));
                break;

            case TextOption::ItalicText:
                m_html.push_back(Trait::latin1ToString("<em>"));
                break;

            case TextOption::StrikethroughText:
                m_html.push_back(Trait::latin1ToString("<del>"));
                break;

            default:
                break;
            }
        }
    }

    /*!
     * Insert into HTML tags for closing styles.
     *
     * \a styles Styles.
     */
    virtual void closeStyle(const typename ItemWithOpts<Trait>::Styles &styles)
    {
        for (const auto &s : styles) {
            switch (s.style()) {
            case TextOption::BoldText:
                m_html.push_back(Trait::latin1ToString("</strong>"));
                break;

            case TextOption::ItalicText:
                m_html.push_back(Trait::latin1ToString("</em>"));
                break;

            case TextOption::StrikethroughText:
                m_html.push_back(Trait::latin1ToString("</del>"));
                break;

            default:
                break;
            }
        }
    }

    /*!
     * Handle new line in HTML.
     */
    void onAddLineEnding() override
    {
        if (!m_justCollectFootnoteRefs)
            m_html.push_back(Trait::latin1ToString("\n"));
    }

    /*!
     * Handle text item.
     *
     * \a t Text.
     */
    void onText(Text<Trait> *t) override
    {
        if (!m_justCollectFootnoteRefs) {
            openStyle(t->openStyles());

            m_html.push_back(prepareTextForHtml(t->text()));

            closeStyle(t->closeStyles());
        }
    }

    /*!
     * Handle LaTeX math expression.
     *
     * \a m Math.
     */
    void onMath(Math<Trait> *m) override
    {
        if (!m_justCollectFootnoteRefs) {
            openStyle(m->openStyles());

            m_html.push_back(m->isInline() ? Trait::latin1ToString("$") : Trait::latin1ToString("$$"));
            m_html.push_back(prepareTextForHtml(m->expr()));
            m_html.push_back(m->isInline() ? Trait::latin1ToString("$") : Trait::latin1ToString("$$"));

            closeStyle(m->closeStyles());
        }
    }

    /*!
     * Handle line break.
     */
    void onLineBreak(LineBreak<Trait> *) override
    {
        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("<br />"));
        }
    }

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
    void onParagraph(Paragraph<Trait> *p, bool wrap, bool skipOpeningWrap = false) override
    {
        if (wrap && !m_justCollectFootnoteRefs && !skipOpeningWrap) {
            m_html.push_back(Trait::latin1ToString("<p dir=\"auto\""));
            printId(p);
            m_html.push_back(Trait::latin1ToString(">"));
        }

        Visitor<Trait>::onParagraph(p, wrap);

        if (wrap && !m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("</p>"));
        }
    }

    /*!
     * Handle heading.
     *
     * \a h Heading.
     */
    void onHeading(Heading<Trait> *h) override
    {
        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("\n"));
        }

        switch (h->level()) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            onHeading(h, Trait::latin1ToString("h") + Trait::latin1ToString(std::to_string(h->level()).c_str()));
            break;

        default:
            break;
        }

        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("\n"));
        }
    }

    /*!
     * Handle code block.
     *
     * \a c Code.
     */
    void onCode(Code<Trait> *c) override
    {
        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("\n"));
            m_html.push_back(Trait::latin1ToString("<pre"));
            printId(c);
            m_html.push_back(Trait::latin1ToString("><code"));

            if (!c->syntax().isEmpty()) {
                m_html.push_back(Trait::latin1ToString(" class=\"language-"));
                m_html.push_back(c->syntax());
                m_html.push_back(Trait::latin1ToString("\""));
            }

            m_html.push_back(Trait::latin1ToString(">"));
            m_html.push_back(prepareTextForHtml(c->text()));
            m_html.push_back(Trait::latin1ToString("</code></pre>"));
            m_html.push_back(Trait::latin1ToString("\n"));
        }
    }

    /*!
     * Handle inline code.
     *
     * \a c Code.
     */
    void onInlineCode(Code<Trait> *c) override
    {
        if (!m_justCollectFootnoteRefs) {
            openStyle(c->openStyles());

            m_html.push_back(Trait::latin1ToString("<code"));
            printId(c);
            m_html.push_back(Trait::latin1ToString(">"));

            m_html.push_back(prepareTextForHtml(c->text()));

            m_html.push_back(Trait::latin1ToString("</code>"));

            closeStyle(c->closeStyles());
        }
    }

    /*!
     * Handle blockquote.
     *
     * \a b Blockquote.
     */
    void onBlockquote(Blockquote<Trait> *b) override
    {
        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("\n<blockquote"));
            printId(b);
            m_html.push_back(Trait::latin1ToString(">"));
        }

        Visitor<Trait>::onBlockquote(b);

        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("</blockquote>\n"));
        }
    }

    /*!
     * Handle list.
     *
     * \a l List.
     */
    void onList(List<Trait> *l) override
    {
        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("\n"));
        }

        typename ListItem<Trait>::ListType type = ListItem<Trait>::Ordered;
        bool first = true;

        for (auto it = l->items().cbegin(), last = l->items().cend(); it != last; ++it) {
            if ((*it)->type() == ItemType::ListItem) {
                auto *item = static_cast<ListItem<Trait> *>(it->get());

                if (first) {
                    type = item->listType();

                    if (type == ListItem<Trait>::Ordered) {
                        if (!m_justCollectFootnoteRefs) {
                            m_html.push_back(Trait::latin1ToString("<ol"));
                            printId(l);

                            if (item->isTaskList()) {
                                m_html.push_back(Trait::latin1ToString(" class=\"contains-task-list\""));
                            }

                            m_html.push_back(Trait::latin1ToString(" dir=\"auto\">\n"));
                        }
                    } else {
                        if (!m_justCollectFootnoteRefs) {
                            m_html.push_back(Trait::latin1ToString("<ul"));
                            printId(l);

                            if (item->isTaskList()) {
                                m_html.push_back(Trait::latin1ToString(" class=\"contains-task-list\""));
                            }

                            m_html.push_back(Trait::latin1ToString(" dir=\"auto\">\n"));
                        }
                    }
                }

                onListItem(item, first);

                first = false;
            }
        }

        if (!first) {
            if (!m_justCollectFootnoteRefs) {
                if (type == ListItem<Trait>::Ordered) {
                    m_html.push_back(Trait::latin1ToString("</ol>\n"));
                } else {
                    m_html.push_back(Trait::latin1ToString("</ul>\n"));
                }
            }
        }
    }

    /*!
     * Handle table.
     *
     * \a t Table.
     */
    void onTable(Table<Trait> *t) override
    {
        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("\n"));
        }

        if (!t->isEmpty()) {
            if (!m_justCollectFootnoteRefs) {
                m_html.push_back(Trait::latin1ToString("<table"));
                printId(t);
                m_html.push_back(Trait::latin1ToString("><thead><tr>\n"));
            }

            int columns = 0;

            for (auto th = (*t->rows().cbegin())->cells().cbegin(), last = (*t->rows().cbegin())->cells().cend(); th != last; ++th) {
                if (!m_justCollectFootnoteRefs) {
                    m_html.push_back(Trait::latin1ToString("<th"));
                    m_html.push_back(tableAlignmentToHtml(t->columnAlignment(columns)));
                    m_html.push_back(Trait::latin1ToString(" dir=\"auto\">\n"));
                }

                this->onTableCell(th->get());

                if (!m_justCollectFootnoteRefs) {
                    m_html.push_back(Trait::latin1ToString("\n</th>\n"));
                }

                ++columns;
            }

            if (!m_justCollectFootnoteRefs) {
                m_html.push_back(Trait::latin1ToString("</tr></thead><tbody>\n"));
            }

            for (auto r = std::next(t->rows().cbegin()), rlast = t->rows().cend(); r != rlast; ++r) {
                if (!m_justCollectFootnoteRefs) {
                    m_html.push_back(Trait::latin1ToString("<tr>\n"));
                }

                int i = 0;

                for (auto c = (*r)->cells().cbegin(), clast = (*r)->cells().cend(); c != clast; ++c) {
                    if (!m_justCollectFootnoteRefs) {
                        m_html.push_back(Trait::latin1ToString("\n<td"));
                        m_html.push_back(tableAlignmentToHtml(t->columnAlignment(i)));
                        m_html.push_back(Trait::latin1ToString(" dir=\"auto\">\n"));
                    }

                    this->onTableCell(c->get());

                    if (!m_justCollectFootnoteRefs) {
                        m_html.push_back(Trait::latin1ToString("\n</td>\n"));
                    }

                    ++i;

                    if (i == columns) {
                        break;
                    }
                }

                if (!m_justCollectFootnoteRefs) {
                    for (; i < columns; ++i) {
                        m_html.push_back(Trait::latin1ToString("<td dir=\"auto\"></td>"));
                    }

                    m_html.push_back(Trait::latin1ToString("\n</tr>\n"));
                }
            }

            if (!m_justCollectFootnoteRefs) {
                m_html.push_back(Trait::latin1ToString("</tbody></table>"));
            }
        }

        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("\n"));
        }
    }

    /*!
     * Handle anchor.
     *
     * \a a Anchor.
     */
    void onAnchor(Anchor<Trait> *a) override
    {
        if (!m_justCollectFootnoteRefs && m_isWrappedInArticle) {
            m_html.push_back(Trait::latin1ToString("\n<div id=\""));
            m_html.push_back(a->label());
            m_html.push_back(Trait::latin1ToString("\"></div>\n"));
        }
    }

    /*!
     * Handle raw HTML.
     *
     * \a h Raw HTML.
     */
    void onRawHtml(RawHtml<Trait> *h) override
    {
        if (!m_justCollectFootnoteRefs) {
            openStyle(h->openStyles());

            m_html.push_back(h->text());

            closeStyle(h->closeStyles());
        }
    }

    /*!
     * Handle horizontal line.
     */
    void onHorizontalLine(HorizontalLine<Trait> *) override
    {
        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("<hr />"));
        }
    }

    /*!
     * Handle link.
     *
     * \a l Link.
     */
    void onLink(Link<Trait> *l) override
    {
        typename Trait::String url = l->url();

        const auto lit = this->m_doc->labeledLinks().find(url);

        if (lit != this->m_doc->labeledLinks().cend()) {
            url = lit->second->url();
        }

        if (std::find(this->m_anchors.cbegin(), this->m_anchors.cend(), url) != this->m_anchors.cend()) {
            url = Trait::latin1ToString("#") + url;
        } else if (url.startsWith(Trait::latin1ToString("#"))) {
            const auto it = this->m_doc->labeledHeadings().find(url);

            if (it == this->m_doc->labeledHeadings().cend()) {
                auto path = static_cast<Anchor<Trait> *>(this->m_doc->items().at(0).get())->label();
                const auto sp = path.lastIndexOf(Trait::latin1ToString("/"));
                path.remove(sp, path.length() - sp);
                const auto p = url.indexOf(path) - 1;
                url.remove(p, url.length() - p);
            } else {
                url = it->second->label();
            }
        }

        if (!m_justCollectFootnoteRefs) {
            openStyle(l->openStyles());

            m_html.push_back(Trait::latin1ToString("<a href=\""));
            m_html.push_back(url);
            m_html.push_back(Trait::latin1ToString("\""));
            printId(l);
            m_html.push_back(Trait::latin1ToString(">"));
        }

        if (l->p() && !l->p()->isEmpty()) {
            onParagraph(l->p().get(), false);
        } else if (!l->img()->isEmpty()) {
            if (!m_justCollectFootnoteRefs) {
                onImage(l->img().get());
            }
        } else if (!l->text().isEmpty()) {
            if (!m_justCollectFootnoteRefs) {
                m_html.push_back(prepareTextForHtml(l->text()));
            }
        } else {
            if (!m_justCollectFootnoteRefs) {
                m_html.push_back(prepareTextForHtml(l->url()));
            }
        }

        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("</a>"));

            closeStyle(l->closeStyles());
        }
    }

    /*!
     * Handle image.
     *
     * \a i Image.
     */
    void onImage(Image<Trait> *i) override
    {
        if (!m_justCollectFootnoteRefs) {
            openStyle(i->openStyles());

            m_html.push_back(Trait::latin1ToString("<img src=\""));
            m_html.push_back(i->url());
            m_html.push_back(Trait::latin1ToString("\" alt=\""));
            m_html.push_back(prepareTextForHtml(i->text()));
            m_html.push_back(Trait::latin1ToString("\" style=\"max-width:100%;\""));
            printId(i);
            m_html.push_back(Trait::latin1ToString(" />"));

            closeStyle(i->closeStyles());
        }
    }

    /*!
     * Handle footnote reference.
     *
     * \a ref Footnote reference.
     */
    void onFootnoteRef(FootnoteRef<Trait> *ref) override
    {
        const auto fit = this->m_doc->footnotesMap().find(ref->id());

        if (fit != this->m_doc->footnotesMap().cend()) {
            const auto r = std::find_if(m_fns.begin(), m_fns.end(), [&ref](const auto &stuff) {
                return ref->id() == stuff.m_id;
            });

            if (!m_justCollectFootnoteRefs) {
                openStyle(ref->openStyles());

                m_html.push_back(Trait::latin1ToString("<sup>"));
                m_html.push_back(Trait::latin1ToString("<a href=\"#"));
                m_html.push_back(ref->id());
                m_html.push_back(Trait::latin1ToString("\" id=\"ref-"));
                m_html.push_back(ref->id());
                m_html.push_back(Trait::latin1ToString("-"));
            }

            if (r == m_fns.end()) {
                if (!m_justCollectFootnoteRefs) {
                    m_html.push_back(Trait::latin1ToString("1"));
                }
            } else {
                if (!m_justCollectFootnoteRefs) {
                    m_html.push_back(Trait::latin1ToString(std::to_string(++(r->m_current)).c_str()));
                }

                if (!m_dontIncrementFootnoteCount) {
                    ++(r->m_count);
                }
            }

            if (!m_justCollectFootnoteRefs) {
                m_html.push_back(Trait::latin1ToString("\">"));
            }

            if (r == m_fns.end()) {
                if (!m_justCollectFootnoteRefs) {
                    m_html.push_back(Trait::latin1ToString(std::to_string(m_fns.size() + 1).c_str()));
                }

                m_fns.push_back({ref->id(), 1, 0});
            } else if (!m_justCollectFootnoteRefs) {
                m_html.push_back(Trait::latin1ToString(std::to_string(std::distance(m_fns.begin(), r) + 1).c_str()));
            }

            if (!m_justCollectFootnoteRefs) {
                m_html.push_back(Trait::latin1ToString("</a></sup>"));

                closeStyle(ref->closeStyles());
            }
        } else
            onText(static_cast<Text<Trait> *>(ref));
    }

    /*!
     * Handle list item.
     *
     * \a i List item.
     *
     * \a first Is this item first in the list?
     *
     * \a skipOpeningWrap Indicates that opening wrap should be added or no.
     */
    void onListItem(ListItem<Trait> *i, bool first, bool skipOpeningWrap = false) override
    {
        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("<li"));
            printId(i);

            if (i->isTaskList()) {
                skipOpeningWrap = Visitor<Trait>::wrapFirstParagraphInListItem(i);

                m_html.push_back(Trait::latin1ToString(" class=\"task-list-item\">"));

                if (skipOpeningWrap && !i->isEmpty() && i->items().at(0)->type() == MD::ItemType::Paragraph) {
                    m_html.push_back(Trait::latin1ToString("<p dir=\"auto\">"));
                }

                m_html.push_back(
                    Trait::latin1ToString("<input "
                                          "type=\"checkbox\" id=\"\" disabled=\"\" class=\"task-list-item-checkbox\""));

                if (i->isChecked()) {
                    m_html.push_back(Trait::latin1ToString(" checked=\"\""));
                }
            }

            if (i->listType() == ListItem<Trait>::Ordered && first) {
                m_html.push_back(Trait::latin1ToString(" value=\""));
                m_html.push_back(Trait::latin1ToString(std::to_string(i->startNumber()).c_str()));
                m_html.push_back(Trait::latin1ToString("\""));
            }

            m_html.push_back(Trait::latin1ToString(">\n"));
        }

        Visitor<Trait>::onListItem(i, first, skipOpeningWrap);

        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("</li>\n"));
        }
    }

    /*!
     * Handle heading.
     *
     * \a h Heading.
     *
     * \a ht Heading tag.
     */
    virtual void onHeading(Heading<Trait> *h, const typename Trait::String &ht)
    {
        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("<"));
            m_html.push_back(ht);
            m_html.push_back(headingIdToHtml(h));
            m_html.push_back(Trait::latin1ToString(" dir=\"auto\">"));
        }

        if (h->text().get()) {
            onParagraph(h->text().get(), false);
        }

        if (!m_justCollectFootnoteRefs) {
            m_html.push_back(Trait::latin1ToString("</"));
            m_html.push_back(ht);
            m_html.push_back(Trait::latin1ToString(">"));
        }
    }

    /*!
     * Handle footnotes.
     *
     * \a hrefForRefBackImage String that will be applied as URL for image for back link from footnote.
     */
    virtual void onFootnotes(const typename Trait::String &hrefForRefBackImage)
    {
        if (!m_fns.empty()) {
            m_html.push_back(Trait::latin1ToString("<section class=\"footnotes\"><ol dir=\"auto\">"));
        }

        int i = 1;

        // This copy is needed. First time we want to iterate on non-modified container.
        const auto tmpFns = m_fns;
        m_justCollectFootnoteRefs = true;

        for (const auto &id : tmpFns) {
            const auto fit = this->m_doc->footnotesMap().find(id.m_id);

            if (fit != this->m_doc->footnotesMap().cend()) {
                this->onFootnote(fit->second.get());
            }
        }

        m_justCollectFootnoteRefs = false;
        m_dontIncrementFootnoteCount = true;

        for (const auto &id : m_fns) {
            m_html.push_back(Trait::latin1ToString("<li id=\""));
            m_html.push_back(id.m_id);
            m_html.push_back(Trait::latin1ToString("\">"));
            ++i;

            const auto fit = this->m_doc->footnotesMap().find(id.m_id);

            if (fit != this->m_doc->footnotesMap().cend()) {
                this->onFootnote(fit->second.get());

                if (!hrefForRefBackImage.isEmpty()) {
                    typename Trait::String backRef;
                    long long int backRefPos = m_html.endsWith(Trait::latin1ToString("</p>")) ? 4 : 0;

                    for (long long int i = 1; i <= id.m_count; ++i) {
                        backRef.push_back(Trait::latin1ToString("<a href=\"#ref-"));
                        backRef.push_back(id.m_id);
                        backRef.push_back(Trait::latin1ToString("-"));
                        backRef.push_back(Trait::latin1ToString(std::to_string(i).c_str()));
                        backRef.push_back(Trait::latin1ToString("\"><img src=\""));
                        backRef.push_back(hrefForRefBackImage);
                        backRef.push_back(Trait::latin1ToString("\" /></a>"));
                    }

                    m_html.insert(m_html.length() - backRefPos, backRef);
                }

                m_html.push_back(Trait::latin1ToString("</li>"));
            }
        }

        if (!m_fns.empty()) {
            m_html.push_back(Trait::latin1ToString("</ol></section>\n"));
        }
    }

    /*!
     * Returns HTML content for heading's ID.
     *
     * \a h Heading.
     */
    virtual typename Trait::String headingIdToHtml(Heading<Trait> *h)
    {
        typename Trait::String html;

        if (h->isLabeled()) {
            html.push_back(Trait::latin1ToString(" id=\""));
            auto label = h->label();
            if (label.startsWith(Trait::latin1ToString("#"))) {
                label.remove(0, 1);
            }
            html.push_back(label);
            html.push_back(Trait::latin1ToString("\""));
        }

        return html;
    }

    /*!
     * Prepare text to insert into HTML content.
     *
     * \a t String.
     */
    virtual typename Trait::String prepareTextForHtml(const typename Trait::String &t)
    {
        auto tmp = t;
        tmp.replace(Trait::latin1ToChar('&'), Trait::latin1ToString("&amp;"));
        tmp.replace(Trait::latin1ToChar('<'), Trait::latin1ToString("&lt;"));
        tmp.replace(Trait::latin1ToChar('>'), Trait::latin1ToString("&gt;"));

        return tmp;
    }

    /*!
     * Returns HTML content for table alignment.
     *
     * \a a Alignment.
     */
    virtual typename Trait::String tableAlignmentToHtml(typename Table<Trait>::Alignment a)
    {
        typename Trait::String html;

        switch (a) {
        case Table<Trait>::AlignLeft:
            html.push_back(Trait::latin1ToString(" align=\"left\""));
            break;

        case Table<Trait>::AlignCenter:
            html.push_back(Trait::latin1ToString(" align=\"center\""));
            break;

        case Table<Trait>::AlignRight:
            html.push_back(Trait::latin1ToString(" align=\"right\""));
            break;

        default:
            break;
        }

        return html;
    }

protected:
    /*!
     * HTML content.
     */
    typename Trait::String m_html;
    /*!
     * Just collect footnote references?
     */
    bool m_justCollectFootnoteRefs = false;
    /*!
     * Just process footnote references and don't increment count number.
     */
    bool m_dontIncrementFootnoteCount = false;
    /*!
     * Is this HTML wrapped in artcile tag?
     */
    bool m_isWrappedInArticle = true;

    /*!
     * \class MD::details::HtmlVisitor::FootnoteRefStuff
     * \inmodule md4qt
     *
     * \brief Auxiliary struct to process footnotes.
     *
     * This structure is used to form ID of footnote reference in generated HTML.
     *
     * ID of footnote reference in HTML looks like "ref-{FOOTNOTE_ID}-{NUMBER_OF_REFERENCE_WITH_THIS_FOOTNOTE_ID}"
     */
    struct FootnoteRefStuff {
        /*!
         * ID of footnote.
         *
         * This ID places to {FOOTNOTE_ID}.
         */
        typename Trait::String m_id;
        /*!
         * Count of references with this ID.
         *
         * Used to generate back links from footnote to footnote reference.
         */
        long long int m_count = 0;
        /*!
         * Number of footnote reference with this ID.
         *
         * This number places to {NUMBER_OF_REFERENCE_WITH_THIS_FOOTNOTE_ID}.
         */
        long long int m_current = 0;
    };

    /*!
     * Vector of processed footnotes references.
     */
    typename Trait::template Vector<FootnoteRefStuff> m_fns;
    /*!
     * Map of IDs to set to corresponding items.
     */
    const IdsMap<Trait> *m_idsMap = nullptr;
}; // class HtmlVisitor

} /* namespace details */

/*!
 * \inheaderfile md4qt/html.h
 *
 * \brief Convert Document to HTML.
 *
 * \a doc Markdown document.
 *
 * \a wrapInBodyTag Wrap HTML into <body> tag?
 *
 * \a hrefForRefBackImage String that will be applied as URL for image for back link from footnote.
 *
 * \a wrapInArticle Wrap HTML with <article> tag?
 *
 * \a idsMap Map of IDs to set to items.
 */
template<class Trait, class HtmlVisitor = details::HtmlVisitor<Trait>>
typename Trait::String toHtml(std::shared_ptr<Document<Trait>> doc,
                              bool wrapInBodyTag = true,
                              const typename Trait::String &hrefForRefBackImage = {},
                              bool wrapInArticle = true,
                              const details::IdsMap<Trait> *idsMap = nullptr)
{
    typename Trait::String html;

    if (wrapInBodyTag) {
        html.push_back(Trait::latin1ToString("<!DOCTYPE html>\n<html><head></head><body>\n"));
    }

    if (wrapInArticle) {
        html.push_back(Trait::latin1ToString("<article class=\"markdown-body\">"));
    }

    HtmlVisitor visitor;

    html.push_back(visitor.toHtml(doc, hrefForRefBackImage, wrapInArticle, idsMap));

    if (wrapInArticle) {
        html.push_back(Trait::latin1ToString("</article>\n"));
    }

    if (wrapInBodyTag) {
        html.push_back(Trait::latin1ToString("</body></html>\n"));
    }

    return html;
}

} /* namespace MD */

#endif // MD4QT_MD_HTML_HPP_INCLUDED
