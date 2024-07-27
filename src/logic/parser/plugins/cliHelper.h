// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QString>

class CLIHelper
{
public:
    static QString findExecutable(const QString &command);
    static QString execCommand(const QString &input);
    static bool commandExists(const QString &command);
};
