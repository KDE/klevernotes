// SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

// KleverNotes includes
#include "oldModelConverter.h"

#include "fileSystemHelper.h"

// KDE includes
#include <KLocalizedString>

// Qt includes
#include <QDir>

void moveNote(const QString &parentPath, const QString &path)
{
    const QString actualNotePath = path + QStringLiteral("/note.md");
    if (QFile::exists(actualNotePath)) {
        const QString newNoteFilePath = path + QStringLiteral(".md");

        const QString actualTodoPath = path + QStringLiteral("/todo.json");
        const QString newTodoPath = path + QStringLiteral(".todo.json");

        QFile::rename(actualNotePath, newNoteFilePath);
        QFile::rename(actualTodoPath, newTodoPath);

        static const QString imagesPath = QStringLiteral("/Images/");
        const QString parentImagesPath = parentPath + imagesPath;

        fileSystemHelper::moveContent(path + imagesPath, parentPath);
        QDir(path).removeRecursively();
    }
}

void moveNotes(const QString &path)
{
    const QFileInfoList fileList = QDir(path).entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries | QDir::Filter::AccessMask);
    for (const QFileInfo &file : fileList) {
        moveNote(file.dir().absolutePath(), file.absoluteFilePath());
    }
}

namespace treeModelConverter
{
void convertFileStructure(const QString &path, const int depth_level)
{
    const QFileInfo fileInfo(path);
    Q_ASSERT(fileInfo.exists());

    if (depth_level == 2) {
        moveNotes(path);
        return;
    }

    const QFileInfoList fileList = QDir(path).entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries | QDir::Filter::AccessMask);
    for (const QFileInfo &file : fileList) {
        convertFileStructure(file.absoluteFilePath(), depth_level + 1);

        if (file.fileName() == QStringLiteral(".BaseGroup")) {
            fileSystemHelper::moveContent(file.absoluteFilePath(), file.dir().absolutePath());
        }
    }

    if (fileInfo.fileName() == QStringLiteral(".BaseCategory")) {
        const QString mainFolderName = i18nc("Main folder name, where all the notes will be stored by default", "Notes");
        QFile::rename(fileInfo.absoluteFilePath(), fileInfo.dir().absolutePath() + QStringLiteral("/") + mainFolderName);
    }
}
}
