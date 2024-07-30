/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

// Qt include.
#include <QColor>

namespace MdEditor
{
struct Colors {
    QColor textColor = QColor(64, 128, 255);
    QColor linkColor = QColor(0, 128, 0);
    QColor mathColor = QColor(128, 0, 0);
    QColor specialColor = QColor(32, 64, 128);
    QColor titleColor = Qt::gray;
    QColor highlightColor = Qt::cyan;
    QColor codeColor = Qt::white;
    QColor codeBgColor = Qt::black;
    bool enabled = true;
};

bool operator!=(const Colors &c1, const Colors &c2);
}
