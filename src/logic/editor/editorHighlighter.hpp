// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

// KleverNotes include
#include "editorHighlighterPrivate.hpp"
#include "logic/editor/colors.hpp"
#include "logic/editor/posCacheUtils.hpp"
#include "logic/parser/plugins/emoji/emojiPlugin.hpp"

// md4qt include.
#define MD4QT_QT_SUPPORT
#include "logic/parser/md4qt/poscache.h"
#include "logic/parser/md4qt/traits.h"

// Qt include.
#include <QColor>
#include <QScopedPointer>

namespace MdEditor
{
//! Markdown syntax highlighter.
class EditorHighlighter : public MD::PosCache<MD::QStringTrait>
{
public:
    explicit EditorHighlighter(EditorHandler *editor);
    ~EditorHighlighter() override;

    void highlight(std::shared_ptr<MD::Document<MD::QStringTrait>> doc, const Colors &colors);
    QList<posCacheUtils::DelimsInfo> showDelimAroundCursor(const bool clearCache = true);
    void setFont(const QFont &f);
    void clearHighlighting();

    void addExtendedSyntax(const long long int opts, const QStringList &info);

    void changeAdaptiveTagSize(const bool adaptive);
    void changeTagScale(const int tagSizeScale);

    MD::ListItem<MD::QStringTrait> *searchListItem(const int line, const int pos);

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

    void onEmoji(EmojiPlugin::EmojiItem *e);

    void revertHeadingDelims(MD::Item<MD::QStringTrait> *item);

    void addBlockDelims(QList<posCacheUtils::DelimsInfo> &delims,
                        const QTextBlock &block,
                        const MD::WithPosition &pos,
                        const MD::WithPosition &selectStartPos = {},
                        const MD::WithPosition &selectEndPos = {});

    QList<posCacheUtils::DelimsInfo> getDelimsFromCursor();

    QList<posCacheUtils::DelimsInfo> revertDelimsStyle();

private:
    Q_DISABLE_COPY(EditorHighlighter)

    QScopedPointer<EditorHighlighterPrivate> d;
}; // !EditorHighlighter
} // !namespace MdEditor
