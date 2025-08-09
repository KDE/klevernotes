/*
    SPDX-FileCopyrightText: 2022-2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: MIT
*/

#ifndef MD4QT_MD_DOC_H_INCLUDED
#define MD4QT_MD_DOC_H_INCLUDED

// md4qt include.
#include "utils.h"

// C++ include.
#include <memory>
#include <utility>

namespace MD
{

//
// ItemType
//

/*!
 * \enum MD::ItemType
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Enumeration of item types.
 *
 * \value Heading Heading.
 * \value Text Text.
 * \value Paragraph Paragraph.
 * \value LineBreak Line break.
 * \value Blockquote Blockquote.
 * \value ListItem List item.
 * \value List List.
 * \value Link Link.
 * \value Image Image.
 * \value Code Code.
 * \value TableCell Table cell.
 * \value TableRow Table row.
 * \value Table Table.
 * \value FootnoteRef Footnote ref.
 * \value Footnote Footnote.
 * \value Document Document.
 * \value PageBreak Page break.
 * \value Anchor Anchor.
 * \value HorizontalLine Horizontal line.
 * \value RawHtml Raw HTML.
 * \value Math Math expression.
 * \value UserDefined Start item for user-defined types.
 */
enum class ItemType : int {
    Heading = 0,
    Text,
    Paragraph,
    LineBreak,
    Blockquote,
    ListItem,
    List,
    Link,
    Image,
    Code,
    TableCell,
    TableRow,
    Table,
    FootnoteRef,
    Footnote,
    Document,
    PageBreak,
    Anchor,
    HorizontalLine,
    RawHtml,
    Math,
    UserDefined = 255
}; // enum class ItemType

//
// WithPosition
//
/*!
 * \class MD::WithPosition
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Base for any thing with start and end position.
 *
 * Such information may be needed for syntax highlighters, for example.
 *
 * \note Start position of the document has coordinates (0, 0).
 */
class WithPosition
{
public:
    /*!
     * Default constructor.
     *
     * All positions will be set to -1.
     */
    WithPosition() = default;
    virtual ~WithPosition() = default;

    /*!
     * Initializing constructor with all positions.
     *
     * \a startColumn Start column.
     *
     * \a startLine Start line.
     *
     * \a endColumn End column.
     *
     * \a endLine End line.
     */
    WithPosition(long long int startColumn, long long int startLine, long long int endColumn, long long int endLine)
        : m_startColumn(startColumn)
        , m_startLine(startLine)
        , m_endColumn(endColumn)
        , m_endLine(endLine)
    {
    }

    /*!
     * Apply positions to this from other.
     *
     * \a other Positions to apply.
     */
    void applyPositions(const WithPosition &other)
    {
        if (this != &other) {
            *this = other;
        }
    }

    /*!
     * Returns start column.
     */
    long long int startColumn() const
    {
        return m_startColumn;
    }

    /*!
     * Returns start line.
     */
    long long int startLine() const
    {
        return m_startLine;
    }

    /*!
     * Returns end column.
     */
    long long int endColumn() const
    {
        return m_endColumn;
    }

    /*!
     * Returns end line.
     */
    long long int endLine() const
    {
        return m_endLine;
    }

    /*!
     * Set start column.
     *
     * \a c New value.
     */
    void setStartColumn(long long int c)
    {
        m_startColumn = c;
    }

    /*!
     * Set start line.
     *
     * \a l New value.
     */
    void setStartLine(long long int l)
    {
        m_startLine = l;
    }

    /*!
     * Set end column.
     *
     * \a c New value.
     */
    void setEndColumn(long long int c)
    {
        m_endColumn = c;
    }

    /*!
     * Set end line.
     *
     * \a l New value.
     */
    void setEndLine(long long int l)
    {
        m_endLine = l;
    }

private:
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
}; // class WithPosition

/*!
 * \relates MD::WithPosition
 * \inheaderfile md4qt/doc.h
 *
 * Returns whether both are equal.
 *
 * \a l Left operand.
 *
 * \a r Right operand.
 */
inline bool operator==(const WithPosition &l, const WithPosition &r)
{
    return (l.startColumn() == r.startColumn() && l.startLine() == r.startLine() && l.endColumn() == r.endColumn() && l.endLine() == r.endLine());
}

template<class Trait>
class Document;

//
// Item
//

/*!
 * \class MD::Item
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Base class for item in Markdown document.
 *
 * All items in MD::Document derived from this class. Main thing that this class does
 * is a virtual method MD::Item::type that return type of the item, so when working
 * with MD::Document a developer can know what type of item he has a pointer to.
 */
template<class Trait>
class Item : public WithPosition
{
protected:
    /*!
     * Default constructor.
     */
    Item() = default;

public:
    ~Item() override = default;

    /*!
     * Returns type of the item.
     */
    virtual ItemType type() const = 0;

    /*!
     * Clone this item.
     *
     * \a doc Parent of new item.
     */
    virtual std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const = 0;

private:
    MD_DISABLE_COPY(Item)
}; // class Item

//
// TextOption
//

/*!
 * \enum MD::TextOption
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Text option.
 *
 * \value TextWithoutFormat No format.
 * \value BoldText Bold text.
 * \value ItalicText Italic text.
 * \value StrikethroughText Strikethrough.
 */
enum TextOption {
    TextWithoutFormat = 0,
    BoldText = 1,
    ItalicText = 2,
    StrikethroughText = 4
}; // enum TextOption

//
// StyleDelim
//

/*!
 * \class MD::StyleDelim
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Emphasis in the Markdown document.
 *
 * Information about where empasises start and end is needed for syntax highlighters.
 * Any item in MD::Paragraph is MD::ItemWithOpts that stores information about
 * openers and closer of emphasises if they are presented before/after the given item
 * in paragraph. A developer may just check MD::ItemWithOpts::openStyles and
 * MD::ItemWithOpts::closeStyles for data in them, and he will know all information
 * about delimiters that started or ended style of the item.
 *
 * Such information may be needed for developing additional styles (plugins), superscripts, subscripts,
 * for example.
 *
 * \sa MD::ItemWithOpts
 */
class StyleDelim final : public WithPosition
{
public:
    /*!
     * Initializing constructor.
     *
     * \a s Style. Actualy it's an ORed combination of MD::TextOption.
     *
     * \a startColumn Start column.
     *
     * \a startLine Start line.
     *
     * \a endColumn End column.
     *
     * \a endLine End line.
     */
    StyleDelim(int s, long long int startColumn, long long int startLine, long long int endColumn, long long int endLine)
        : WithPosition(startColumn, startLine, endColumn, endLine)
        , m_style(s)
    {
    }

