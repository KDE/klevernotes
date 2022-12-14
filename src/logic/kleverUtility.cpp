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

bool KleverUtility::exist(QString url)
{
    return QDir().exists(url);
}

void KleverUtility::create(QUrl url)
{
    QDir dir;

    QString path = getPath(url);
    if (!exist(path))
    {
        dir.mkpath(path);
    }
}
