/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "logic/parser/md4qtDataGetter.hpp"

namespace EmojiExtension
{
inline long long int processEmoji(MDParagraphPtr p, MDParsingOpts &po, long long int rawIdx);

void emojiHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options);
}
