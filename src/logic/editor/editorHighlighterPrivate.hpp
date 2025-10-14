// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// KleverNotes includes
#include "logic/editor/editorHandler.hpp"
#include "logic/editor/posCacheUtils.hpp"

// Qt include.
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextEdit>

using namespace Qt::Literals::StringLiterals;

namespace MdEditor
{
class EditorHighlighterPrivate
{
public:
    EditorHighlighterPrivate(EditorHandler *e);

    /**
     * @brief Clear all the formating of the editor.
     */
    void clearFormats();

    /**
     * @brief Apply formating to the editor.
     */
    void applyFormats();

    /**
     * @brief Apply the given format to the given position.
     *
     * @param format The format to apply.
     * @param pos The position on which to apply the format.
     */
    void setFormat(const QTextCharFormat &format, const MD::WithPosition &pos);

    /**
     * @brief Apply the given format to the given position.
     *
     * @param format The format to apply.
     * @param startLine The starting line on which to apply the format.
     * @param startColumn The starting column on which to apply the format.
     * @param endLine The ending line on which to apply the format.
     * @param endColumn The ending column on which to apply the format.
     */
    void setFormat(const QTextCharFormat &format, long long int startLine, long long int startColumn, long long int endLine, long long int endColumn);

    /**
     * @brief Apply formats that have changed.
     */
    void applyFormatChanges();

    /**
     * @brief Style the font based on the given information.
     *
     * @param opts The style options which will affect the font.
     * @param isSpecial Whether the font is special, for example, if it is used for the delims.
     * @return A font styled based on the given info.
     */
    QFont styleFont(int opts, bool isSpecial = false) const;

    // KleverNotes
    /**
     * @brief Get a color based on its info.
     *
     * @param info The info of the color.
     * @return A color corresponding to the given info. An invalid color of the given info was not valid.
     */
    QColor getColor(const QString &info);

    /**
     * @brief Create a format base on the given options.
     *
     * @param opts The style options which will affect the format.
     * @return A format corresponding to the given options.
     */
    QTextCharFormat makeFormat(const long long int opts);

    /**
     * @brief Restore the cached formats.
     */
    void restoreCachedFormats();

    /**
     * @brief Remove and cache the formats currently applied to the given position.
     *
     * @param withPosition The position on which we want to remove and cache the formats.
     */
    void revertFormat(const MD::WithPosition &withPosition);

    /**
     * @brief Remove and cache the formats currently applied at the positions defined in the delimInfo.
     *
     * @param delimInfo The delimInfo on which we want to remove and cache the formats.
     */
    void revertFormats(const posCacheUtils::DelimsInfo &delimInfo);

    /**
     * @brief Simple workaround to prevent the TextArea from auto scrolling to put the cursor line at the bottom.
     */
    void preventAutoScroll();

    // Editor.
    EditorHandler *editor = nullptr;
    // Document.
    std::shared_ptr<MD::Document<MD::QStringTrait>> doc;
    // Colors.
    Colors colors;
    // Default font.
    QFont font;
    // Additional style that should be applied for any item.
    int additionalStyle = 0;
    // Current text block.
    QTextBlock currentBlock;
    // Formats for current block.
    QList<QTextCharFormat> formatChanges;

    struct Format {
        QTextBlock block;
        QList<QTextCharFormat> formats;
    };

    // Formats.
    QMap<int, Format> formats;
    QMap<int, Format> cachedFormats;
    int headingLevel = 0;

    // KleverNotes
    QMap<int, QStringList> modifications;
    inline static const QStringList colorsName = {u"background"_s, u"text"_s, u"link"_s, u"special"_s, u"title"_s, u"highlight"_s, u"code"_s};

    int tagSizeScale = 100;
    bool adaptiveTagSize = false;
}; // !EditorHighlighterPrivate
} // !namespace MdEditor
