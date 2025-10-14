// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QObject>
#include <QQmlEngine>

/**
 * @class PrintingUtility
 * @brief Class providing utility methods to QML to manage pdf.
 *
 */
class PrintingUtility : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit PrintingUtility(QObject *parent = nullptr);

    /**
     * @brief Create an empty pdf in the given path.
     *
     * @param path The path where the pdf should be created.
     */
    Q_INVOKABLE void writePdf(const QString &path) const;

    /**
     * @brief Copy a pdf from one place to another.
     *
     * @param fromPath The current pdf path.
     * @param toPath The future pdf path.
     *
     * @signal pdfCopyDone Once the action is done.
     */
    Q_INVOKABLE void copy(const QString &fromPath, const QString &toPath);

Q_SIGNALS:
    /**
     * @brief A signal sent once the copy is action is done.
     *
     * @param succes Whether the action was successful or not.
     * @param message The possible error message.
     */
    void pdfCopyDone(const bool succes, const QString &message);
};
