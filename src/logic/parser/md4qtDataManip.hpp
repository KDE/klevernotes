/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "md4qtDataGetter.hpp"

namespace md4qtHelperFunc
{
// Styles
void mergeStyle(MDItemWithOptsPtr a, MDItemWithOptsPtr b);

void transferStyle(MDItemWithOptsPtr a, MDItemWithOptsPtr b, const bool transferClose);

// String manip
bool addStringTo(const MDItemWithOptsPtr item,
                 const bool atStart,
                 const QString &str,
                 MDParsingOpts &po,
                 const bool atParagraphStart,
                 const bool atParagraphEnd);

void mergeFromIndex(const long long int from, MDParagraphPtr p, MDParsingOpts &po);

int splitItem(MDParagraphPtr p,
              MDParsingOpts &po,
              const long long int paraIdx,
              const long long int rawIdx,
              const long long int from,
              const long long int length);
}
