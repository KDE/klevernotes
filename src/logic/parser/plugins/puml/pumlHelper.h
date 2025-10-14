// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QString>

class PumlHelper
{
public:
    /**
     * @brief Makes a diagram from the input string using PUML.
     *
     * @param inputStr The input string.
     * @param diagName The name of the resulting diagram.
     * @param darkTheme Whether the diagram should have a dark background.
     * @return True if the diagram was successfully made, false otherwise.
     */
    static bool makeDiagram(const QString &inputStr, const QString diagName, const bool darkTheme);
};
