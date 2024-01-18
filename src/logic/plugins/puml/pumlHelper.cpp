// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "pumlHelper.h"

#include "../cliHelper.h"

bool PumlHelper::makeDiagram(const QString &inputStr, const QString diagName)
{
    const bool pumlExist = CLIHelper::commandExists(QStringLiteral("plantuml"));
    if (pumlExist) {
        const QString pumlError; // Run the command (use execCommand)
        if (!pumlError.isEmpty()) {
            return false;
        }
    }
    return true;
}
