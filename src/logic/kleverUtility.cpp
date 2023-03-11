// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "kleverUtility.h"
#include <QDebug>
#include <QDir>
#include <QString>
#include <QUrl>

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

bool KleverUtility::exists(const QString &url)
{
    return QFile().exists(url);
}

void KleverUtility::create(const QString &path)
{
    if (!exists(path)) {
        QDir().mkpath(path);
    }
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
    qDebug() << imagePath + " C++";
    return imagePath;
}

bool KleverUtility::isEmptyDir(const QString &path)
{
    return !exists(path) || QDir(path).isEmpty();
}
