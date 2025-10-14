/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "../md4qtDataGetter.hpp"

namespace ExtendedSyntaxMaker
{

enum TagType {
    Opening = 0,
    Closing,
    Both,
};

/**
 * @class StyleDelimInfo
 * @brief Structure holding info about already existing style delims
 */
struct StyleDelimInfo {
    long long int paraIdx;
    MD::StyleDelim delim;
    TagType type;

    /* For sorting */
    long long int startColumn;
    long long int startLine;
};

/**
 * @class DelimInfo
 * @brief Structure holding info about potential new style delims
 */
struct DelimInfo {
    long long int paraIdx;
    long long int m_startColumn;
    long long int m_startLine;
    long long int endColumn;
    TagType type;
    bool atLineEnd;

    /* For validDelims */
    bool paired = false;
    bool badOpening = false;

    /* For sorting */
    long long int startColumn() const
    {
        return m_startColumn;
    }

    long long int startLine() const
    {
        return m_startLine;
    }
};

/**
 * @brief Get the infos of delims matching the 'searchedDelim' and place them inside 'delimInfos'
 * Get the already applied styles opening/closing pairs and places them inside openCloseStyles
 *
 * @param p The current MD::Paragraph being treated
 * @param po The current MD::TextParsingOpts used for `rawTextData`
 * @param idx Index of text (in raw text)
 * @param delimInfos List where all the gathered DelimInfo will be added
 * @param waitingOpeningsStyles List of all the gathered opening StyleDelimInfo waiting to be paired
 * @param openCloseStyles List of all the gathered StyleDelimInfo opening/closing already paired
 * @param searchedDelim Delimiter text being search in the paragraph
 */
void getDelims(MDParagraphPtr p,
               MDParsingOpts &po,
               const long long int idx,
               QList<DelimInfo> &delimInfos,
               QList<StyleDelimInfo> &waitingOpeningsStyles,
               QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
               const QString &searchedDelim);

/**
 * @brief Check if the openDelim/closeDelim pair is a valid one
 * Add 'bad' styles to the DelimInfo to revert them later
 *
 * @param openCloseStyles List of all the gathered StyleDelimInfo opening/closing already paired
 * @param badStyles List where all the StyleDelimInfo that should be reverted will be added
 * @param openDelim DelimInfo to be checked with the `closeDelim` to make a valid pair
 * @param closeDelim DelimInfo to be checked with the `openDelim` to make a valid pair
 *
 * @return true if the pair open/close delim is a valid one, false otherwise
 */
bool validDelimsPairs(QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                      QList<StyleDelimInfo> &badStyles,
                      DelimInfo &openDelim,
                      DelimInfo &closeDelim);

/**
 * @brief Pair delimInfo that can be paired
 *
 * @param openCloseStyles List of all the gathered StyleDelimInfo opening/closing already paired
 * @param badStyles List where all the StyleDelimInfo that should be reverted will be added
 * @param delimInfos List of all the already gathered DelimInfo
 *
 * @return A list of valid DelimInfo
 */
QList<DelimInfo> pairDelims(QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles, QList<StyleDelimInfo> &badStyles, QList<DelimInfo> &delimInfos);

/**
 * @brief Remove the opts and styleDelim of 'bad' styles found by validDelimsPairs
 *
 * @param p The current MD::Paragraph being treated
 * @param openCloseStyles List of all the gathered StyleDelimInfo opening/closing already paired
 * @param badStyles List of all the StyleDelimInfo that should be reverted
 */
void removeBadStylesOptsAndDelims(MDParagraphPtr p,
                                  const QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                                  const QList<StyleDelimInfo> &badStyles);

/**
 * @brief Add back the 'bad' styles delim text
 *
 * @param p The current MD::Paragraph being treated
 * @param po The current MD::TextParsingOpts used for `rawTextData`
 * @param badStyles List of all the StyleDelimInfo that should be reverted
 */
void restoreBadStylesTexts(MDParagraphPtr p, MDParsingOpts &po, const QList<StyleDelimInfo> &badStyles);

/**
 * @brief Remove the new style delim text from the paragraph item and the rawTextData
 * Add the style delim to the correct paragraph item
 *
 * @param p The current MD::Paragraph being treated
 * @param po The current MD::TextParsingOpts used for `rawTextData`
 * @param pairs List of all the newly paired delims
 * @param newStyleOpt Value of the new style option to be added to the items affected by the delims
 * @param delimLength Length of the delim to be removed
 */
void removeDelimText(MDParagraphPtr p, MDParsingOpts &po, const QList<DelimInfo> &pairs, const int newStyleOpt, const int delimLength);

/**
 * @brief Add the opts for the new style
 *
 * @param p The current MD::Paragraph being treated
 * @param pairs List of all the newly paired delims
 * @param newStyleOpt Value of the new style option to be added to the items affected by the delims
 */
void addNewStyleOpt(MDParagraphPtr p, const QList<DelimInfo> &pairs, const int newStyleOpt);

/**
 * @brief Provide a simple way to execute the whole parsing for a new style
 *
 * @param p The current MD::Paragraph being treated
 * @param po The current MD::TextParsingOpts used for `rawTextData`
 * @param searchedDelim Delimiter text being search in the paragraph
 * @param newStyleOpt Value of the new style option to be added to the items affected by the delims
 */
void processExtendedSyntax(MDParagraphPtr p, MDParsingOpts &po, const QString &searchedDelim, const int newStyleOpt);

/**
 * @brief Function exposed to md4qt::Parser::addTextPlugin
 *
 * @param p The current MD::Paragraph being treated
 * @param po The current MD::TextParsingOpts used for `rawTextData`
 * @param options Options of this plugins
 */
void extendedSyntaxHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options);
} // ExtendedSyntaxMaker
