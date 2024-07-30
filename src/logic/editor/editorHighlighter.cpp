// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "editorHighlighter.hpp"

// KleverNotes includes
#include "logic/editor/editorHandler.hpp"

// md-editor include.
#include "logic/parser/md4qt/doc.hpp"

// Qt include.
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextEdit>

// C++ include.
#include <algorithm>

using namespace Qt::Literals::StringLiterals;

namespace MdEditor
{
// EditorHighlighterPrivate
struct EditorHighlighterPrivate {
    EditorHighlighterPrivate(EditorHandler *e)
        : editor(e)
    {
    }

    void clearFormats()
    {
        auto b = editor->document()->firstBlock();

        while (b.isValid()) {
            b.layout()->clearFormats();

            b = b.next();
        }

        formats.clear();
    }

    void applyFormats()
    {
        for (const auto &f : std::as_const(formats)) {
            currentBlock = f.block;
            formatChanges = f.formats;

            applyFormatChanges();
        }
    }

    void setFormat(const QTextCharFormat &format, const MD::WithPosition &pos)
    {
        setFormat(format, pos.startLine(), pos.startColumn(), pos.endLine(), pos.endColumn());
    }

    void setFormat(const QTextCharFormat &format, long long int startLine, long long int startColumn, long long int endLine, long long int endColumn)
    {
        if (colors.enabled) {
            for (auto i = startLine; i <= endLine; ++i) {
                formats[i].block = editor->document()->findBlockByNumber(i);

                if (formats[i].formats.isEmpty())
                    formats[i].formats.fill(QTextCharFormat(), formats[i].block.length() - 1);

                int start = (i == startLine ? startColumn : 0);
                int length = (i == startLine ? (i == endLine ? endColumn - startColumn + 1 : formats[i].block.length() - 1 - startColumn)
                                             : (i == endLine ? endColumn + 1 : formats[i].block.length() - 1));

                for (int j = start; j < start + length; ++j)
                    formats[i].formats[j] = format;
            }
        }
    }

    void applyFormatChanges()
    {
        bool formatsChanged = false;

        QTextLayout *layout = currentBlock.layout();

        QList<QTextLayout::FormatRange> ranges = layout->formats();

        const int preeditAreaStart = layout->preeditAreaPosition();
        const int preeditAreaLength = layout->preeditAreaText().size();

        if (preeditAreaLength != 0) {
            auto isOutsidePreeditArea = [=](const QTextLayout::FormatRange &range) {
                return range.start < preeditAreaStart || range.start + range.length > preeditAreaStart + preeditAreaLength;
            };
            if (ranges.removeIf(isOutsidePreeditArea) > 0)
                formatsChanged = true;
        } else if (!ranges.isEmpty()) {
            ranges.clear();
            formatsChanged = true;
        }

        int i = 0;
        while (i < formatChanges.size()) {
            QTextLayout::FormatRange r;

            while (i < formatChanges.size() && formatChanges.at(i) == r.format)
                ++i;

            if (i == formatChanges.size())
                break;

            r.start = i;
            r.format = formatChanges.at(i);

            while (i < formatChanges.size() && formatChanges.at(i) == r.format)
                ++i;

            Q_ASSERT(i <= formatChanges.size());
            r.length = i - r.start;

            if (preeditAreaLength != 0) {
                if (r.start >= preeditAreaStart)
                    r.start += preeditAreaLength;
                else if (r.start + r.length >= preeditAreaStart)
                    r.length += preeditAreaLength;
            }

            ranges << r;
            formatsChanged = true;
        }

        if (formatsChanged) {
            layout->setFormats(ranges);
            editor->document()->markContentsDirty(currentBlock.position(), currentBlock.length());
        }
    }

