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
    QString getName(QString path);
    QString getPath(QUrl url);
    bool exists(QString path);
    void create(QString path);
    QString getImageStoragingPath(QString noteImagesStoringPath, QString wantedName, int iteration=0);
};

