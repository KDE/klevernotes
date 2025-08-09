/*
    SPDX-FileCopyrightText: 2022-2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: MIT
*/

#ifndef MD4QT_MD_POSCACHE_HPP_INCLUDED
#define MD4QT_MD_POSCACHE_HPP_INCLUDED

#include "visitor.h"

// C++ include.
#include <algorithm>
#include <cassert>
#include <memory>
#include <stack>
#include <vector>

namespace MD
{

namespace details
{

//
// PosRange
//

/*!
 * \class MD::details::PosRange
 * \inmodule md4qt
 * \inheaderfile md4qt/poscache.h
 *
 * \brief Cached position of MD::Item.
 *
 * Just a data structure to hold information about positions and pointers to items.
 * Purely for internal use, a developer should not have a need to work with this structure.
 */
template<class Trait>
struct PosRange {
    /*!
     * Initializing constructor.
     *
     * \a startColumn Start column.
     *
     * \a startLine Start line.
     *
     * \a endColumn End column.
     *
     * \a endLine End line.
     */
    PosRange(long long int startColumn, long long int startLine, long long int endColumn, long long int endLine)
        : m_startColumn(startColumn)
        , m_startLine(startLine)
        , m_endColumn(endColumn)
        , m_endLine(endLine)
    {
    }

    /*!
     * Initializing constructor.
     *
     * \a startColumn Start column.
     *
     * \a startLine Start line.
     *
     * \a endColumn End column.
     *
     * \a endLine End line.
     *
     * \a item This item.
     */
    PosRange(long long int startColumn, long long int startLine, long long int endColumn, long long int endLine, Item<Trait> *item)
        : m_startColumn(startColumn)
        , m_startLine(startLine)
        , m_endColumn(endColumn)
        , m_endLine(endLine)
        , m_item(item)
    {
    }

    /*!
     * Initializing constructor.
     *
     * \a startColumn Start column.
     *
     * \a startLine Start line.
     *
     * \a endColumn End column.
     *
     * \a endLine End line.
     *
     * \a item This item.
     *
     * \a children Children items.
     */
    PosRange(long long int startColumn,
             long long int startLine,
             long long int endColumn,
             long long int endLine,
             Item<Trait> *item,
             const std::vector<std::shared_ptr<PosRange<Trait>>> &children)
        : m_startColumn(startColumn)
        , m_startLine(startLine)
        , m_endColumn(endColumn)
        , m_endLine(endLine)
        , m_item(item)
        , m_children(children)
    {
    }

    /*!
     * Start column
     */
    long long int m_startColumn = -1;
    /*!
     * Start line.
     */
    long long int m_startLine = -1;
    /*!
     * End column.
     */
    long long int m_endColumn = -1;
    /*!
     * End line.
     */
    long long int m_endLine = -1;
    /*!
     * Pointer to this item.
     */
    Item<Trait> *m_item = nullptr;
    /*!
     * List of children.
     */
    std::vector<std::shared_ptr<PosRange<Trait>>> m_children = {};

    /*!
     * Returns whether position valid.
     */
    inline bool isValidPos() const
    {
        return m_startColumn > -1 && m_startLine > -1 && m_endColumn > -1 && m_endLine > -1;
    }
};

// Look at this equality operator like on rectangles intersection.
// If rectangles in text intersect then rectangles are equal.
/*!
 * \relates MD::details::PosRange
 * \inheaderfile md4qt/poscache.h
 *
 * Returns whether both are equal.
 *
 * \a l Left operand.
 *
 * \a r Right operand.
 */
template<class Trait>
bool operator==(const PosRange<Trait> &l, const PosRange<Trait> &r)
{
    return (l.m_startLine <= r.m_endLine && l.m_endLine >= r.m_startLine
            && (l.m_startLine == r.m_endLine && l.m_endLine == r.m_startLine ? l.m_endColumn >= r.m_startColumn && l.m_startColumn <= r.m_endColumn : true));
}

/*!
 * \relates MD::details::PosRange
 * \inheaderfile md4qt/poscache.h
 *
 * Returns whether left is less of right.
 *
 * \a l Left operand.
 *
 * \a r Right operand.
 */
template<class Trait>
bool operator<(const PosRange<Trait> &l, const PosRange<Trait> &r)
{
    return (l.m_endLine < r.m_startLine || (l.m_endLine == r.m_startLine && l.m_endColumn < r.m_startColumn));
}

} /* namespace details */

template<class Trait>
bool comparePosRangeLower(const std::shared_ptr<details::PosRange<Trait>> &ptr, const details::PosRange<Trait> &range)
{
    return (*ptr.get() < range);
}

template<class Trait>
bool comparePosRangeUpper(const details::PosRange<Trait> &range, const std::shared_ptr<details::PosRange<Trait>> &ptr)
{
    return (range < *ptr.get());
}

//
// PosCache
//

/*!
 * \class MD::PosCache
 * \inmodule md4qt
 * \inheaderfile md4qt/poscache.h
 *
 * \brief Cache of Markdown items to be accessed via position.
 *
 * A visitor that during walking through a document gathers information about positions of items
 * and stores it internally. When positions cache is initialized a developer can search for items
 * by its positions with MD::PosCache::findFirstInCache method.
 *
 * A complexity of walking is O(N), whereas searching is LOG(N).
 *
 * \sa MD::Visitor
 */
template<class Trait>
class PosCache : public MD::Visitor<Trait>
{
public:
    /*!
     * Default constructor.
     */
    PosCache() = default;
    ~PosCache() override = default;

