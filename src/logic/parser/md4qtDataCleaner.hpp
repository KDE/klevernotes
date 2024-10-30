/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "md4qtDataGetter.hpp"

namespace md4qtDataCleaner
{
void removeEmpty(MDParagraphPtr p, MDParsingOpts &po);

void dataCleaningFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options);
}
