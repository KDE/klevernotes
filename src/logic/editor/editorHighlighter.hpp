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
/**
 * @class EditorHighlighter
 * @brief Class providing highlight to the QML editor.
 */
class EditorHighlighter : public MD::PosCache<MD::QStringTrait>
{
public:
    explicit EditorHighlighter(EditorHandler *editor);
    ~EditorHighlighter() override;

    /**
     * @brief Cache the given doc and highlight the editor.
     *
     * @param doc The MD::Document resulting of the parsing.
     * @param highlight Whether to highlight the editor.
     */
    void cacheAndHighlight(std::shared_ptr<MD::Document<MD::QStringTrait>> doc, const bool highlight);

    /**
     * @brief Change the style of the delims surrounding the cursor to make them more visible.
     *
     * @param clearCache Whether to clear the cached format.
     */
    QList<posCacheUtils::DelimsInfo> showDelimAroundCursor(const bool clearCache = true);

    /**
     * @brief Set the font used for the highlight.
     *
     * @param f The font to be used for the highlight.
     */
    void setFont(const QFont &f);

    /**
     * @brief Clear the highlighting in the editor.
     */
    void clearHighlighting();

    /**
     * @brief Set the colors to be used for the highlighter.
     *
     * @param colors The colors.
     */
    void setColors(const Colors &colors);

    /**
     * @brief Add an extended syntax to the highlighter.
     *
     * @param opts The options used to identify the given extended syntax.
     * @param info The highlighting information for the extended syntax.
     */
    void addExtendedSyntax(const long long int opts, const QStringList &info);

    /**
     * @brief Changed the way the delims will be highlighted.
     * Adaptive will make then smaller than the rest of the text.
     *
     * @param adaptive Whether the delim style should be adaptive.
     */
    void changeAdaptiveTagSize(const bool adaptive);

    /**
     * @brief Change the scale of the text used when the adaptive tag scale is active.
     *
     * @param tagSizeScale The scale of the text.
     */
    void changeTagScale(const int tagSizeScale);

    /**
     * @brief Search a list item near the given position.
     *
     * @param line The line on which we want to search the item.
     * @param pos The position (column) on which we want to search the item.
     * @result A pointer to the list item if it is found. A nullptr otherwise.
     */
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
    void onListItem(MD::ListItem<MD::QStringTrait> *l, bool first, bool skipOpeningWrap) override;
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

    void revertDelimsStyle(const QList<posCacheUtils::DelimsInfo> &delims);

private:
    Q_DISABLE_COPY(EditorHighlighter)

    bool m_highlightEnabled = false;

    QScopedPointer<EditorHighlighterPrivate> d;
}; // !EditorHighlighter
} // !namespace MdEditor
