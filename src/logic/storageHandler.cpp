// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "storageHandler.h"
#include <QDebug>
#include <QDir>
#include <QString>
#include <QUrl>
#include "kleverUtility.h"

StorageHandler::StorageHandler(QObject *parent)
    : QObject(parent)
{

}

void StorageHandler::makeNote(QString groupPath, QString noteName)
{
    QDir noteDir;
    QString notePath = groupPath.append("/"+noteName);
    QFile note(notePath+"/note.html");
    QFile todo(notePath+"/todo.html");
    QFile docu(notePath+"/docu.html");

    if (!noteDir.exists(notePath))
    {
        noteDir.mkpath(notePath);

        note.open(QIODevice::ReadWrite);
        todo.open(QIODevice::ReadWrite);
        docu.open(QIODevice::ReadWrite);
    }
}

void StorageHandler::makeGroup(QString categoryPath, QString groupName)
{
    QDir group;
    QString groupPath = categoryPath.append(groupName);
    if (!group.exists(categoryPath))
    {
        group.mkpath(groupPath);
    }
    QString noteName = "TEST";
    makeNote(groupPath,noteName);
}

void StorageHandler::makeCategory(QString storagePath, QString categoryName)
{
    QDir category;
    QString categoryPath = storagePath.append(categoryName);
    if (!category.exists(categoryPath))
    {
        category.mkpath(storagePath);
    }
    QString groupName = "/.BaseGroup";
    makeGroup(categoryPath,groupName);
}

void StorageHandler::makeStorage(QUrl storageUrl)
{
    util.create(storageUrl);
    QString categoryName = "/.BaseCategory";
    QString storagePath = util.getPath(storageUrl);
    makeCategory(storagePath,categoryName);
}

bool StorageHandler::rename(QString oldPath, QString newPath)
{
    QDir dir;
    return dir.rename(oldPath,newPath);
}
