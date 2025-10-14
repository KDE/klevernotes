// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022-2025 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QFont>
#include <QJsonObject>
#include <QObject>
#include <QQmlEngine>
#include <QUrl>

/**
 * @class KleverUtility
 * @brief Provide different utility methods to QML.
 */
class KleverUtility : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit KleverUtility(QObject *parent = nullptr);

    /**
     * @brief Get the name of the file located at `path`.
     *
     * @param path The path to the file.
     * @return The name of the file if it exist, an empty string otherwise.
     */
    Q_INVOKABLE QString getName(const QString &path) const;

    /**
     * @brief Get a local path from the given url.
     *
     * @param url The url we want to convert to a path.
     * @return A local path in form of a string.
     */
    Q_INVOKABLE QString getPath(const QUrl &url) const;

    /**
     * @brief Check if the directory located at `path` is empty.
     *
     * @param path The path to the directory.
     * @return True if the directory exists and is empty, false otherwise.
     */
    Q_INVOKABLE bool isEmptyDir(const QString &path) const;

    // TODO: rename this
    /**
     * @brief Check if the combination of `parentPath` and `name` makes a valid path.
     * This method will return an empty string if the path is valid and one of the following error if it is not:
     * - "dot" => the given name start with a `.`
     * - "/" => the given name contains a `/`
     * - "exist" => the resulting path already exist
     *
     * @param parentPath The path to the parent directory.
     * @param name The wanted name for the file.
     * @return A string containing one of the mentionned error, or an empty string (valid).
     */
    Q_INVOKABLE QString isProperPath(const QString &parentPath, const QString &name) const;

    /**
     * @brief Get the parent path for a given path.
     *
     * @param path The path for which we want to know the parent.
     * @return The parent path.
     */
    Q_INVOKABLE QString getParentPath(const QString &path) const;

    /**
     * @brief Provide a valid name for an image to be saved based on the location we want to saved it and the wanted name.
     * The resulting name of the file could be followed by numbers if another file with the same wanted name is already saved at this location.
     *
     * @param noteImagesStoringPath The path to the directory were the image would be saved
     * @param wantedName The wanted name for the image.
     * @param iteration The number of iteration the combination as been through.
     * @return A valid path where the image can be saved.
     */
    Q_INVOKABLE QString getImageStoragingPath(const QString &noteImagesStoringPath, const QString &wantedName, int iteration = 0) const;

    /**
     * @brief Remove the file located at `path`.
     *
     * @param path The path to the file.
     * @return True if the file was successfully remoded, false otherwise.
     */
    Q_INVOKABLE bool remove(const QString &path) const;

    /**
     * @brief Get the json object containing info about the different css styles available.
     * Keys are the name of the styles, values are path to those styles.
     *
     * If the default directory where new styles can be stored is not found,
     * this method will create it and add an example style file.
     *
     * @return A json object with info about the different available styles.
     */
    Q_INVOKABLE QJsonObject getCssStylesList() const;

    /**
     * @brief Get the important info about the given font.
     *
     * @param font A json object containing the important info of the font.
     */
    Q_INVOKABLE QJsonObject fontInfo(const QFont &font) const;

    /**
     * @brief Check if the currently running instance of the app is a flatpak.
     *
     * @return True if it is a flatpak, false otherwise.
     */
    Q_INVOKABLE bool isFlatpak() const;

    // TODO: make Clipboard helper singleton
    // TODO: rename this
    /**
     * @brief Check if the clipboard contains an image that can be temporarily saved at the given path.
     *
     * @param tempPath The path were the image is temporarily saved.
     * @return True if it is an image that has successfully been saved temporarily, false otherwise.
     */
    Q_INVOKABLE bool checkPaste(const QString &tempPath) const;

    /**
     * @brief Copy a given string to the system clipboard.
     *
     * @param toCopy The string to copy.
     * @return True if the string was successfully copied to the clipboard, false otherwise.
     */
    Q_INVOKABLE void copyToClipboard(const QString &toCopy) const;

    /**
     * @brief Create a file/directory located at the given path if it doesn't already exists.
     *
     * @param path The path where the file/directory should be created.
     * @return True if the file/directory didn't exists and was successfully created, false otherwise.
     */
    static bool create(const QString &path);

    /**
     * @brief Check if a file/directory exists at the given path.
     *
     * @param path The path to check.
     * @return True if it exists, false otherwise.
     */
    static bool exists(const QString &path);
};
