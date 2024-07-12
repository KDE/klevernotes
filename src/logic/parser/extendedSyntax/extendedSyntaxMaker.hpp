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

struct StyleDelimInfo {
    long long int paraIdx;
    MD::StyleDelim delim;
    TagType type;

    /* For sorting */
    long long int startColumn;
    long long int startLine;
};

struct DelimInfo {
    long long int paraIdx;
    long long int m_startColumn;
    long long int m_startLine;
    long long int endColumn;
    TagType type;

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
 * Get the infos of delims matching the 'searchedDelim' and place them inside 'delimInfos'
 * Get the already applied styles opening/closing pairs and places them inside openCloseStyles
 */
void getDelims(MDParagraphPtr p,
               MDParsingOpts &po,
               const long long int idx,
               QList<DelimInfo> &delimInfos,
               QList<StyleDelimInfo> &waitingOpeningsStyles,
               QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
               const QString &searchedDelim);

/**
 * Check if the openDelim/closeDelim pair is a valid one
 * Add 'bad' styles to the DelimInfo to revert them later
 */
bool validDelimsPairs(QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                      QList<StyleDelimInfo> &badStyles,
                      DelimInfo &openDelim,
                      DelimInfo &closeDelim);

/**
 * Returns a list containing only DelimInfo that have been paired (open/close)
 */
QList<DelimInfo> pairDelims(QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles, QList<StyleDelimInfo> &badStyles, QList<DelimInfo> &delimInfos);

/**
 * Remove the opts and styleDelim of 'bad' styles found by validDelimsPairs
 */
void removeBadStylesOptsAndDelims(MDParagraphPtr p,
                                  const QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                                  const QList<StyleDelimInfo> &badStyles);

/**
 * Add back the 'bad' styles delim text
 */
void restoreBadStylesTexts(MDParagraphPtr p, MDParsingOpts &po, const QList<StyleDelimInfo> &badStyles);

/**
 * Remove the new style delim text from the paragraph item and the rawTextData
 * Add the style delim to the correct paragraph item
 */
void removeDelimText(MDParagraphPtr p, MDParsingOpts &po, const QList<DelimInfo> &pairs, const int newStyleOpt, const int delimLength);

/**
 * Add the opts for the new style
 */
void addNewStyleOpt(MDParagraphPtr p, const QList<DelimInfo> &pairs, const int newStyleOpt);

/**
 * Provide a simple way to execute the whole parsing for a new style
 */
void processExtendedSyntax(MDParagraphPtr p, MDParsingOpts &po, const QString &searchedDelim, const int newStyleOpt);

/**
 * Function exposed to md4qt::Parser::addTextPlugin
 */
void extendedSyntaxHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options);
} // ExtendedSyntaxMaker