    ~StyleDelim() override = default;

    /*!
     * Returns style.
     */
    int style() const
    {
        return m_style;
    }

    /*!
     * Set style.
     *
     * \a t New value.
     */
    void setStyle(int t)
    {
        m_style = t;
    }

private:
    int m_style = TextWithoutFormat;
}; // class StyleDelim

/*!
 * \relates MD::StyleDelim
 * \inheaderfile md4qt/doc.h
 *
 * \brief Returns whether both are equal.
 *
 * \a l Left operand.
 *
 * \a r Right operand.
 */
inline bool operator==(const StyleDelim &l, const StyleDelim &r)
{
    return (static_cast<WithPosition>(l) == static_cast<WithPosition>(r) && l.style() == r.style());
}

//
// ItemWithOpts
//

/*!
 * \class MD::ItemWithOpts
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Base class for items with style options.
 *
 * Base class for items that can have style options.
 * These are all items in Paragraph.
 */
template<class Trait>
class ItemWithOpts : public Item<Trait>
{
protected:
    /*!
     * Default constructor.
     */
    ItemWithOpts() = default;

public:
    ~ItemWithOpts() override = default;

    /*!
     * Apply other item with options to this.
     *
     * \a other Value to apply.
     */
    void applyItemWithOpts(const ItemWithOpts<Trait> &other)
    {
        if (this != &other) {
            WithPosition::applyPositions(other);
            m_opts = other.m_opts;
            m_openStyles = other.m_openStyles;
            m_closeStyles = other.m_closeStyles;
        }
    }

    /*!
     * \typealias MD::ItemWithOpts::Styles
     *
     * Type of list of emphasis.
     */
    using Styles = typename Trait::template Vector<StyleDelim>;

    /*!
     * Returns style options.
     */
    int opts() const
    {
        return m_opts;
    }

    /*!
     * Set style options.
     *
     * \a o New value.
     */
    void setOpts(int o)
    {
        m_opts = o;
    }

    /*!
     * Returns list of all opening emphasises.
     */
    const Styles &openStyles() const
    {
        return m_openStyles;
    }

    /*!
     * Returns list of all opening emphasises.
     */
    Styles &openStyles()
    {
        return m_openStyles;
    }

    /*!
     * Returns list of all closing emphasises.
     */
    const Styles &closeStyles() const
    {
        return m_closeStyles;
    }

    /*!
     * Returns list of all closing emphasises.
     */
    Styles &closeStyles()
    {
        return m_closeStyles;
    }

private:
    /*!
     * Style options.
     */
    int m_opts = 0;
    /*!
     * List of opening emphasises.
     */
    Styles m_openStyles;
    /*!
     * List of closing emphasises.
     */
    Styles m_closeStyles;

    MD_DISABLE_COPY(ItemWithOpts)
}; // class ItemWithOpts

//
// PageBreak
//

/*!
 * \class MD::PageBreak
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Page break.
 *
 * Like MD::Anchor starts a file in MD::Document, MD::PageBreak ends a file.
 * MD::PageBreak may appear in MD::Document only in recursive mode.
 *
 * \note The last file in the document does not have a page break.
 */
template<class Trait>
class PageBreak final : public Item<Trait>
{
public:
    /*!
     * Default constructor.
     */
    PageBreak() = default;
    ~PageBreak() override = default;

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::PageBreak;
    }

    /*!
     * Clone this page break.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        MD_UNUSED(doc)

        return std::make_shared<PageBreak<Trait>>();
    }

private:
    MD_DISABLE_COPY(PageBreak)
}; // class PageBreak

//
// HorizontalLine
//

/*!
 * \class MD::HorizontalLine
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Horizontal line.
 *
 * Thematic break in Markdown file. In HTML this is usually a \c {<hr />} tag.
 */
template<class Trait>
class HorizontalLine final : public Item<Trait>
{
public:
    /*!
     * Default constructor.
     */
    HorizontalLine() = default;
    ~HorizontalLine() override = default;

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::HorizontalLine;
    }

    /*!
     * Clone this horizontal line.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        MD_UNUSED(doc)

        auto h = std::make_shared<HorizontalLine<Trait>>();
        h->applyPositions(*this);

        return h;
    }

private:
    MD_DISABLE_COPY(HorizontalLine)
}; // class HorizontalLine

//
// Anchor
//

/*!
 * \class MD::Anchor
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Just an anchor.
 *
 * This library supports recursive Markdown parsing - parsed Markdown file may has links
 * to other Markdown files, that may be parsed recursively with the root file.
 * So in the resulting document can be represented more than one Markdown file. Each file in the
 * document starts with MD::Anchor, it just shows that during traversing through
 * the document you reached new file.
 */
template<class Trait>
class Anchor final : public Item<Trait>
{
public:
    /*!
     * Initializing constructor.
     *
     * \a l Label.
     */
    explicit Anchor(const typename Trait::String &l)
        : m_label(l)
    {
    }

    ~Anchor() override = default;

    /*!
     * Clone this anchor.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        MD_UNUSED(doc)

        return std::make_shared<Anchor<Trait>>(m_label);
    }

    /*!
     * Returns item type.
     */
    ItemType type() const override
    {
        return ItemType::Anchor;
    }

    /*!
     * Returns label of this anchor.
     */
    const typename Trait::String &label() const
    {
        return m_label;
    }

private:
    MD_DISABLE_COPY(Anchor)

    /*!
     * Label
     */
    typename Trait::String m_label;
}; // class Anchor

//
// RawHtml
//

/*!
 * \class MD::RawHtml
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Raw HTML.
 *
 * HTML injection in Markdown. When converting to HTML this item should be converted to HTML
 * by inserting returned by MD::RawHtml::text string.
 */
template<class Trait>
class RawHtml final : public ItemWithOpts<Trait>
{
public:
    /*!
     * Default constructor.
     */
    RawHtml() = default;
    ~RawHtml() override = default;

