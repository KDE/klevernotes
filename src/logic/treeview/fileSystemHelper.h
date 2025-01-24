// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

// Qt includes
#include <QString>

namespace fileSystemHelper
{
void moveContent(const QString &from, const QString &to);

bool createFolder(const QString &path);

bool createFile(const QString &path);
}
