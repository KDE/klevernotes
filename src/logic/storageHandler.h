// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QObject>
#include "kleverUtility.h"

class StorageHandler : public QObject
{
    Q_OBJECT
    KleverUtility util;

public:
    explicit StorageHandler(QObject *parent = nullptr);

public slots:
    void makeStorage(QString storagePath);
    void makeCategory(QString storagePath, QString categoryName);
    void makeGroup(QString categoryPath, QString groupName);
    void makeNote(QString groupPath, QString noteName);

    bool rename(QString oldPath, QString newPath);

    bool remove(QString path);
};