    /*!
     * Clone this raw HTML.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        MD_UNUSED(doc)

        auto h = std::make_shared<RawHtml<Trait>>();
        h->applyItemWithOpts(*this);
        h->setText(m_text);
        h->setFreeTag(m_isFreeTag);

        return h;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::RawHtml;
    }

    /*!
     * Returns HTML content.
     */
    const typename Trait::String &text() const
    {
        return m_text;
    }

    /*!
     * Set HTML content.
     *
     * \a t New value.
     */
    void setText(const typename Trait::String &t)
    {
        m_text = t;
    }

protected:
    template<class T>
    friend class Parser;

    template<class T>
    friend struct UnprotectedDocsMethods;

    /*!
     * Returns whether this HTML a free tag, not inline one.
     *
     * \note This method is for internal use only.
     */
    bool isFreeTag() const
    {
        return m_isFreeTag;
    }

    /*!
     * Set that this HTML is a free, not inline one.
     *
     * \note This method is for internal use only.
     *
     * \a on New value.
     */
    void setFreeTag(bool on = true)
    {
        m_isFreeTag = on;
    }

private:
    /*!
     * HTML content.
     */
    typename Trait::String m_text;
    /*!
     * Is this HTML a free tag, not inline one.
     */
    bool m_isFreeTag = true;

    MD_DISABLE_COPY(RawHtml)
}; // class RawHtml

//
// Text
//

/*!
 * \class MD::Text
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Text item in Paragraph.
 *
 * Regular text in paragraph. Information in these items in paragraph is a subject
 * for custom plugins implementation.
 */
template<typename Trait>
class Text : public ItemWithOpts<Trait>
{
public:
    /*!
     * Default constructor.
     */
    Text() = default;
    ~Text() override = default;

    /*!
     * Apply other text to this.
     *
     * \a t Value to apply.
     */
    void applyText(const Text<Trait> &t)
    {
        if (this != &t) {
            ItemWithOpts<Trait>::applyItemWithOpts(t);
            setText(t.text());
        }
    }

    /*!
     * Clone this text item.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        MD_UNUSED(doc)

        auto t = std::make_shared<Text<Trait>>();
        t->applyText(*this);

        return t;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::Text;
    }

    /*!
     * Returns text content.
     */
    const typename Trait::String &text() const
    {
        return m_text;
    }

    /*!
     * Set text content.
     *
     * \a t New value.
     */
    void setText(const typename Trait::String &t)
    {
        m_text = t;
    }

private:
    /*!
     * Text content.
     */
    typename Trait::String m_text;

    MD_DISABLE_COPY(Text)
}; // class Text

//
// LineBreak
//

/*!
 * \class MD::LineBreak
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Line break.
 *
 * Line break in Markdown. This item in HTML is a usual \c {<br />}.
 */
template<class Trait>
class LineBreak final : public Text<Trait>
{
public:
    /*!
     * Default constructor.
     */
    LineBreak() = default;
    ~LineBreak() override = default;

    /*!
     * Clone this line break.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        MD_UNUSED(doc)

        auto b = std::make_shared<LineBreak<Trait>>();
        b->applyText(*this);

        return b;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::LineBreak;
    }

private:
    MD_DISABLE_COPY(LineBreak)
}; // class LineBreak

//
// Block
//

/*!
 * \class MD::Block
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Abstract block (storage of child items).
 *
 * Base class for any item in MD::Document that may has children items.
 *
 * \sa MD::Paragraph, MD::Blockquote, MD::List, MD::ListItem, MD::TableCell, MD::Footnote.
 */
template<class Trait>
class Block : public Item<Trait>
{
protected:
    /*!
     * Default constructor.
     */
    Block() = default;

public:
    ~Block() override = default;

    /*!
     * \typealias MD::Block::ItemSharedPointer
     *
     * Type of pointer to child item.
     */
    using ItemSharedPointer = std::shared_ptr<Item<Trait>>;
    /*!
     * \typealias MD::Block::Items
     *
     * Type of list of children.
     */
    using Items = typename Trait::template Vector<ItemSharedPointer>;

    /*!
     * Apply other block to this.
     *
     * \a other Value to apply.
     *
     * \a doc Parent of new item.
     */
    void applyBlock(const Block<Trait> &other, Document<Trait> *doc = nullptr)
    {
        if (this != &other) {
            WithPosition::applyPositions(other);

            m_items.clear();

            for (const auto &i : other.items())
                appendItem(i->clone(doc));
        }
    }

    /*!
     * Returns list of child items.
     */
    const Items &items() const
    {
        return m_items;
    }

    /*!
     * Insert child item at given position.
     *
     * \a idx Index where insert.
     *
     * \a i Item to insert.
     */
    void insertItem(long long int idx, ItemSharedPointer i)
    {
        m_items.insert(m_items.cbegin() + idx, i);
    }

    /*!
     * Append child item.
     *
     * \a i Item to append.
     */
    void appendItem(ItemSharedPointer i)
    {
        m_items.push_back(i);
    }

    /*!
     * Remove child item at the given position.
     *
     * \a idx Index where to remove.
     */
    void removeItemAt(long long int idx)
    {
        if (idx >= 0 && idx < static_cast<long long int>(m_items.size()))
            m_items.erase(m_items.cbegin() + idx);
    }

    /*!
     * Returns child item at the given position.
     *
     * \a idx Index.
     */
    ItemSharedPointer getItemAt(long long int idx) const
    {
        return m_items.at(idx);
    }

    /*!
     * Returns whether there are no children.
     */
    bool isEmpty() const
    {
        return m_items.empty();
    }

private:
    /*!
     * Child items.
     */
    Items m_items;

    MD_DISABLE_COPY(Block)
}; // class Block

//
// Paragraph
//

/*!
 * \class MD::Paragraph
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Paragraph.
 *
 * Block of inline items, such as MD::Text, MD::Link, MD::Image,
 * MD::RawHtml, MD::Code, MD::Math, MD::LineBreak, MD::FootnoteRef.
 */
template<class Trait>
class Paragraph final : public Block<Trait>
{
public:
    /*!
     * Default constructor.
     */
    Paragraph() = default;
    ~Paragraph() override = default;

    /*!
     * Clone this paragraph.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        auto p = std::make_shared<Paragraph<Trait>>();
        p->applyBlock(*this, doc);

        return p;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::Paragraph;
    }

private:
    MD_DISABLE_COPY(Paragraph)
}; // class Paragraph

//
// Heading
//

/*!
 * \class MD::Heading
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Heading.
 *
 * Any heading in Markdown.
 */
template<class Trait>
class Heading final : public Item<Trait>
{
public:
    /*!
     * Default constructor.
     */
    Heading()
        : m_text(new Paragraph<Trait>)
    {
    }

