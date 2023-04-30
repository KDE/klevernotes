// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "storageHandler.h"
#include "kleverUtility.h"
#include <QDebug>
#include <QDir>
#include <QString>

StorageHandler::StorageHandler(QObject *parent)
    : QObject(parent)
{
}

bool StorageHandler::makeNote(const QString &groupPath, const QString &noteName)
{
    QDir noteDir;
    QString notePath = groupPath;
    notePath.append(QLatin1Char('/') + noteName);
    QFile note(notePath + "/note.md");
    QFile todo(notePath + "/todo.json");

    bool noteCreated = util.create(notePath);
    if (noteCreated) {
        note.open(QIODevice::ReadWrite);
        note.close();
        todo.open(QIODevice::ReadWrite);
        todo.close();
    }
    return noteCreated;
}

bool StorageHandler::makeGroup(const QString &categoryPath, const QString &groupName)
{
    QString groupPath = categoryPath;
    groupPath.append(QLatin1Char('/') + groupName);
    return util.create(groupPath);
}

bool StorageHandler::makeCategory(const QString &storagePath, const QString &categoryName)
{
    QString categoryPath = storagePath;
    categoryPath.append(QLatin1Char('/') + categoryName);
    return makeGroup(categoryPath, ".BaseGroup");
}

void StorageHandler::makeStorage(const QString &storagePath)
{
    makeCategory(storagePath, "/.BaseCategory");
}

bool StorageHandler::rename(const QString &oldPath, const QString &newPath)
{
    QDir dir;
    return dir.rename(oldPath, newPath);
}

void StorageHandler::remove(const QString &path, const QModelIndex rowIndex)
{
    auto *job = KIO::trash(QUrl::fromLocalFile(path));
    job->start();
    connect(job, &KJob::result, this, [this, job, rowIndex] {
        if (!job->error()) {
            emit storageUpdated(rowIndex);
            return;
        }
        qDebug() << job->errorString();
    });
}
