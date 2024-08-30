// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "editorHighlighterPrivate.hpp"
#include "logic/parser/md4qt/doc.hpp"
#include <utility>

using namespace Qt::Literals::StringLiterals;

namespace MdEditor
{
EditorHighlighterPrivate::EditorHighlighterPrivate(EditorHandler *e)
    : editor(e)
{
}

void EditorHighlighterPrivate::clearFormats()
{
    auto b = editor->document()->firstBlock();

    while (b.isValid()) {
        b.layout()->clearFormats();

        b = b.next();
    }

    formats.clear();
    cachedFormats.clear();
}

void EditorHighlighterPrivate::applyFormats()
{
    for (const auto &f : std::as_const(formats)) {
        currentBlock = f.block;
        formatChanges = f.formats;

        applyFormatChanges();
    }
}

void EditorHighlighterPrivate::setFormat(const QTextCharFormat &format, const MD::WithPosition &pos)
{
    setFormat(format, pos.startLine(), pos.startColumn(), pos.endLine(), pos.endColumn());
}

void EditorHighlighterPrivate::setFormat(const QTextCharFormat &format,
                                         long long int startLine,
                                         long long int startColumn,
                                         long long int endLine,
                                         long long int endColumn)
{
    if (colors.enabled) {
        for (auto i = startLine; i <= endLine; ++i) {
            formats[i].block = editor->document()->findBlockByNumber(i);

            if (formats[i].formats.isEmpty())
                formats[i].formats.fill(QTextCharFormat(), formats[i].block.length() - 1);

            int start = (i == startLine ? startColumn : 0);
            int length = (i == startLine ? (i == endLine ? endColumn - startColumn + 1 : formats[i].block.length() - 1 - startColumn)
                                         : (i == endLine ? endColumn + 1 : formats[i].block.length() - 1));

            for (int j = start; j < start + length; ++j) {
                auto &t = formats[i];
                t.formats[j] = format;
            }
        }
    }
}

void EditorHighlighterPrivate::applyFormatChanges()
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

QFont EditorHighlighterPrivate::styleFont(int opts, bool isSpecial) const
{
    auto f = font;
    auto size = f.pointSize();
    if (!isSpecial || adaptiveTagSize) {
        // Base on KleverStyle.css
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
    }
    if (isSpecial) {
        size = size * tagSizeScale / 100;
    }
    if (size < 1) {
        size = 1;
    }
    f.setPointSize(size);

    if (opts & MD::ItalicText)
        f.setItalic(true);

    if (opts & MD::BoldText)
        f.setBold(true);

    if (opts & MD::StrikethroughText)
        f.setStrikeOut(true);

    return f;
}

// KleverNotes
QColor EditorHighlighterPrivate::getColor(const QString &info)
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

QTextCharFormat EditorHighlighterPrivate::makeFormat(const long long int opts)
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

void EditorHighlighterPrivate::restoreCachedFormats()
{
    for (const auto &[line, value] : cachedFormats.asKeyValueRange()) {
        formats[line] = value;
    }

    cachedFormats.clear();
}

void EditorHighlighterPrivate::revertFormat(const MD::WithPosition &withPosition)
{
    for (int j = withPosition.startColumn(); j < withPosition.endColumn() + 1; ++j) {
        const auto line = withPosition.startLine();
        if (!cachedFormats.contains(line)) {
            cachedFormats[line] = formats[line];
        }
        auto &t = formats[line];
        QTextCharFormat defaultFormat;
        defaultFormat.setForeground(colors.specialColor);
        defaultFormat.setFont(styleFont(0));

        t.formats[j] = defaultFormat;
    }
}

void EditorHighlighterPrivate::revertFormats(const QList<std::pair<MD::WithPosition, MD::WithPosition>> delims)
{
    if (!delims.isEmpty()) {
        for (const auto &pair : delims) {
            revertFormat(pair.first);
            revertFormat(pair.second);
        }
    }
}
} // !namespace MdEditor
