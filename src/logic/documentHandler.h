// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QJsonObject>
#include <QObject>
#include <QQmlEngine>
#include <QUrl>

/**
 * @class DocumentHandler
 * @brief Provide methods to perform actions on file to QML.
 */
class DocumentHandler : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit DocumentHandler(QObject *parent = nullptr);

    /**
     * @brief Get the content of a file.
     *
     * @param path The path to the file.
     * @return The content of the file if it exist. An empty string otherwise.
     */
    Q_INVOKABLE static QString readFile(const QString &path);

    /**
     * @brief Write the content (`note`) inside a file located at `path`.
     *
     * @param note The content to be written.
     * @param path The path to the file.
     */
    Q_INVOKABLE static void writeFile(const QString &note, const QString &path);

    /**
     * @brief Try to get the css style located at `path`.
     * If the action fails, the default `KleverStyle` will be used instead.
     *
     * @param path The path to the file.
     * @return The css style located at the path if the file exists. The default `KleverStyle` otherwise.
     */
    Q_INVOKABLE static QString getCssStyle(const QString &path);

    /**
     * @brief Check if the file located at `path` contains the specified header.
     *
     * @param path The path to the file.
     * @param header The header we're looking for.
     * @return True if the header is found, false otherwise.
     */
    static bool checkForHeader(const QString &path, const QString &header);

    /**
     * @brief Try to get the json object contained in the file located at `jsonPath`.
     *
     * @param jsonPath The path to the file containing the wanted json.
     * @return The json object found at the given path. An empty object if the file doesn't exist.
     */
    Q_INVOKABLE static QJsonObject getJson(const QString &jsonPath);

    /**
     * @brief Try to save the given json to a file located at `path`.
     *
     * @param json The json object we want to save.
     * @param path The path to the file.
     * @return True if the save was successful, false otherwise.
     */
    Q_INVOKABLE static bool saveJson(const QJsonObject &json, const QString &path);
};
