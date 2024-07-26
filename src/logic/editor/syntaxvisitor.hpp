// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

// KleverNotes include
#include "logic/editor/colors.hpp"
#include "logic/parser/plugins/emoji/emojiExtension.hpp"

// md4qt include.
#define MD4QT_QT_SUPPORT
#include "logic/parser/md4qt/poscache.hpp"
#include "logic/parser/md4qt/traits.hpp"

// Qt include.
#include <QColor>
#include <QScopedPointer>

// C++ include.
#include <vector>

class EditorHandler;
namespace MdEditor
{
//
// SyntaxVisitor
//

struct SyntaxVisitorPrivate;

//! Markdown syntax highlighter.
class SyntaxVisitor : public MD::PosCache<MD::QStringTrait>
{
public:
    explicit SyntaxVisitor(EditorHandler *editor);
    ~SyntaxVisitor() override;

    void highlight(std::shared_ptr<MD::Document<MD::QStringTrait>> doc, const Colors &colors);
    void setFont(const QFont &f);
    void clearHighlighting();

    void addExtendedSyntax(const long long int opts, const QStringList &info);

protected:
    void onReferenceLink(MD::Link<MD::QStringTrait> *l) override;
    void onText(MD::Text<MD::QStringTrait> *t) override;
    void onMath(MD::Math<MD::QStringTrait> *m) override;
    void onHeading(MD::Heading<MD::QStringTrait> *h) override;
    void onCode(MD::Code<MD::QStringTrait> *c) override;
    void onInlineCode(MD::Code<MD::QStringTrait> *c) override;
    void onBlockquote(MD::Blockquote<MD::QStringTrait> *b) override;
    void onTable(MD::Table<MD::QStringTrait> *t) override;
    void onRawHtml(MD::RawHtml<MD::QStringTrait> *h) override;
    void onHorizontalLine(MD::HorizontalLine<MD::QStringTrait> *l) override;
    void onLink(MD::Link<MD::QStringTrait> *l) override;
    void onImage(MD::Image<MD::QStringTrait> *i) override;
    void onFootnoteRef(MD::FootnoteRef<MD::QStringTrait> *ref) override;
    void onFootnote(MD::Footnote<MD::QStringTrait> *f) override;
    void onListItem(MD::ListItem<MD::QStringTrait> *l, bool first) override;
    void onUserDefined(MD::Item<MD::QStringTrait> *item) override;

private:
    void onItemWithOpts(MD::ItemWithOpts<MD::QStringTrait> *i);

    void onEmoji(EmojiExtension::EmojiItem *e);

private:
    Q_DISABLE_COPY(SyntaxVisitor)

    QScopedPointer<SyntaxVisitorPrivate> d;
}; // class SyntaxVisitor

} /* namespace MdEditor */