    ~Heading() override = default;

    /*!
     * \typealias MD::Heading::Delims
     *
     * Type of list of service chanracters.
     */
    using Delims = typename Trait::template Vector<WithPosition>;

    /*!
     * Clone this heading.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        auto h = std::make_shared<Heading<Trait>>();
        h->applyPositions(*this);
        h->setText(std::static_pointer_cast<Paragraph<Trait>>(m_text->clone(doc)));
        h->setLevel(m_level);
        h->setLabel(m_label);
        h->setDelims(m_delims);
        h->setLabelPos(m_labelPos);
        h->setLabelVariants(m_labelVariants);

        if (doc && isLabeled())
            for (const auto &label : std::as_const(m_labelVariants)) {
                doc->insertLabeledHeading(label, h);
            }

        return h;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::Heading;
    }

    /*!
     * \typealias MD::Heading::ParagraphSharedPointer
     *
     * Type of smart pointer to paragraph.
     */
    using ParagraphSharedPointer = std::shared_ptr<Paragraph<Trait>>;

    /*!
     * Returns content of the heading.
     */
    ParagraphSharedPointer text() const
    {
        return m_text;
    }

    /*!
     * Set content of the heading.
     *
     * \a t New value.
     */
    void setText(ParagraphSharedPointer t)
    {
        m_text = t;
    }

    /*!
     * Returns level of the heading.
     */
    int level() const
    {
        return m_level;
    }

    /*!
     * Set level of the heading.
     *
     * \a l New value.
     */
    void setLevel(int l)
    {
        m_level = l;
    }

    /*!
     * Returns whether this heading has label?
     */
    bool isLabeled() const
    {
        return m_label.size() > 0;
    }

    /*!
     * Returns label of the heading.
     */
    const typename Trait::String &label() const
    {
        return m_label;
    }

    /*!
     * Set label of the heading.
     *
     * \a l New value.
     */
    void setLabel(const typename Trait::String &l)
    {
        m_label = l;
    }

    /*!
     * Returns list of service characters.
     */
    const Delims &delims() const
    {
        return m_delims;
    }

    /*!
     * Set list of service characters.
     *
     * \a d New value.
     */
    void setDelims(const Delims &d)
    {
        m_delims = d;
    }

    /*!
     * Returns position of a label in the heading.
     */
    const WithPosition &labelPos() const
    {
        return m_labelPos;
    }

    /*!
     * Set position of a label in the heading.
     *
     * \a p New value.
     */
    void setLabelPos(const WithPosition &p)
    {
        m_labelPos = p;
    }

    /*!
     * \typealias MD::Heading::LabelsVector
     *
     * Type of a vector of labels.
     */
    using LabelsVector = typename Trait::template Vector<typename Trait::String>;

    /*!
     * Returns label variants.
     */
    const LabelsVector &labelVariants() const
    {
        return m_labelVariants;
    }

    /*!
     * Returns label variants.
     */
    LabelsVector &labelVariants()
    {
        return m_labelVariants;
    }

    /*!
     * Set label variants.
     *
     * \a vars New value.
     */
    void setLabelVariants(const LabelsVector &vars)
    {
        m_labelVariants = vars;
    }

private:
    /*!
     * Content of the heading.
     */
    ParagraphSharedPointer m_text;
    /*!
     * Level of the heading.
     */
    int m_level = 0;
    /*!
     * Label of the heading.
     */
    typename Trait::String m_label;
    /*!
     * List of service characters.
     */
    Delims m_delims;
    /*!
     * Position of the label.
     */
    WithPosition m_labelPos;
    /*!
     * Label variants.
     */
    LabelsVector m_labelVariants;

    MD_DISABLE_COPY(Heading)
}; // class Heading

//
// Blockquote
//

/*!
 * \class MD::Blockquote
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Blockquote.
 *
 * Blockquote block in Markdown.
 */
template<class Trait>
class Blockquote final : public Block<Trait>
{
public:
    /*!
     * Default constructor.
     */
    Blockquote() = default;
    ~Blockquote() override = default;

    /*!
     * Clone this blockquote.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        auto b = std::make_shared<Blockquote<Trait>>();
        b->applyBlock(*this, doc);
        b->delims() = m_delims;

        return b;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::Blockquote;
    }

    /*!
     * \typealias MD::Blockquote::Delims
     *
     * Type of a list of service characters.
     */
    using Delims = typename Trait::template Vector<WithPosition>;

    /*!
     * Returns list of service characters.
     */
    const Delims &delims() const
    {
        return m_delims;
    }

    /*!
     * Returns list of service characters.
     */
    Delims &delims()
    {
        return m_delims;
    }

private:
    /*!
     * List of service characters.
     */
    Delims m_delims;

    MD_DISABLE_COPY(Blockquote)
}; // class Blockquote

//
// ListItem
//

/*!
 * \class MD::ListItem
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief List item in a list.
 *
 * List item in a list. List may has only list items, all other content is stored in list items.
 *
 * \sa MD::List
 */
template<class Trait>
class ListItem final : public Block<Trait>
{
public:
    /*!
     * Default constructor.
     */
    ListItem() = default;
    ~ListItem() override = default;

    /*!
     * Clone this list item.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        auto l = std::make_shared<ListItem<Trait>>();
        l->applyBlock(*this, doc);
        l->setListType(m_listType);
        l->setOrderedListPreState(m_orderedListState);
        l->setStartNumber(m_startNumber);
        l->setTaskList(m_isTaskList);
        l->setChecked(m_isChecked);
        l->setDelim(m_delim);
        l->setTaskDelim(m_taskDelim);

        return l;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::ListItem;
    }

    /*!
     * \enum MD::ListItem::ListType
     *
     * Type of the list.
     *
     * \value Ordered Ordered.
     * \value Unordered Unordered.
     */
    enum ListType {
        Ordered,
        Unordered
    }; // enum ListType

    /*!
     * \enum MD::ListItem::OrderedListPreState
     *
     * Preliminary state of the ordered list.
     *
     * \value Start Start item.
     * \value Continue Continue of the list.
     */
    enum OrderedListPreState {
        Start,
        Continue
    }; // enum OrderedListPreState