    /*!
     * Initialize m_cache with the given document.
     *
     * \note Document should not be recursive.
     *
     * \a doc Document.
     */
    virtual void initialize(std::shared_ptr<MD::Document<Trait>> doc)
    {
        m_cache.clear();

        if (doc) {
            Visitor<Trait>::process(doc);

            for (auto it = doc->footnotesMap().cbegin(), last = doc->footnotesMap().cend(); it != last; ++it) {
                onFootnote(it->second.get());
            }

            for (auto it = doc->labeledLinks().cbegin(), last = doc->labeledLinks().cend(); it != last; ++it) {
                onReferenceLink(it->second.get());
            }
        }
    }

    /*!
     * \class MD::PosCache::Items
     * \inmodule md4qt
     *
     * \brief Vector of items.
     *
     * Vector with items, where front is a top-level item,
     * and back is most nested child.
     */
    using Items = typename Trait::template Vector<Item<Trait> *>;

    /*!
     * Returns first occurence of Markdown item with all first children by the given position.
     *
     * \a pos Position.
     */
    Items findFirstInCache(const MD::WithPosition &pos) const
    {
        Items res;

        details::PosRange<Trait> tmp{pos.startColumn(), pos.startLine(), pos.endColumn(), pos.endLine()};

        findFirstInCache(m_cache, tmp, res);

        return res;
    }

protected:
    /*!
     * Find in cache an item with the given position.
     *
     * \a vec Cache of position.
     *
     * \a pos Position of sought-for item.
     *
     * \a ordered Indicates that we sure that searching item places after everything.
     */
    details::PosRange<Trait> *
    findInCache(const std::vector<std::shared_ptr<details::PosRange<Trait>>> &vec, const details::PosRange<Trait> &pos, bool ordered = false) const
    {
        if (!m_currentItems.empty()) {
            return m_currentItems.top().get();
        } else if (ordered) {
            return nullptr;
        } else {
            const auto it = std::lower_bound(vec.begin(), vec.end(), pos, comparePosRangeLower<Trait>);

            if (it != vec.end() && *it->get() == pos) {
                if (!it->get()->m_children.empty()) {
                    auto nested = findInCache(it->get()->m_children, pos);

                    return (nested ? nested : it->get());
                } else {
                    return it->get();
                }
            } else {
                return nullptr;
            }
        }
    }

    /*!
     * Find in cache items with the given position with all parents.
     *
     * \a vec Cache.
     *
     * \a pos Position of sought-for item.
     *
     * \a res Reference to result of search.
     */
    void findFirstInCache(const std::vector<std::shared_ptr<details::PosRange<Trait>>> &vec, const details::PosRange<Trait> &pos, Items &res) const
    {
        const auto it = std::lower_bound(vec.begin(), vec.end(), pos, comparePosRangeLower<Trait>);

        if (it != vec.end() && *it->get() == pos) {
            res.push_back(it->get()->m_item);

            if (!it->get()->m_children.empty()) {
                findFirstInCache(it->get()->m_children, pos, res);
            }
        }
    }

