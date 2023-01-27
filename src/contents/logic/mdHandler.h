// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>


class MDHandler : public QObject
{
    Q_OBJECT

public:
    explicit MDHandler(QObject *parent = nullptr);

public slots:
    QJsonArray getLines(QString text);
    // Would love to use a pair here, but QML doesn't like it :/
    QStringList getPositionLineInfo(QJsonArray lines, int position);
    QJsonObject getInstructions(QString selectedText, QStringList charsList);
};
