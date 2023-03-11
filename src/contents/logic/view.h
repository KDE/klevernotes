// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVariant>

class View : public QObject
{
    Q_OBJECT

public:
    explicit View(QObject *parent = nullptr);

signals:
    void hierarchySent(QJsonObject);

public slots:
    QJsonObject getHierarchy(QString path, int lvl);
    void hierarchySupplier(QString path, int lvl);
};
