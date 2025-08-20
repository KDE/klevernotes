// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QFont>
#include <QJsonObject>
#include <QObject>
#include <QQmlEngine>
#include <QUrl>

class KleverUtility : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit KleverUtility(QObject *parent = nullptr);

    Q_INVOKABLE QString getName(const QString &path) const;
    Q_INVOKABLE QString getPath(const QUrl &url) const;
    Q_INVOKABLE bool isEmptyDir(const QString &path) const;
    Q_INVOKABLE QString isProperPath(const QString &parentPath, const QString &name) const;
    Q_INVOKABLE QString getParentPath(const QString &path) const;
    Q_INVOKABLE QString getImageStoragingPath(const QString &noteImagesStoringPath, const QString &wantedName, int iteration = 0) const;
    Q_INVOKABLE bool remove(const QString &path) const;
    Q_INVOKABLE QJsonObject getCssStylesList() const;
    Q_INVOKABLE QJsonObject fontInfo(const QFont &font) const;
    Q_INVOKABLE bool isFlatpak() const;
    // TODO: make Clipboard helper singleton
    Q_INVOKABLE bool checkPaste(const QString &tempPath) const;
    Q_INVOKABLE void copyToClipboard(const QString &toCopy) const;
    static bool create(const QString &path);
    static bool exists(const QString &path);
};
