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

    Q_INVOKABLE QJsonObject getInstructions(const QString &selectedText,
                                            const QStringList &charsList,
                                            const bool checkLineEnd,
                                            const bool applyIncrement,
                                            const bool checkByBlock) const;

private:
    QJsonObject blockChecker(const QString &selectedText, const QStringList &charsList) const;
};
