/*
    SPDX-FileCopyrightText: 2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "colors.hpp"

// Qt include.
#include <QColor>

namespace MdEditor
{
//
// Colors
//

bool operator!=(const Colors &c1, const Colors &c2)
{
    return (c1.enabled != c2.enabled || c1.inlineColor != c2.inlineColor || c1.linkColor != c2.linkColor || c1.textColor != c2.textColor
            || c1.htmlColor != c2.htmlColor || c1.codeColor != c2.codeColor || c1.enabled != c2.enabled || c1.mathColor != c2.mathColor
            || c1.specialColor != c2.specialColor);
}
}
