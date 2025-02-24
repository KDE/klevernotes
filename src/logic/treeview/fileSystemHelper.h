// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

// Qt includes
#include <QString>

namespace fileSystemHelper
{
/*
 * Move all the content from one folder to another
 */
void moveContent(const QString &from, const QString &to);

/*
 * Create a folder to the given path and returns the succes
 */
bool createFolder(const QString &path);

/*
 * Create a file to the given path and returns the succes
 */
bool createFile(const QString &path);
}
