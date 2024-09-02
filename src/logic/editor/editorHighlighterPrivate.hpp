// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// KleverNotes includes
#include "logic/editor/editorHandler.hpp"

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

    void clearFormats();

    void applyFormats();

    void setFormat(const QTextCharFormat &format, const MD::WithPosition &pos);

    void setFormat(const QTextCharFormat &format, long long int startLine, long long int startColumn, long long int endLine, long long int endColumn);

    void applyFormatChanges();

    QFont styleFont(int opts, bool isSpecial = false) const;

    // KleverNotes
    QColor getColor(const QString &info);

    QTextCharFormat makeFormat(const long long int opts);

    void restoreCachedFormats();

    void revertFormat(const MD::WithPosition &withPosition);

    void revertFormats(const QList<std::pair<MD::WithPosition, MD::WithPosition>> delimsPairs);

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
