/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "md4qtDataGetter.hpp"

namespace md4qtHelperFunc
{
// Styles

/**
 * @brief Merge the styles of `a` and `b` and puts them into a.
 *
 * @param a A MD::ItemWithOpts which will get the resulting merged style.
 * @param b A MD::ItemWithOpts which styles will be merge into `a`.
 */
void mergeStyle(MDItemWithOptsPtr a, MDItemWithOptsPtr b);

/**
 * @brief Transfert the styles of `a` to `b`.
 *
 * @param a A MD::ItemWithOpts which will give its styles.
 * @param b A MD::ItemWithOpts which will receive the styles.
 * @param transferClose Whether we should transfer the opening or closing styles.
 */
void transferStyle(MDItemWithOptsPtr a, MDItemWithOptsPtr b, const bool transferClose);

// String manip
/**
 * @brief Add the given string to start or end of the given MD::ItemWithOpts.
 *
 * @param item The item which will receive the string.
 * @param atStart Whether the string should be added at the start, otherwise at the end.
 * @param str The string to be added.
 * @param po The current MD::TextParsingOpts used for `rawTextData`.
 * @param atParagraphStart Whether the item is at the start of the Paragraph.
 * @param atParagraphEnd Whether the item is at the end of the Paragraph.
 * @return True if the string was successfully added, false otherwise.
 */
bool addStringTo(const MDItemWithOptsPtr item,
                 const bool atStart,
                 const QString &str,
                 MDParsingOpts &po,
                 const bool atParagraphStart,
                 const bool atParagraphEnd);

/**
 * @brief Try to merge the item located at `from` in the Paragraph with the next one.
 *
 * @param from The index of the item we want to merge with the next one.
 * @param p The current MD::Paragraph being treated.
 * @param po The current MD::TextParsingOpts used for `rawTextData`.
 */
void mergeFromIndex(const long long int from, MDParagraphPtr p, MDParsingOpts &po);

/**
 * @brief Try to split an item and returns the number of resulting items being added.
 *
 * @param p The current MD::Paragraph being treated.
 * @param po The current MD::TextParsingOpts used for `rawTextData`.
 * @param paraIdx The index index in the Paragraph of the item being splitted.
 * @param rawIdx The index index in the rawTextData of the item being splitted.
 * @param from The position were the split should start.
 * @param length The length of the split.
 * @return The number of items being added. [-1, 1]
 */
int splitItem(MDParagraphPtr p,
              MDParsingOpts &po,
              const long long int paraIdx,
              const long long int rawIdx,
              const long long int from,
              const long long int length);
}