    /*!
     * Insert in cache.
     *
     * \a item Position for insertion.
     *
     * \a sort Should we sord when insert top-level item, or we can be sure that this item is last?
     *
     * \a insertInStack Indicates that this item is some kind a block with children that should be
     *                  added into stack for fast finding parent item for the next children. This
     *                  techinque allows to have O(N) walking complexity.
     */
    void insertInCache(const details::PosRange<Trait> &item, bool sort = false, bool insertInStack = false)
    {
        const auto insertInStackFunc = [this, insertInStack](std::shared_ptr<details::PosRange<Trait>> item) {
            if (insertInStack) {
                this->m_currentItems.push(item);
            }
        };

        static const auto makeSharedPosRange = [](const details::PosRange<Trait> &range) -> std::shared_ptr<details::PosRange<Trait>> {
            return std::make_shared<details::PosRange<Trait>>(range.m_startColumn,
                                                              range.m_startLine,
                                                              range.m_endColumn,
                                                              range.m_endLine,
                                                              range.m_item,
                                                              range.m_children);
        };

        if (!m_skipInCache) {
            assert(item.isValidPos());

            auto pos = findInCache(m_cache, item, !sort);

            if (pos) {
                pos->m_children.push_back(makeSharedPosRange(item));

                insertInStackFunc(pos->m_children.back());
            } else {
                if (sort) {
                    const auto it = std::upper_bound(m_cache.begin(), m_cache.end(), item, comparePosRangeUpper<Trait>);

                    if (it != m_cache.end()) {
                        insertInStackFunc(*m_cache.insert(it, makeSharedPosRange(item)));
                    } else {
                        m_cache.push_back(makeSharedPosRange(item));

                        insertInStackFunc(m_cache.back());
                    }
                } else {
                    m_cache.push_back(makeSharedPosRange(item));

                    insertInStackFunc(m_cache.back());
                }
            }
        }
    }

protected:
    /*!
     * Cache user defined item.
     *
     * \a i Item.
     */
    void onUserDefined(Item<Trait> *i) override
    {
        details::PosRange<Trait> r{i->startColumn(), i->startLine(), i->endColumn(), i->endLine(), i};

        insertInCache(r);
    }

    /*!
     * Cache shortcut link.
     *
     * \a l Link.
     */
    virtual void onReferenceLink(Link<Trait> *l)
    {
        details::PosRange<Trait> r{l->startColumn(), l->startLine(), l->endColumn(), l->endLine(), l};

        insertInCache(r, true);
    }

    void onAddLineEnding() override
    {
    }

    /*!
     * Cache text item.
     *
     * \a t Text.
     */
    void onText(Text<Trait> *t) override
    {
        details::PosRange<Trait> r{t->openStyles().empty() ? t->startColumn() : t->openStyles().front().startColumn(),
                                   t->startLine(),
                                   t->closeStyles().empty() ? t->endColumn() : t->closeStyles().back().endColumn(),
                                   t->endLine(),
                                   t};

        insertInCache(r);
    }

    /*!
     * Cache LaTeX math expression.
     *
     * \a m Math.
     */
    void onMath(Math<Trait> *m) override
    {
        auto startColumn = m->startDelim().startColumn();
        auto startLine = m->startDelim().startLine();
        auto endColumn = m->endDelim().endColumn();
        auto endLine = m->endDelim().endLine();

        if (!m->openStyles().empty()) {
            startColumn = m->openStyles().front().startColumn();
            startLine = m->openStyles().front().startLine();
        }

        if (!m->closeStyles().empty()) {
            endColumn = m->closeStyles().back().endColumn();
            endLine = m->closeStyles().back().endLine();
        }

        details::PosRange<Trait> r{startColumn, startLine, endColumn, endLine, m};

        insertInCache(r);
    }

    void onLineBreak(LineBreak<Trait> *) override
    {
    }

    /*!
     * Cache paragraph.
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
        details::PosRange<Trait> r{p->startColumn(), p->startLine(), p->endColumn(), p->endLine(), p};

        insertInCache(r, false, true);

        Visitor<Trait>::onParagraph(p, wrap, skipOpeningWrap);

        if (!m_skipInCache) {
            m_currentItems.pop();
        }
    }

    /*!
     * Cache heading.
     *
     * \a h Heading.
     */
    void onHeading(Heading<Trait> *h) override
    {
        details::PosRange<Trait> r{h->startColumn(), h->startLine(), h->endColumn(), h->endLine(), h};

        insertInCache(r, false, true);

        if (h->text() && !h->text()->isEmpty()) {
            onParagraph(h->text().get(), false);
        }

        m_currentItems.pop();
    }

