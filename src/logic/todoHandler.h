// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QUrl>

class TodoHandler : public QObject
{
    Q_OBJECT

public:
    explicit TodoHandler(QObject *parent = nullptr);

    Q_INVOKABLE QJsonObject readTodos(const QString &path) const;
    Q_INVOKABLE void writeTodos(const QJsonObject &todos, const QString &path);
};
