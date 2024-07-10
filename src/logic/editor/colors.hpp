/*
    SPDX-FileCopyrightText: 2024 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

// Qt include.
#include <QColor>
#include <qcolor.h>

namespace MdEditor
{
//
// Colors
//

//! Color scheme.
struct Colors {
    QColor textColor = QColor(0, 0, 128);
    QColor linkColor = QColor(0, 128, 0);
    QColor mathColor = QColor(128, 0, 0);
    QColor specialColor = QColor(128, 0, 0);
    QColor titleColor = QColor(0, 0, 128);
    QColor highlightColor = Qt::cyan;
    QColor codeColor = Qt::black;
    QColor codeBgColor = Qt::black;
    bool enabled = true;
}; // struct Colors

bool operator!=(const Colors &c1, const Colors &c2);
}