    /*!
     * Cache code.
     *
     * \a c Code.
     */
    void onCode(Code<Trait> *c) override
    {
        auto startColumn = c->isFensedCode() ? c->startDelim().startColumn() : c->startColumn();
        auto startLine = c->isFensedCode() ? c->startDelim().startLine() : c->startLine();
        auto endColumn = c->isFensedCode() && c->endDelim().endColumn() > -1 ? c->endDelim().endColumn() : c->endColumn();
        auto endLine = c->isFensedCode() && c->endDelim().endLine() > -1 ? c->endDelim().endLine() : c->endLine();

        details::PosRange<Trait> r{startColumn, startLine, endColumn, endLine, c};

        insertInCache(r);
    }

    /*!
     * Cache inline code.
     *
     * \a c Code.
     */
    void onInlineCode(Code<Trait> *c) override
    {
        auto startColumn = c->startDelim().startColumn();
        auto startLine = c->startDelim().startLine();
        auto endColumn = c->endDelim().endColumn();
        auto endLine = c->endDelim().endLine();

        if (!c->openStyles().empty()) {
            startColumn = c->openStyles().front().startColumn();
            startLine = c->openStyles().front().startLine();
        }

        if (!c->closeStyles().empty()) {
            endColumn = c->closeStyles().back().endColumn();
            endLine = c->closeStyles().back().endLine();
        }

        details::PosRange<Trait> r{startColumn, startLine, endColumn, endLine, c};

        insertInCache(r);
    }

    /*!
     * Cache blockquote.
     *
     * \a b Blockquote.
     */
    void onBlockquote(Blockquote<Trait> *b) override
    {
        details::PosRange<Trait> r{b->startColumn(), b->startLine(), b->endColumn(), b->endLine(), b};

        insertInCache(r, false, true);

        Visitor<Trait>::onBlockquote(b);

        m_currentItems.pop();
    }

    /*!
     * Cache list.
     *
     * \a l List.
     */
    void onList(List<Trait> *l) override
    {
        bool first = true;

        details::PosRange<Trait> r{l->startColumn(), l->startLine(), l->endColumn(), l->endLine(), l};

        insertInCache(r, false, true);

        for (auto it = l->items().cbegin(), last = l->items().cend(); it != last; ++it) {
            if ((*it)->type() == ItemType::ListItem) {
                onListItem(static_cast<ListItem<Trait> *>(it->get()), first);

                first = false;
            }
        }

        m_currentItems.pop();
    }

    /*!
     * Cache table.
     *
     * \a t Table.
     */
    void onTable(Table<Trait> *t) override
    {
        details::PosRange<Trait> r{t->startColumn(), t->startLine(), t->endColumn(), t->endLine(), t};

        insertInCache(r, false, true);

        if (!t->isEmpty()) {
            int columns = 0;

            for (auto th = (*t->rows().cbegin())->cells().cbegin(), last = (*t->rows().cbegin())->cells().cend(); th != last; ++th) {
                Visitor<Trait>::onTableCell(th->get());

                ++columns;
            }

            for (auto r = std::next(t->rows().cbegin()), rlast = t->rows().cend(); r != rlast; ++r) {
                int i = 0;

                for (auto c = (*r)->cells().cbegin(), clast = (*r)->cells().cend(); c != clast; ++c) {
                    Visitor<Trait>::onTableCell(c->get());

                    ++i;

                    if (i == columns) {
                        break;
                    }
                }
            }
        }

        m_currentItems.pop();
    }

    void onAnchor(Anchor<Trait> *) override
    {
    }

    /*!
     * Cache raw HTML.
     *
     * \a h Raw HTML.
     */
    void onRawHtml(RawHtml<Trait> *h) override
    {
        details::PosRange<Trait> r{h->startColumn(), h->startLine(), h->endColumn(), h->endLine(), h};

        insertInCache(r);
    }

    /*!
     * Cache horizontal line.
     *
     * \a l Horizontal line.
     */
    void onHorizontalLine(HorizontalLine<Trait> *l) override
    {
        details::PosRange<Trait> r{l->startColumn(), l->startLine(), l->endColumn(), l->endLine(), l};

        insertInCache(r);
    }

