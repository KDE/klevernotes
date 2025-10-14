// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

// Qt includes
#include <QString>

namespace fileSystemHelper
{
/*
 * @brief Move all the content from one folder to another.
 *
 * @param from The path of the folder to be moved.
 * @param to The path where the folder should be moved.
 */
void moveContent(const QString &from, const QString &to);

/*
 * @brief Create a folder to the given path and returns the succes.
 *
 * @param path The path of the folder that should be created.
 * @return True if the folder was successfully created, false otherwise.
 */
bool createFolder(const QString &path);

/*
 * @brief Create a file to the given path and returns the succes.
 *
 * @param path The path of the file that should be created.
 * @return True if the file was successfully created, false otherwise.
 */
bool createFile(const QString &path);
}
