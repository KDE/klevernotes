// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

#pragma once

// Qt includes
#include <QString>

namespace treeModelConverter
{
/*
 * Convert the old file structure (Category/Group/Note) to the new one.
 *
 * @param path: The path to the current folder
 * @param depth_level: The depth_level of the current folder being converted
 */
void convertFileStructure(const QString &path, const int depth_level = 0);
}
