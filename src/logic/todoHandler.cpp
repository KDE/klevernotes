// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#include "todoHandler.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QString>

TodoHandler::TodoHandler(QObject *parent)
    : QObject(parent)
{
}

QJsonObject TodoHandler::readTodos(const QString &path) const
{
    QFile file(path);

    QJsonObject todos;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        todos = QJsonDocument::fromJson(file.readAll()).object();
        file.close();
    }
    return todos;
}

void TodoHandler::writeTodos(const QJsonObject &todos, const QString &path)
{
    QJsonDocument doc = QJsonDocument(todos);

    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        return;
    }
    qDebug() << "Error !!";
}
