// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QJsonObject>
#include <QObject>
#include <QQmlEngine>
#include <QUrl>

class DocumentHandler : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit DocumentHandler(QObject *parent = nullptr);

    Q_INVOKABLE static QString readFile(const QString &path);
    Q_INVOKABLE static void writeFile(const QString &note, const QString &path);
    Q_INVOKABLE static QString getCssStyle(const QString &path);

    static bool checkForHeader(const QString &path, const QString &header);
    Q_INVOKABLE static QJsonObject getJson(const QString &jsonPath);
    Q_INVOKABLE static bool saveJson(const QJsonObject &json, const QString &path);
};
