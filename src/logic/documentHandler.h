// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QJsonObject>
#include <QObject>
#include <QUrl>

class DocumentHandler : public QObject
{
    Q_OBJECT

public:
    explicit DocumentHandler(QObject *parent = nullptr);

    Q_INVOKABLE QString readFile(const QString &path) const;
    Q_INVOKABLE void writeFile(const QString &note, const QString &path);
    Q_INVOKABLE QString getCssStyle(const QString &path) const;

    bool checkForHeader(const QString &path, const QString &header);
    QJsonObject getSavedMap(const QString &mapPath);
    bool saveMap(const QJsonObject &map, const QString &path);
};
