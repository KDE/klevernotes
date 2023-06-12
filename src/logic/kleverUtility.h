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

    Q_INVOKABLE QString getName(const QString &path);
    Q_INVOKABLE QString getPath(const QUrl &url);
    Q_INVOKABLE bool isEmptyDir(const QString &path);
    Q_INVOKABLE QString isProperPath(const QString &parentPath, const QString &name);
    Q_INVOKABLE QString getParentPath(const QString &path);
    Q_INVOKABLE QString getImageStoragingPath(const QString &noteImagesStoringPath, const QString &wantedName, int iteration = 0);
    static bool create(const QString &path);
    static bool exists(const QString &path);
};
