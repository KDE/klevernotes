/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "subScriptExtension.hpp"
#include "logic/extendedSyntax/extendedSyntaxHelper.hpp"

namespace SubScriptExtension
{
void subScriptExtensionFunc(ExtendedSyntaxHelper::MDParagraphPtr p, ExtendedSyntaxHelper::MDParsingOpts &po)
{
    static const QString searchedDelim = QStringLiteral("--");
    static const int newStyleOpt = MD::TextOption::StrikethroughText * 4;

    ExtendedSyntaxHelper::extendedSyntaxHelperFunc(p, po, searchedDelim, newStyleOpt);
}
}
