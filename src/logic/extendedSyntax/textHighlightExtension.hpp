/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QList>

#define MD4QT_QT_SUPPORT
#include "logic/md4qt/doc.hpp"
#include "logic/md4qt/parser.hpp"

namespace TextHighlightFunc
{

using MDParagraphPtr = std::shared_ptr<MD::Paragraph<MD::QStringTrait>>;
using MDParsingOpts = MD::TextParsingOpts<MD::QStringTrait>;

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
};

struct DelimInfo {
    long long int paraIdx;
    long long int startColumn;
    long long int startLine;
    TagType type;

    /* WARNING: temp, for debug */
    QString rawLine;
    /* ======================== */

    /* For validDelims */
    bool paired = false;
    bool badOpening = false;
};

/*
 * Get the info of 'searchedDelim' and place them inside 'delimInfos'
 * Get the already applied styles opening/closing pairs and places them inside openCloseStyles
 */
void getDelim(MDParagraphPtr p,
              MDParsingOpts &po,
              long long int idx,
              QList<DelimInfo> &delimInfos,
              QList<StyleDelimInfo> &waitingOpeningsStyles,
              QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
              QList<long long int> &paraIdxToRawIdx,
              const QString &searchedDelim);

/*
 * Check if the openDelim/closeDelim pairs is a valid one
 * Add 'bad' style to the DelimInfo to revert them later
 */
bool validDelimsPairs(MDParagraphPtr p,
                      QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                      QList<StyleDelimInfo> &badStyles,
                      DelimInfo &openDelim,
                      DelimInfo &closeDelim);

/*
 * Returns a list containing only DelimInfo that have been paired (open/close)
 */
QList<DelimInfo>
pairDelims(MDParagraphPtr p, QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles, QList<StyleDelimInfo> &badStyles, QList<DelimInfo> &delimInfos);

void removeBadStylesOpts(MDParagraphPtr p, QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles, QList<StyleDelimInfo> &badStyles);

void restoreBadStyleText(MDParagraphPtr p, MDParsingOpts &po, QList<StyleDelimInfo> &badStyles, QList<long long int> &paraIdxToRawIdx);

void removeBadStyles(MDParagraphPtr paragraphsList,
                     MDParsingOpts &po,
                     QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                     QList<StyleDelimInfo> &badStyles,
                     QList<long long int> &paraIdxToRawIdx);

void addNewStyleOpt(MDParagraphPtr p, QList<DelimInfo> &pairs, const int newStyleOpt, const int delimLength);

void removeDelimText(MDParagraphPtr p,
                     MDParsingOpts &po,
                     QList<DelimInfo> &pairs,
                     QList<long long int> &paraIdxToRawIdx,
                     const int newStyleOpt,
                     const int delimLength);

void addNewStyles(MDParagraphPtr p, MDParsingOpts &po, QList<DelimInfo> &pairs, QList<long long int> &paraIdxToRawIdx, const int delimLength);

void textHighlightExtension(MDParagraphPtr p, MDParsingOpts &po);

} // TextHighlightFunc
