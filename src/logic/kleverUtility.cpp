// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "kleverUtility.h"
#include <QDebug>
#include <QDir>
#include <QString>
#include <QUrl>
#include <kio/global.h>

KleverUtility::KleverUtility(QObject *parent)
    : QObject(parent)
{
}

QString KleverUtility::getName(const QString &path)
{
    return QDir(path).dirName();
}

QString KleverUtility::getPath(const QUrl &url)
{
    return url.toLocalFile();
}

bool KleverUtility::exists(const QString &path)
{
    return QFile().exists(path);
}

bool KleverUtility::create(const QString &path)
{
    if (!exists(path)) {
        return QDir().mkpath(path);
    }
    return false;
}

QString KleverUtility::getImageStoragingPath(const QString &noteImagesStoringPath, const QString &wantedName, int iteration)
{
    create(noteImagesStoringPath);

    QString imagePath = noteImagesStoringPath + wantedName;
    if (iteration != 0)
        imagePath += "(" + QString::number(iteration) + ")";
    imagePath += ".png";

    if (exists(imagePath)) {
        return getImageStoragingPath(noteImagesStoringPath, wantedName, iteration + 1);
    }
    return imagePath;
}

bool KleverUtility::isEmptyDir(const QString &path)
{
    return !exists(path) || QDir(path).isEmpty();
}

QString KleverUtility::isProperPath(const QString &parentPath, const QString &name)
{
    if (name.startsWith("."))
        return "dot";

    QString properName = KIO::encodeFileName(name);

    QString newPath = parentPath + "/" + properName;

    return (exists(newPath)) ? "exist" : "";
}

QString KleverUtility::getParentPath(const QString &path)
{
    QDir dir(path);
    dir.cdUp();
    return dir.absolutePath();
}

bool KleverUtility::remove(const QString &path)
{
    QFile file(path);
    return file.remove();
}

