/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "logic/parser/md4qtDataGetter.hpp"

namespace NoteLinkingPlugin
{
inline long long int processNoteLinking(MDParagraphPtr p, MDParsingOpts &po, long long int rawIdx);

void noteLinkingHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options);
}
