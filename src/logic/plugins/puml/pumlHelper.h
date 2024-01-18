// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QString>

class PumlHelper
{
public:
    static bool makeDiagram(const QString &inputStr, const QString diagName);
};