    /*!
     * Returns type of the list.
     */
    ListType listType() const
    {
        return m_listType;
    }

    /*!
     * Set type of the list.
     *
     * \a t New value.
     */
    void setListType(ListType t)
    {
        m_listType = t;
    }

    /*!
     * Returns preliminary state of the ordered list.
     */
    OrderedListPreState orderedListPreState() const
    {
        return m_orderedListState;
    }

    /*!
     * Set preliminary state of the ordered list.
     *
     * \a s New value.
     */
    void setOrderedListPreState(OrderedListPreState s)
    {
        m_orderedListState = s;
    }

    /*!
     * Returns start number of the ordered list.
     */
    int startNumber() const
    {
        return m_startNumber;
    }

    /*!
     * Set start number of the ordered list.
     *
     * \a n New value.
     */
    void setStartNumber(int n)
    {
        m_startNumber = n;
    }

    /*!
     * Returns whether this list item a task list item?
     */
    bool isTaskList() const
    {
        return m_isTaskList;
    }

    /*!
     * Set this list item to be a tsk list item.
     *
     * \a on New value.
     */
    void setTaskList(bool on = true)
    {
        m_isTaskList = on;
    }

    /*!
     * Returns whether this task list item checked?
     */
    bool isChecked() const
    {
        return m_isChecked;
    }

    /*!
     * Set this task list item to be checked.
     *
     * \a on New value.
     */
    void setChecked(bool on = true)
    {
        m_isChecked = on;
    }

    /*!
     * Returns service character position.
     */
    const WithPosition &delim() const
    {
        return m_delim;
    }

    /*!
     * Set service character position.
     *
     * \a d New value.
     */
    void setDelim(const WithPosition &d)
    {
        m_delim = d;
    }

    /*!
     * Returns position of the task list "checkbox" in Markdown.
     */
    const WithPosition &taskDelim() const
    {
        return m_taskDelim;
    }

    /*!
     * Set position of the task list "checkbox" in Markdown.
     *
     * \a d New value.
     */
    void setTaskDelim(const WithPosition &d)
    {
        m_taskDelim = d;
    }

private:
    /*!
     * Type of the list.
     */
    ListType m_listType = Unordered;
    /*!
     * Preliminary state of the ordered list.
     */
    OrderedListPreState m_orderedListState = Start;
    /*!
     * Start number of the ordered list.
     */
    int m_startNumber = 1;
    /*!
     * Is this list item a task list item?
     */
    bool m_isTaskList = false;
    /*!
     * Is this task list item checked?
     */
    bool m_isChecked = false;
    /*!
     * Service character position.
     */
    WithPosition m_delim = {};
    /*!
     * Task list "checkbox" position.
     */
    WithPosition m_taskDelim = {};

    MD_DISABLE_COPY(ListItem)
}; // class ListItem

//
// List
//

/*!
 * \class MD::List
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief List.
 *
 * List in Markdown. This class is derived from MD::Block, but actually will has only
 * MD::ListItem as children.
 */
template<class Trait>
class List final : public Block<Trait>
{
public:
    /*!
     * Default constructor.
     */
    List() = default;
    ~List() override = default;

    /*!
     * Clone this list.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        auto l = std::make_shared<List<Trait>>();
        l->applyBlock(*this, doc);

        return l;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::List;
    }

private:
    MD_DISABLE_COPY(List)
}; // class List

//
// LinkBase
//

/*!
 * \class MD::LinkBase
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Base class for links.
 *
 * Base class for any links in Markdown, even images.
 *
 * \sa MD::Link, MD::Image.
 */
template<class Trait>
class LinkBase : public ItemWithOpts<Trait>
{
public:
    /*!
     * Default constructor.
     */
    LinkBase()
        : m_p(new Paragraph<Trait>)
    {
    }

    ~LinkBase() override = default;

    /*!
     * Apply other base of link to this.
     *
     * \a other Value to apply.
     *
     * \a doc Parent of new item.
     */
    void applyLinkBase(const LinkBase<Trait> &other, Document<Trait> *doc = nullptr)
    {
        if (this != &other) {
            ItemWithOpts<Trait>::applyItemWithOpts(other);
            setUrl(other.url());
            setText(other.text());
            setP(std::static_pointer_cast<Paragraph<Trait>>(other.p()->clone(doc)));
            setTextPos(other.textPos());
            setUrlPos(other.urlPos());
        }
    }

    /*!
     * \typealias MD::LinkBase::ParagraphSharedPointer
     *
     * Type of a smart pointer to link's description.
     */
    using ParagraphSharedPointer = std::shared_ptr<Paragraph<Trait>>;

    /*!
     * Returns URL of the link.
     */
    const typename Trait::String &url() const
    {
        return m_url;
    }

    /*!
     * Set URL of the link.
     *
     * \a u New value.
     */
    void setUrl(const typename Trait::String &u)
    {
        m_url = u;
    }

    /*!
     * Returns not parsed text of link's description.
     */
    const typename Trait::String &text() const
    {
        return m_text;
    }

    /*!
     * Set not parsed text of link's description.
     *
     * \a t New value.
     */
    void setText(const typename Trait::String &t)
    {
        m_text = t;
    }

    /*!
     * Returns whether this link empty?
     */
    bool isEmpty() const
    {
        return m_url.size() <= 0;
    }

    /*!
     * Returns pointer to parsed text of link's description.
     */
    ParagraphSharedPointer p() const
    {
        return m_p;
    }

    /*!
     * Set pointer to parsed text of link's description.
     *
     * \a v New value.
     */
    void setP(ParagraphSharedPointer v)
    {
        m_p = v;
    }

    /*!
     * Returns position of link's desciption.
     */
    const WithPosition &textPos() const
    {
        return m_textPos;
    }

    /*!
     * Set position of link's description.
     *
     * \a pos New value.
     */
    void setTextPos(const WithPosition &pos)
    {
        m_textPos = pos;
    }

    /*!
     * Returns position of URL.
     */
    const WithPosition &urlPos() const
    {
        return m_urlPos;
    }

    /*!
     * Set position of URL.
     *
     * \a pos New value.
     */
    void setUrlPos(const WithPosition &pos)
    {
        m_urlPos = pos;
    }

private:
    /*!
     * URL.
     */
    typename Trait::String m_url;
    /*!
     * Not parsed content of link's description.
     */
    typename Trait::String m_text;
    /*!
     * Parsed content of link's description.
     */
    ParagraphSharedPointer m_p;
    /*!
     * Position of link's description.
     */
    WithPosition m_textPos = {};
    /*!
     * URL position.
     */
    WithPosition m_urlPos = {};

