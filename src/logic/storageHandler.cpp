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

    util.create(notePath);
    note.open(QIODevice::ReadWrite);
    todo.open(QIODevice::ReadWrite);
    docu.open(QIODevice::ReadWrite);
}

void StorageHandler::makeGroup(QString categoryPath, QString groupName)
{
    QString groupPath = categoryPath.append("/"+groupName);
    util.create(groupPath);
}

void StorageHandler::makeCategory(QString storagePath, QString categoryName)
{
    QString categoryPath = storagePath.append("/"+categoryName);
    makeGroup(categoryPath,".BaseGroup");
}

void StorageHandler::makeStorage(QString storagePath)
{
    makeCategory(storagePath,"/.BaseCategory");
}

bool StorageHandler::rename(QString oldPath, QString newPath)
{
    QDir dir;
    return dir.rename(oldPath,newPath);
}
