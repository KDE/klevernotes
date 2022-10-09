// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QObject>
#include <QJsonObject>
#include <QString>
#include <QVariant>

class View : public QObject
{
    Q_OBJECT

public:
    explicit View(QObject *parent = nullptr);

public slots:
    QJsonObject hierarchy(QString path);
    void print(QString path);
};
