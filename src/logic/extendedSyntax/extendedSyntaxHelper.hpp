/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QList>

#define MD4QT_QT_SUPPORT
#include "logic/md4qt/doc.hpp"
#include "logic/md4qt/parser.hpp"

namespace ExtendedSyntaxHelper
{

using MDParagraphPtr = std::shared_ptr<MD::Paragraph<MD::QStringTrait>>;
using MDParsingOpts = MD::TextParsingOpts<MD::QStringTrait>;
using MDTextItemPtr = std::shared_ptr<MD::Text<MD::QStringTrait>>;
using MDItemWithOptsPtr = std::shared_ptr<MD::ItemWithOpts<MD::QStringTrait>>;

enum TagType {
    Opening = 0,
    Closing,
    Both,
};

struct StyleDelimInfo {
    long long int paraIdx;
    MDItemWithOptsPtr itemPtr;
    MD::StyleDelim delim;
    TagType type;

    /* For sorting */
    long long int startColumn;
    long long int startLine;
};

struct DelimInfo {
    long long int paraIdx;
    MDItemWithOptsPtr itemPtr;
    long long int startColumn;
    long long int startLine;
    TagType type;

    /* For validDelims */
    bool paired = false;
    bool badOpening = false;
};

/**
 * Get the info of 'searchedDelim' and place them inside 'delimInfos'
 * Get the already applied styles opening/closing pairs and places them inside openCloseStyles
 */
void getDelim(MDParagraphPtr p,
              MDParsingOpts &po,
              const long long int idx,
              QList<DelimInfo> &delimInfos,
              QList<StyleDelimInfo> &waitingOpeningsStyles,
              QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
              QList<long long int> &paraIdxToRawIdx,
              const QString &searchedDelim);

/**
 * Check if the openDelim/closeDelim pair is a valid one
 * Add 'bad' styles to the DelimInfo to revert them later
 */
bool validDelimsPairs(MDParagraphPtr p,
                      QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                      QList<StyleDelimInfo> &badStyles,
                      DelimInfo &openDelim,
                      DelimInfo &closeDelim);

/**
 * Returns a list containing only DelimInfo that have been paired (open/close)
 */
QList<DelimInfo>
pairDelims(MDParagraphPtr p, QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles, QList<StyleDelimInfo> &badStyles, QList<DelimInfo> &delimInfos);

/**
 * Remove the opts and styleDelim of 'bad' styles found by validDelimsPairs
 */
void removeBadStylesOpts(MDParagraphPtr p, const QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles, const QList<StyleDelimInfo> &badStyles);

/**
 * Add the opts and styleDelim for the new style
 */
void addNewStyleOpt(MDParagraphPtr p, const QList<DelimInfo> &pairs, const int newStyleOpt, const int delimLength);

/**
 * Regroups 'removeBadStylesOpts' and 'addNewStyleOpt'
 */
void handleOptsAndStyles(MDParagraphPtr p,
                         const QList<DelimInfo> &pairs,
                         const int newStyleOpt,
                         const int delimLength,
                         const QList<QPair<StyleDelimInfo, StyleDelimInfo>> &openCloseStyles,
                         const QList<StyleDelimInfo> &badStyles);

/**
 * Transfert the opening/closing style (depends on newBeforeExisting) from the existingItem to the newItem
 * This will also handle the "setSpaceBefore/After" for both
 */
void transferStyles(const StyleDelimInfo &styleInfo, MDItemWithOptsPtr &existingItem, MDTextItemPtr &newItem, const bool newBeforeExisting);

/**
 * Add back the 'bad' styles delim text
 */
void restoreBadStyleText(MDParagraphPtr p, MDParsingOpts &po, const QList<StyleDelimInfo> &badStyles, QList<long long int> &paraIdxToRawIdx);

/**
 * Remove the new style delim text from the paragraph and the rawTextData
 */
void removeDelimText(MDParagraphPtr p,
                     MDParsingOpts &po,
                     const QList<DelimInfo> &pairs,
                     QList<long long int> &paraIdxToRawIdx,
                     const int newStyleOpt,
                     const int delimLength);

/**
 * Regroups 'restoreBadStyleText' and 'removeDelimText'
 */
void handleText(MDParagraphPtr p,
                MDParsingOpts &po,
                QList<DelimInfo> &pairs,
                QList<long long int> &paraIdxToRawIdx,
                const int delimLength,
                const int newStyleOpt,
                QList<StyleDelimInfo> &badStyles);

/**
 * Provide a simple way to execute the whole parsing for a new style
 */
void processExtendedSyntax(MDParagraphPtr p, MDParsingOpts &po, const QString &searchedDelim, const int newStyleOpt);

void extendedSyntaxHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options);
} // ExtendedSyntaxHelper
