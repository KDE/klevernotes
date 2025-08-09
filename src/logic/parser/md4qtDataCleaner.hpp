/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "md4qtDataGetter.hpp"

namespace md4qtDataCleaner
{
const MD::TextPlugin dataCleanerId = static_cast<MD::TextPlugin>(1024);

void removeEmpty(MDParagraphPtr p, MDParsingOpts &po);

void dataCleaningFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options);
}