    /*!
     * Cache link.
     *
     * \a l Link.
     */
    void onLink(Link<Trait> *l) override
    {
        auto startColumn = l->startColumn();
        auto startLine = l->startLine();
        auto endColumn = l->endColumn();
        auto endLine = l->endLine();

        if (!l->openStyles().empty()) {
            startColumn = l->openStyles().front().startColumn();
            startLine = l->openStyles().front().startLine();
        }

        if (!l->closeStyles().empty()) {
            endColumn = l->closeStyles().back().endColumn();
            endLine = l->closeStyles().back().endLine();
        }

        details::PosRange<Trait> r{startColumn, startLine, endColumn, endLine, l};

        insertInCache(r);

        if (l->p()) {
            RollbackBool skipInCacheRollback(m_skipInCache, true);
            onParagraph(l->p().get(), true);
        }
    }

    /*!
     * Cache image.
     *
     * \a i Image.
     */
    void onImage(Image<Trait> *i) override
    {
        auto startColumn = i->startColumn();
        auto startLine = i->startLine();
        auto endColumn = i->endColumn();
        auto endLine = i->endLine();

        if (!i->openStyles().empty()) {
            startColumn = i->openStyles().front().startColumn();
            startLine = i->openStyles().front().startLine();
        }

        if (!i->closeStyles().empty()) {
            endColumn = i->closeStyles().back().endColumn();
            endLine = i->closeStyles().back().endLine();
        }

        details::PosRange<Trait> r{startColumn, startLine, endColumn, endLine, i};

        insertInCache(r);

        if (i->p()) {
            RollbackBool skipInCacheRollback(m_skipInCache, true);
            onParagraph(i->p().get(), true);
        }
    }

    /*!
     * Cache footnote reference.
     *
     * \a ref Footnote reference.
     */
    void onFootnoteRef(FootnoteRef<Trait> *ref) override
    {
        auto startColumn = ref->startColumn();
        auto startLine = ref->startLine();
        auto endColumn = ref->endColumn();
        auto endLine = ref->endLine();

        if (!ref->openStyles().empty()) {
            startColumn = ref->openStyles().front().startColumn();
            startLine = ref->openStyles().front().startLine();
        }

        if (!ref->closeStyles().empty()) {
            endColumn = ref->closeStyles().back().endColumn();
            endLine = ref->closeStyles().back().endLine();
        }

        details::PosRange<Trait> r{startColumn, startLine, endColumn, endLine, ref};

        insertInCache(r);
    }

    /*!
     * Cache footnote.
     *
     * \a f Footnote.
     */
    void onFootnote(Footnote<Trait> *f) override
    {
        details::PosRange<Trait> r{f->startColumn(), f->startLine(), f->endColumn(), f->endLine(), f};

        insertInCache(r, true, true);

        Visitor<Trait>::onFootnote(f);

        m_currentItems.pop();
    }

    /*!
     * Cache list item.
     *
     * \a l List item.
     *
     * \a first Is this item first in the list?
     *
     * \a skipOpeningWrap Indicates that opening wrap should be added or no.
     */
    void onListItem(ListItem<Trait> *l, bool first, bool skipOpeningWrap = false) override
    {
        details::PosRange<Trait> r{l->startColumn(), l->startLine(), l->endColumn(), l->endLine(), l};

        insertInCache(r, false, true);

        Visitor<Trait>::onListItem(l, first, skipOpeningWrap);

        m_currentItems.pop();
    }

protected:
    /*!
     * Cache.
     */
    std::vector<std::shared_ptr<details::PosRange<Trait>>> m_cache;
    /*!
     * Skip adding in cache.
     */
    bool m_skipInCache = false;

private:
    std::stack<std::shared_ptr<details::PosRange<Trait>>> m_currentItems;

    struct RollbackBool {
        RollbackBool(bool &variable, bool newValue)
            : m_variable(variable)
            , m_prevValue(variable)
        {
            m_variable = newValue;
        }

        ~RollbackBool()
        {
            m_variable = m_prevValue;
        }

    private:
        bool &m_variable;
        bool m_prevValue;
    }; // struct RollbackBool
}; // class PosCache

} /* namespace MD */

#endif // MD4QT_MD_POSCACHE_HPP_INCLUDED
