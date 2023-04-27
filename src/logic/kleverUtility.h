// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QObject>
#include <QUrl>

class KleverUtility : public QObject
{
    Q_OBJECT

public:
    explicit KleverUtility(QObject *parent = nullptr);

public slots:
    QString getName(const QString &path);
    QString getPath(const QUrl &url);
    bool exists(const QString &path);
    bool create(const QString &path);
    QString getImageStoragingPath(const QString &noteImagesStoringPath, const QString &wantedName, int iteration = 0);
    bool isEmptyDir(const QString &path);
    QString isProperPath(const QString &parentPath, const QString &name);
    QString getParentPath(const QString &path);
};
