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

QString KleverUtility::getName(QString path)
{
    return QDir(path).dirName();
}

QString KleverUtility::getPath(QUrl url)
{
    return url.toLocalFile();
}

bool KleverUtility::exists(QString url)
{
    return QFile().exists(url);
}

void KleverUtility::create(QString path)
{
    if (!exists(path)){
        QDir().mkpath(path);
    }
}

QString KleverUtility::getImageStoragingPath(QString noteImagesStoringPath, QString wantedName, int iteration)
{
    create(noteImagesStoringPath);

    QString imagePath = noteImagesStoringPath+wantedName;
    if (iteration != 0) imagePath += "("+QString::number(iteration)+")";
    imagePath += ".png";

    if (exists(imagePath)){
        return getImageStoragingPath(noteImagesStoringPath, wantedName, iteration+1);
    }


    return imagePath;
}