    MD_DISABLE_COPY(LinkBase)
}; // class LinkBase

//
// Image
//

/*!
 * \class MD::Image
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Image.
 *
 * Image in Markdown.
 */
template<class Trait>
class Image final : public LinkBase<Trait>
{
public:
    /*!
     * Default constructor.
     */
    Image() = default;
    ~Image() override = default;

    /*!
     * Clone this image.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        auto i = std::make_shared<Image<Trait>>();
        i->applyLinkBase(*this, doc);

        return i;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::Image;
    }

private:
    MD_DISABLE_COPY(Image)
}; // class Image

//
// Link
//

/*!
 * \class MD::Link
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Link.
 *
 * Link in Markdown. Doesn't matter what kind of link it is - autolink, shortcut link,
 * GitHub flavored Markdown autolinks, all links will be stored in the document with this item.
 */
template<class Trait>
class Link final : public LinkBase<Trait>
{
public:
    /*!
     * Default constructor.
     */
    Link()
        : LinkBase<Trait>()
        , m_img(new Image<Trait>)
    {
    }

    /*!
     * Clone this link.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        auto l = std::make_shared<Link<Trait>>();
        l->applyLinkBase(*this, doc);
        l->setImg(std::static_pointer_cast<Image<Trait>>(m_img->clone(doc)));

        return l;
    }

    ~Link() override = default;

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::Link;
    }

    /*!
     * \typealias MD::Link::ImageSharedPointer
     *
     * Type of a smart pointer to image.
     */
    using ImageSharedPointer = std::shared_ptr<Image<Trait>>;

    /*!
     * Returns image of the link.
     */
    ImageSharedPointer img() const
    {
        return m_img;
    }

    /*!
     * Set image of the link.
     *
     * \a i New value.
     */
    void setImg(ImageSharedPointer i)
    {
        m_img = i;
    }

private:
    /*!
     * Image of the link.
     */
    ImageSharedPointer m_img;

    MD_DISABLE_COPY(Link)
}; // class Link

//
// Code
//

/*!
 * \class MD::Code
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Code.
 *
 * Block of code in Markdown. Used not only for fenced code blocks, indented code blocks, but
 * for code spans too.
 */
template<class Trait>
class Code : public ItemWithOpts<Trait>
{
public:
    /*!
     * Initializing constructor.
     *
     * \a t Code's text.
     *
     * \a fensedCode Whether this code is a fensed code block.
     *
     * \a inl Whether this code is an inline code.
     */
    explicit Code(const typename Trait::String &t, bool fensedCode, bool inl)
        : ItemWithOpts<Trait>()
        , m_text(t)
        , m_inlined(inl)
        , m_fensed(fensedCode)
    {
    }

    ~Code() override = default;

    /*!
     * Apply other code to this.
     *
     * \a other Value to apply.
     */
    void applyCode(const Code<Trait> &other)
    {
        if (this != &other) {
            ItemWithOpts<Trait>::applyItemWithOpts(other);
            setText(other.text());
            setInline(other.isInline());
            setSyntax(other.syntax());
            setSyntaxPos(other.syntaxPos());
            setStartDelim(other.startDelim());
            setEndDelim(other.endDelim());
            setFensedCode(other.isFensedCode());
        }
    }

    /*!
     * Clone this code.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        MD_UNUSED(doc)

        auto c = std::make_shared<Code<Trait>>(m_text, m_fensed, m_inlined);
        c->applyCode(*this);

        return c;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::Code;
    }

    /*!
     * Returns content of the code.
     */
    const typename Trait::String &text() const
    {
        return m_text;
    }

    /*!
     * Set content of the code.
     *
     * \a t New value.
     */
    void setText(const typename Trait::String &t)
    {
        m_text = t;
    }

    /*!
     * Returns whether this code inline?
     */
    bool isInline() const
    {
        return m_inlined;
    }

    /*!
     * Set this code to be inline.
     *
     * \a on New value.
     */
    void setInline(bool on = true)
    {
        m_inlined = on;
    }

    /*!
     * Returns syntax of the fensed code block.
     */
    const typename Trait::String &syntax() const
    {
        return m_syntax;
    }

    /*!
     * Set syntax of the fensed code block.
     *
     * \a s New value.
     */
    void setSyntax(const typename Trait::String &s)
    {
        m_syntax = s;
    }

    /*!
     * Returns position of the syntax of the fensed code block.
     */
    const WithPosition &syntaxPos() const
    {
        return m_syntaxPos;
    }

    /*!
     * Set position of the syntax of the fensed code block.
     *
     * \a p New value.
     */
    void setSyntaxPos(const WithPosition &p)
    {
        m_syntaxPos = p;
    }

    /*!
     * Returns position of the start service characters.
     */
    const WithPosition &startDelim() const
    {
        return m_startDelim;
    }

    /*!
     * Set position of the start service characters.
     *
     * \a d New value.
     */
    void setStartDelim(const WithPosition &d)
    {
        m_startDelim = d;
    }

    /*!
     * Returns position of the ending service characters.
     */
    const WithPosition &endDelim() const
    {
        return m_endDelim;
    }

    /*!
     * Set position of the ending service characters.
     *
     * \a d New value.
     */
    void setEndDelim(const WithPosition &d)
    {
        m_endDelim = d;
    }

    /*!
     * Returns whether this a fensed code block?
     */
    bool isFensedCode() const
    {
        return m_fensed;
    }

    /*!
     * Set this code block to be a fensed code block.
     *
     * \a on New value.
     */
    void setFensedCode(bool on = true)
    {
        m_fensed = on;
    }

private:
    /*!
     * Content of the code.
     */
    typename Trait::String m_text;
    /*!
     * Is this code inline?
     */
    bool m_inlined = true;
    /*!
     * Is this code a fensed code block.
     */
    bool m_fensed = false;
    /*!
     * Syntax of the fensed code lock.
     */
    typename Trait::String m_syntax;
    /*!
     * Position of start service characters.
     */
    WithPosition m_startDelim = {};
    /*!
     * Position of ending service characters.
     */
    WithPosition m_endDelim = {};
    /*!
     * Position of syntax of fensed code block.
     */
    WithPosition m_syntaxPos = {};

