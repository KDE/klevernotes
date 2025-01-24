// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

// KleverNotes includes
#include "fileSystemHelper.h"

// Qt includes
#include <QDir>

namespace fileSystemHelper
{
void moveContent(const QString &from, const QString &to)
{
    QDir dir(from);
    if (dir.exists() && QDir(to).exists()) {
        const QFileInfoList fileList = dir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries | QDir::Filter::AccessMask);
        for (const QFileInfo &file : fileList) {
            QFile::rename(file.absoluteFilePath(), to + QStringLiteral("/") + file.fileName());
        }
        dir.removeRecursively();
    }
}

bool createFolder(const QString &path)
{
    const QDir folder(path);
    if (!folder.exists()) {
        return folder.mkpath(path);
    }
    return false;
}

bool createFile(const QString &path)
{
    QFile file(path);
    bool creationSucces = file.open(QIODevice::ReadWrite);
    file.close();
    return creationSucces;
}
}
