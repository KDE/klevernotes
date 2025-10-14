/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/
#pragma once

#include <QJsonObject>
#include <QObject>
#include <QRegularExpression>

/**
 * @class NoteMapperUtils
 * @brief Class providing utility methods to the NoteMapper
 */
class NoteMapperUtils : public QObject
{
    Q_OBJECT
public:
    explicit NoteMapperUtils(QObject *parent = nullptr);

    enum SpecialValues {
        InvalidHeaderLevel = -1,
    };

    // TODO: will change with the new Header plugin
    /**
     * @brief Clean the given header to try to make it look like one we could find in a file.
     *
     * @param header The header we want to clean.
     * @return The cleaned version of the header.
     */
    static QString cleanHeader(const QString &header);

    /**
     * @brief Get the given header level.
     *
     * @param header The header for which we want the level.
     * @return The level of the header.
     */
    static int headerLevel(const QString &header);

    /**
     * @brief Get the given header text.
     *
     * @param _header The header for which we want the text.
     * @return The text of the header.
     */
    static QString headerText(const QString &_header);

    /**
     * @brief Convert the JSON object into a QVariantMap.
     *
     * @param savedMap The JSON object to be converted.
     * @return The JSON object in form of a QVariantMap.
     */
    static QVariantMap convertSavedMap(const QJsonObject &savedMap);

    /**
     * @brief Check if the given path info has already been check.
     *
     * @param pathInfo Information about a path.
     * @return True if it has been checked, false otherwise.
     */
    static bool entirelyChecked(const QVariantMap &pathInfo);

    /**
     * @brief Get the saved note headers of the path.
     *
     * @param pathInfo Information about a path.
     * @return A list of all the saved note headers.
     */
    static QStringList getNoteHeaders(const QVariantMap &pathInfo);

    /**
     * @brief Get a list of map containing the header text as key and the full header as the value.
     *
     * @param headers A list of headers to be treated.
     * @return A list of map with the headers info.
     */
    static QList<QVariantMap> getHeadersComboList(const QStringList &headers);
};