    MD_DISABLE_COPY(Code)
}; // class Code

//
// Math
//

/*!
 * \class MD::Math
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief LaTeX math expression.
 *
 * LaTeX math expression in Markdown.
 */
template<class Trait>
class Math final : public Code<Trait>
{
public:
    /*!
     * Default constructor.
     */
    Math()
        : Code<Trait>(typename Trait::String(), false, true)
    {
    }

    ~Math() override = default;

    /*!
     * Clone this LaTeX math expression.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        MD_UNUSED(doc)

        auto m = std::make_shared<Math<Trait>>();
        m->applyCode(*this);

        return m;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::Math;
    }

    /*!
     * Returns content.
     */
    const typename Trait::String &expr() const
    {
        return Code<Trait>::text();
    }

    /*!
     * Set content.
     *
     * \a e New value.
     */
    void setExpr(const typename Trait::String &e)
    {
        Code<Trait>::setText(e);
    }

private:
    MD_DISABLE_COPY(Math)
}; // class Math

//
// TableCell
//

/*!
 * \class MD::TableCell
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Table cell.
 *
 * Block of items in table cell in GitHub flavored Markdown tables.
 *
 * \sa MD::TableRow, MD::Table.
 */
template<class Trait>
class TableCell final : public Block<Trait>
{
public:
    /*!
     * Default constructor.
     */
    TableCell() = default;
    ~TableCell() override = default;

    /*!
     * Clone this table cell.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        auto c = std::make_shared<TableCell<Trait>>();
        c->applyBlock(*this, doc);

        return c;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::TableCell;
    }

private:
    MD_DISABLE_COPY(TableCell)
}; // class TableCell

//
// TableRow
//

/*!
 * \class MD::TableRow
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Table row.
 *
 * Row of table cells in GitHub flavored Markdown tables.
 *
 * \sa MD::TableCell, MD::Table.
 */
template<class Trait>
class TableRow final : public Item<Trait>
{
public:
    /*!
     * Default constructor.
     */
    TableRow() = default;
    ~TableRow() override = default;

    /*!
     * Clone this table row.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        auto t = std::make_shared<TableRow<Trait>>();
        t->applyPositions(*this);

        for (const auto &c : cells()) {
            t->appendCell(std::static_pointer_cast<TableCell<Trait>>(c->clone(doc)));
        }

        return t;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::TableRow;
    }

    /*!
     * \typealias MD::TableRow::TableCellSharedPointer
     *
     * Type of a smart pointer to table cell.
     */
    using TableCellSharedPointer = std::shared_ptr<TableCell<Trait>>;
    /*!
     * \typealias MD::TableRow::Cells
     *
     * Type of a list of table cells.
     */
    using Cells = typename Trait::template Vector<TableCellSharedPointer>;

    /*!
     * Returns list of cells.
     */
    const Cells &cells() const
    {
        return m_cells;
    }

    /*!
     * Append cell.
     *
     * \a c New value.
     */
    void appendCell(TableCellSharedPointer c)
    {
        m_cells.push_back(c);
    }

    /*!
     * Returns whether this row empty?
     */
    bool isEmpty() const
    {
        return m_cells.empty();
    }

private:
    /*!
     * List of cells.
     */
    Cells m_cells;

    MD_DISABLE_COPY(TableRow)
}; // class TableRow

//
// Table
//

/*!
 * \class MD::Table
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Table.
 *
 * GitHub flavored Markdown table.
 *
 * \sa MD::TableCell, MD::TableRow.
 */
template<class Trait>
class Table final : public Item<Trait>
{
public:
    /*!
     * Default constructor.
     */
    Table() = default;
    ~Table() override = default;

    /*!
     * Clone this table.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        auto t = std::make_shared<Table<Trait>>();
        t->applyPositions(*this);

        for (const auto &r : rows()) {
            t->appendRow(std::static_pointer_cast<TableRow<Trait>>(r->clone(doc)));
        }

        for (int i = 0; i < columnsCount(); ++i) {
            t->setColumnAlignment(i, columnAlignment(i));
        }

        return t;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::Table;
    }

    /*!
     * \typealias MD::Table::TableRowSharedPointer
     *
     * Type of a smart pointer to table row.
     */
    using TableRowSharedPointer = std::shared_ptr<TableRow<Trait>>;
    /*!
     * \typealias MD::Table::Rows
     *
     * Type of list of rows.
     */
    using Rows = typename Trait::template Vector<TableRowSharedPointer>;

    /*!
     * Returns list of rows.
     */
    const Rows &rows() const
    {
        return m_rows;
    }

    /*!
     * Append row.
     *
     * \a r New value.
     */
    void appendRow(TableRowSharedPointer r)
    {
        m_rows.push_back(r);
    }

    /*!
     * \enum MD::Table::Alignment
     * \inmodule md4qt
     *
     * Alignment.
     *
     * \value AlignLeft Left.
     * \value AlignRight Right.
     * \value AlignCenter Center.
     */
    enum Alignment {
        AlignLeft,
        AlignRight,
        AlignCenter
    }; // enum Alignmnet.

    /*!
     * \typealias MD::Table::ColumnsAlignments
     *
     * Type of list alignments.
     */
    using ColumnsAlignments = typename Trait::template Vector<Alignment>;

    /*!
     * Returns alignment of the given column.
     *
     * \a idx Index.
     */
    Alignment columnAlignment(int idx) const
    {
        return m_aligns.at(idx);
    }

    /*!
     * Set alignment of the given column.
     *
     * \a idx Index.
     *
     * \a a New value.
     */
    void setColumnAlignment(int idx, Alignment a)
    {
        if (idx + 1 > columnsCount()) {
            m_aligns.push_back(a);
        } else {
            m_aligns[idx] = a;
        }
    }

    /*!
     * Returns count of columns.
     */
    int columnsCount() const
    {
        return m_aligns.size();
    }

    /*!
     * Returns whether this table empty?
     */
    bool isEmpty() const
    {
        return (m_aligns.empty() || m_rows.empty());
    }

private:
    /*!
     * Rows.
     */
    Rows m_rows;
    /*!
     * Columns' alignments.
     */
    ColumnsAlignments m_aligns;

