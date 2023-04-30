// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
#pragma once

#include "kleverUtility.h"
#include <KIO/CopyJob>
#include <QModelIndex>
#include <QObject>
#include <kio_version.h>

class StorageHandler : public QObject
{
    Q_OBJECT
    KleverUtility util;

public:
    explicit StorageHandler(QObject *parent = nullptr);

signals:
    QModelIndex storageUpdated(const QModelIndex &model);

public slots:
    void makeStorage(const QString &storagePath);
    bool makeCategory(const QString &storagePath, const QString &categoryName);
    bool makeGroup(const QString &categoryPath, const QString &groupName);
    bool makeNote(const QString &groupPath, const QString &noteName);

    bool rename(const QString &oldPath, const QString &newPath);

    void remove(const QString &path, const QModelIndex rowIndex);
};
