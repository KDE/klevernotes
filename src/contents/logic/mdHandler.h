// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QObject>

class MDHandler : public QObject
{
    Q_OBJECT

public:
    explicit MDHandler(QObject *parent = nullptr);

public slots:
    QJsonArray getLines(const QString &text) const;
    // Would love to use a pair here, but QML doesn't like it :/
    QStringList getPositionLineInfo(QJsonArray lines, int position);
    QJsonObject getInstructions(const QString &selectedText, const QStringList &charsList, const bool checkLineEnd, const bool applyIncrement) const;
};