    QFont styleFont(int opts, bool adjustSize = true) const
    {
        auto f = font;
        if (adjustSize) {
            // Base on KleverStyle.css
            auto size = f.pointSize();
            switch (headingLevel) {
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

            f.setPointSize(size);
        }

        if (opts & MD::ItalicText)
            f.setItalic(true);

        if (opts & MD::BoldText)
            f.setBold(true);

        if (opts & MD::StrikethroughText)
            f.setStrikeOut(true);

        return f;
    }

    // KleverNotes
    QColor getColor(const QString &info)
    {
        const int specialColorIdx = colorsName.indexOf(info);
        if (specialColorIdx != -1) {
            switch (specialColorIdx) {
            case 0:
                return colors.textColor;
            case 1:
                return colors.linkColor;
            case 2:
                return colors.specialColor;
            case 3:
                return colors.titleColor;
            case 4:
                return colors.highlightColor;
            case 5:
                return colors.codeColor;
            }
        } else if (QColor::isValidColorName(info)) {
            return QColor(info);
        }
        return QColor().convertTo(QColor::Spec::Invalid);
    }

    QTextCharFormat makeFormat(const long long int opts)
    {
        QTextCharFormat format;
        format.setForeground(headingLevel ? colors.titleColor : colors.textColor);
        format.setFont(styleFont(opts));
        for (auto i = modifications.cbegin(), end = modifications.cend(); i != end; ++i) {
            const long long int modifOpts = i.key();

            if (opts & modifOpts) {
                const QStringList &info = i.value();

                float sizeScale = 1;
                if (!info[3].isEmpty()) {
                    bool ok;
                    float scale = info[3].toFloat(&ok);
                    sizeScale = ok ? scale : 1;
                }
                const int size = format.font().pointSize() * sizeScale;
                format.setFontPointSize(size);

                const QColor foreground = getColor(info[4]);
                if (foreground.isValid()) {
                    format.setForeground(foreground);
                }

                const QColor background = getColor(info[5]);
                if (background.isValid()) {
                    format.setBackground(background);
                }

                if (!info[6].isEmpty()) {
                    bool ok;
                    int alignmentValue = info[6].toInt(&ok);
                    if (ok && 0 < alignmentValue && alignmentValue < 7) {
                        QTextCharFormat::VerticalAlignment alignment;
                        switch (alignmentValue) {
                        case 1:
                            alignment = QTextCharFormat::VerticalAlignment::AlignSuperScript;
                            break;
                        case 2:
                            alignment = QTextCharFormat::VerticalAlignment::AlignSubScript;
                            break;
                        case 3:
                            alignment = QTextCharFormat::VerticalAlignment::AlignMiddle;
                            break;
                        case 4:
                            alignment = QTextCharFormat::VerticalAlignment::AlignBottom;
                            break;
                        case 5:
                            alignment = QTextCharFormat::VerticalAlignment::AlignTop;
                            break;
                        case 6:
                            alignment = QTextCharFormat::VerticalAlignment::AlignBaseline;
                            break;
                        }
                        format.setVerticalAlignment(alignment);
                    }
                }

                if (!info[7].isEmpty()) {
                    format.setFontWeight(700);
                }
                if (!info[8].isEmpty()) {
                    format.setFontItalic(true);
                }
                if (!info[9].isEmpty()) {
                    format.setFontStrikeOut(true);
                }
                if (!info[10].isEmpty()) {
                    format.setFontUnderline(true);
                }
                if (!info[11].isEmpty()) {
                    bool ok;
                    int underlineStyleValue = info[11].toInt(&ok);
                    if (ok && 0 < underlineStyleValue && underlineStyleValue < 8) {
                        // Apply to format
                        QTextCharFormat::UnderlineStyle underlineStyle;
                        switch (underlineStyleValue) {
                        case 1:
                            underlineStyle = QTextCharFormat::UnderlineStyle::SingleUnderline;
                            break;
                        case 2:
                            underlineStyle = QTextCharFormat::UnderlineStyle::DashUnderline;
                            break;
                        case 3:
                            underlineStyle = QTextCharFormat::UnderlineStyle::DotLine;
                            break;
                        case 4:
                            underlineStyle = QTextCharFormat::UnderlineStyle::DashDotLine;
                            break;
                        case 5:
                            underlineStyle = QTextCharFormat::UnderlineStyle::DashDotDotLine;
                            break;
                        case 6:
                            underlineStyle = QTextCharFormat::UnderlineStyle::WaveUnderline;
                            break;
                        case 7:
                            underlineStyle = QTextCharFormat::UnderlineStyle::SpellCheckUnderline;
                            break;
                        }
                        format.setUnderlineStyle(underlineStyle);
                    }
                }
            }
        }
        return format;
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
    //! Current text block.
    QTextBlock currentBlock;
    //! Formats for current block.
    QList<QTextCharFormat> formatChanges;

    struct Format {
        QTextBlock block;
        QList<QTextCharFormat> formats;
    };

    //! Formats.
    QMap<int, Format> formats;
    int headingLevel = 0;
    // KleverNotes
    QMap<int, QStringList> modifications;
    inline static const QStringList colorsName = {u"text"_s, u"link"_s, u"special"_s, u"title"_s, u"highlight"_s, u"code"_s};
}; // !EditorHighlighterPrivate

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
    d->applyFormats();
    c.endEditBlock();
}

void EditorHighlighter::addExtendedSyntax(const long long int opts, const QStringList &info)
{
    d->modifications[opts] = info;
}

void EditorHighlighter::onItemWithOpts(MD::ItemWithOpts<MD::QStringTrait> *i)
{
    QTextCharFormat special;
    special.setForeground(d->colors.specialColor);
    // TODO: add settings for `adjustSize` on/off
    special.setFont(d->styleFont(d->additionalStyle, false));

    for (const auto &s : i->openStyles())
        d->setFormat(special, s);

    for (const auto &s : i->closeStyles())
        d->setFormat(special, s);
}

void EditorHighlighter::onReferenceLink(MD::Link<MD::QStringTrait> *l)
{
    QTextCharFormat format;
    format.setForeground(d->colors.linkColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, l->startLine(), l->startColumn(), l->endLine(), l->endColumn());

    MD::PosCache<MD::QStringTrait>::onReferenceLink(l);
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

void EditorHighlighter::onHeading(MD::Heading<MD::QStringTrait> *h)
{
    {
        QScopedValueRollback headingLevel(d->headingLevel, h->level());
        QScopedValueRollback style(d->additionalStyle, d->additionalStyle | MD::BoldText);

        QTextCharFormat baseFormat;
        baseFormat.setForeground(d->colors.titleColor);
        baseFormat.setFont(d->styleFont(MD::BoldText));
        d->setFormat(baseFormat, h->startLine(), h->text()->startColumn(), h->endLine(), h->endColumn());

        MD::PosCache<MD::QStringTrait>::onHeading(h);
    }

    QTextCharFormat special;
    special.setForeground(d->colors.specialColor);
    special.setFont(d->styleFont(MD::TextWithoutFormat));

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

    d->setFormat(format, c->startLine(), c->startColumn(), c->endLine(), c->endColumn());

    special.setForeground(d->colors.specialColor);
    special.setFont(d->styleFont(d->additionalStyle));

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
    special.setFont(d->styleFont(d->additionalStyle));

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

void EditorHighlighter::onImage(MD::Image<MD::QStringTrait> *i)
{
    QTextCharFormat format;
    format.setForeground(d->colors.linkColor);
    format.setFont(d->styleFont(d->additionalStyle));

    d->setFormat(format, i->startLine(), i->startColumn(), i->endLine(), i->endColumn());

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
    generalFormat.setFont(d->styleFont(e->opts() | d->additionalStyle));

    d->setFormat(generalFormat, e->startLine(), e->startColumn(), e->endLine(), e->endColumn());

    QScopedValueRollback style(d->additionalStyle, d->additionalStyle | e->opts());

    QTextCharFormat emojiFormat;
    emojiFormat.setForeground(QColor(QStringLiteral("magenta")));
    emojiFormat.setFont(d->styleFont(e->opts() | d->additionalStyle));
    d->setFormat(emojiFormat, e->emojiNamePos());

    QTextCharFormat optionsFormat;
    optionsFormat.setForeground(QColor(QStringLiteral("yellow")));
    optionsFormat.setFont(d->styleFont(e->opts() | d->additionalStyle));

    d->setFormat(optionsFormat, e->optionsPos());

    onItemWithOpts(e);
}

void EditorHighlighter::onUserDefined(MD::Item<MD::QStringTrait> *item)
{
    static const int userDefinedType = static_cast<int>(MD::ItemType::UserDefined);
    const int itemType = static_cast<int>(item->type());
    switch (itemType) {
    case userDefinedType + 1: {
        auto emoji = static_cast<EmojiPlugin::EmojiItem *>(item);
        onEmoji(emoji);
        break;
    }
    default:
        qWarning() << "Unsupported custom item";
        return;
    }
}
// !EditorHighlighter
} // !namespace MdEditor
