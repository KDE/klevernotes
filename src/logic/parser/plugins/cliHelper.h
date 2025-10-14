// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QString>

/**
 * @class CLIHelper
 * @brief Helper class to interact with the shell.
 */
class CLIHelper
{
public:
    /**
     * @brief Try to find the executable path based on the command name.
     *
     * @param command The name of the command.
     * @return The path if it is found, an empty string otherwise.
     */
    static QString findExecutable(const QString &command);

    /**
     * @brief Execute the given input inside a `sh` shell.
     *
     * @param input The command (with args) to be executed.
     * @return The output of the command.
     */
    static QString execCommand(const QString &input);

    /**
     * @brief Check if the given command exists.
     *
     * @param command The command we want to check.
     * @return True if it exists, false otherwise.
     */
    static bool commandExists(const QString &command);
};