    MD_DISABLE_COPY(Table)
}; // class Table

//
// FootnoteRef
//

/*!
 * \class MD::FootnoteRef
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Footnote reference.
 *
 * Inline footnote reference in MD::Paragraph. Ususally renders as
 * superscript number.
 */
template<class Trait>
class FootnoteRef final : public Text<Trait>
{
public:
    /*!
     * Initializing constructor.
     *
     * \a i ID.
     */
    explicit FootnoteRef(const typename Trait::String &i)
        : m_id(i)
    {
    }

    ~FootnoteRef() override = default;

    /*!
     * Clone this footnote reference.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        MD_UNUSED(doc)

        auto f = std::make_shared<FootnoteRef<Trait>>(m_id);
        f->applyText(*this);
        f->setIdPos(m_idPos);

        return f;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::FootnoteRef;
    }

    /*!
     * Returns ID of footnote reference.
     */
    const typename Trait::String &id() const
    {
        return m_id;
    }

    /*!
     * Returns position of ID.
     */
    const WithPosition &idPos() const
    {
        return m_idPos;
    }

    /*!
     * Set position of ID.
     *
     * \a pos Position.
     */
    void setIdPos(const WithPosition &pos)
    {
        m_idPos = pos;
    }

private:
    /*!
     * ID.
     */
    typename Trait::String m_id;
    /*!
     * Position of ID.
     */
    WithPosition m_idPos;

    MD_DISABLE_COPY(FootnoteRef)
}; // class FootnoteRef

//
// Footnote
//

/*!
 * \class MD::Footnote
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Footnote.
 *
 * Footnote block in Markdown.
 */
template<class Trait>
class Footnote final : public Block<Trait>
{
public:
    /*!
     * Default constructor.
     */
    Footnote() = default;
    ~Footnote() override = default;

    /*!
     * Clone this footnote.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        auto f = std::make_shared<Footnote<Trait>>();
        f->applyBlock(*this, doc);
        f->setIdPos(m_idPos);

        return f;
    }

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::Footnote;
    }

    /*!
     * Returns position of ID.
     */
    const WithPosition &idPos() const
    {
        return m_idPos;
    }

    /*!
     * Set position of ID.
     *
     * \a pos Position.
     */
    void setIdPos(const WithPosition &pos)
    {
        m_idPos = pos;
    }

private:
    /*!
     * Position of ID.
     */
    WithPosition m_idPos = {};

    MD_DISABLE_COPY(Footnote)
}; // class Footnote

//
// Document
//

/*!
 * \class MD::Document
 * \inmodule md4qt
 * \inheaderfile md4qt/doc.h
 *
 * \brief Document.
 *
 * Markdown document.
 */
template<class Trait>
class Document final : public Block<Trait>
{
public:
    /*!
     * Default constructor.
     */
    Document() = default;
    ~Document() override = default;

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return ItemType::Document;
    }

    /*!
     * Clone this document.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        MD_UNUSED(doc)

        auto d = std::make_shared<Document<Trait>>();
        d->applyBlock(*this, d.get());

        for (auto it = m_footnotes.cbegin(), last = m_footnotes.cend(); it != last; ++it) {
            d->insertFootnote(it->first, std::static_pointer_cast<Footnote<Trait>>(it->second->clone(d.get())));
        }

        for (auto it = m_labeledLinks.cbegin(), last = m_labeledLinks.cend(); it != last; ++it) {
            d->insertLabeledLink(it->first, std::static_pointer_cast<Link<Trait>>(it->second->clone(d.get())));
        }

        return d;
    }

    /*!
     * \typealias MD::Document::FootnoteSharedPointer
     *
     * Type of a smart pointer to footnote.
     */
    using FootnoteSharedPointer = std::shared_ptr<Footnote<Trait>>;
    /*!
     * \typealias MD::Document::Footnotes
     *
     * Type of a map of footnotes.
     */
    using Footnotes = typename Trait::template Map<typename Trait::String, FootnoteSharedPointer>;

    /*!
     * Returns map of footnotes.
     */
    const Footnotes &footnotesMap() const
    {
        return m_footnotes;
    }

    /*!
     * Insert footnote with the give ID.
     *
     * \a id ID.
     *
     * \a fn Footnote.
     */
    void insertFootnote(const typename Trait::String &id, FootnoteSharedPointer fn)
    {
        m_footnotes.insert({id, fn});
    }

    /*!
     * \typealias MD::Document::LinkSharedPointer
     *
     * Type of a smart pointer to link.
     */
    using LinkSharedPointer = std::shared_ptr<Link<Trait>>;
    /*!
     * \typealias MD::Document::LabeledLinks
     *
     * Type of a map of shortcut links.
     */
    using LabeledLinks = typename Trait::template Map<typename Trait::String, LinkSharedPointer>;

    /*!
     * Returns map of shortcut links.
     */
    const LabeledLinks &labeledLinks() const
    {
        return m_labeledLinks;
    }

    /*!
     * Insert shortcut link with the given label.
     *
     * \a label Label.
     *
     * \a lnk Link.
     */
    void insertLabeledLink(const typename Trait::String &label, LinkSharedPointer lnk)
    {
        m_labeledLinks.insert({label, lnk});
    }

    /*!
     * \typealias MD::Document::HeadingSharedPointer
     *
     * Type of a smart pointer to heading.
     */
    using HeadingSharedPointer = std::shared_ptr<Heading<Trait>>;
    /*!
     * \typealias MD::Document::LabeledHeadings
     *
     * Type of a map of headings.
     */
    using LabeledHeadings = typename Trait::template Map<typename Trait::String, HeadingSharedPointer>;

    /*!
     * Returns map of headings.
     */
    const LabeledHeadings &labeledHeadings() const
    {
        return m_labeledHeadings;
    }

    /*!
     * Insert heading with the given label.
     *
     * \a label Label.
     *
     * \a h Heading.
     */
    void insertLabeledHeading(const typename Trait::String &label, HeadingSharedPointer h)
    {
        m_labeledHeadings.insert({label, h});
    }

private:
    /*!
     * Map of footnotes.
     */
    Footnotes m_footnotes;
    /*!
     * Map of shortcut links.
     */
    LabeledLinks m_labeledLinks;
    /*!
     * Map of headings.
     */
    LabeledHeadings m_labeledHeadings;

    MD_DISABLE_COPY(Document)
}; // class Document;

} /* namespace MD */

#endif // MD4QT_MD_DOC_H_INCLUDED
