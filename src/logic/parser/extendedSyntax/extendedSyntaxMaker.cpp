/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "extendedSyntaxMaker.hpp"

namespace ExtendedSyntaxMaker
{

MD::ItemWithOpts::Styles makeStylesByTwo(int style, qsizetype startPos, qsizetype lineNumber, qsizetype length)
{
    MD::ItemWithOpts::Styles styles;

    for (auto i = 0; i < length; i += 2) {
        styles.append(MD::StyleDelim(style, startPos + i, lineNumber, startPos + i + 1, lineNumber));
    }

    return styles;
}

MD::ItemWithOpts::Styles makeStyles(int style, qsizetype startPos, qsizetype lineNumber, qsizetype length)
{
    MD::ItemWithOpts::Styles styles;

    for (auto i = 0; i < length; ++i) {
        styles.append(MD::StyleDelim(style, startPos, lineNumber, startPos, lineNumber));
        ++startPos;
    }

    return styles;
}

//
// SubEmphasisParser
//

SubEmphasisParser::SubEmphasisParser()
    : m_symbol(QLatin1Char('-'))
{
}

const QChar &SubEmphasisParser::symbol() const
{
    return m_symbol;
}

bool SubEmphasisParser::isEmphasis(int length) const
{
    return (length % 2 == 0);
}

bool SubEmphasisParser::isLengthCorrespond() const
{
    return false;
}

MD::ItemWithOpts::Styles SubEmphasisParser::openStyles(qsizetype startPos, qsizetype lineNumber, qsizetype length) const
{
    return makeStylesByTwo(16, startPos, lineNumber, length);
}

MD::ItemWithOpts::Styles SubEmphasisParser::closeStyles(qsizetype startPos, qsizetype lineNumber, qsizetype length) const
{
    return makeStylesByTwo(16, startPos, lineNumber, length);
}

//
// SupEmphasisParser
//

SupEmphasisParser::SupEmphasisParser()
    : m_symbol(QLatin1Char('^'))
{
}

const QChar &SupEmphasisParser::symbol() const
{
    return m_symbol;
}

bool SupEmphasisParser::isEmphasis(int) const
{
    return true;
}

bool SupEmphasisParser::isLengthCorrespond() const
{
    return false;
}

MD::ItemWithOpts::Styles SupEmphasisParser::openStyles(qsizetype startPos, qsizetype lineNumber, qsizetype length) const
{
    return makeStyles(32, startPos, lineNumber, length);
}

MD::ItemWithOpts::Styles SupEmphasisParser::closeStyles(qsizetype startPos, qsizetype lineNumber, qsizetype length) const
{
    return makeStyles(32, startPos, lineNumber, length);
}

//
// HighlightEmphasisParser
//

HighlightEmphasisParser::HighlightEmphasisParser()
    : m_symbol(QLatin1Char('='))
{
}

const QChar &HighlightEmphasisParser::symbol() const
{
    return m_symbol;
}

bool HighlightEmphasisParser::isEmphasis(int length) const
{
    return (length % 2 == 0);
}

bool HighlightEmphasisParser::isLengthCorrespond() const
{
    return false;
}

MD::ItemWithOpts::Styles HighlightEmphasisParser::openStyles(qsizetype startPos, qsizetype lineNumber, qsizetype length) const
{
    return makeStylesByTwo(8, startPos, lineNumber, length);
}

MD::ItemWithOpts::Styles HighlightEmphasisParser::closeStyles(qsizetype startPos, qsizetype lineNumber, qsizetype length) const
{
    return makeStylesByTwo(8, startPos, lineNumber, length);
}

} // !ExtendedSyntaxMaker
