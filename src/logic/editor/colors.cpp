/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "colors.hpp"

namespace MdEditor
{
bool operator!=(const Colors &c1, const Colors &c2)
{
    return (c1.enabled != c2.enabled || c1.backgroundColor != c2.backgroundColor || c1.textColor != c2.textColor || c1.linkColor != c2.linkColor
            || c1.mathColor != c2.mathColor || c1.specialColor != c2.specialColor || c1.titleColor != c2.titleColor || c1.highlightColor != c2.highlightColor
            || c1.codeColor != c2.codeColor || c1.codeBgColor != c2.codeBgColor);
}
}
