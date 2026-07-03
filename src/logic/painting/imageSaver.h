// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2026 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QQuickItem>

/**
 * @class ImageSaver
 * @brief Helper class for saving image from canvas
 */
class ImageSaver : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit ImageSaver(QObject *parent = nullptr);

    /**
     * @brief Try to save the canvas content to a given file name.
     *
     * @param canvas The QML canvas.
     * @param filePath The file path in which the canvas content must be saved.
     * @param cropping Whether the image should be cropped.
     * @emit finished(bool success)
     */
    Q_INVOKABLE void saveImage(QQuickItem *canvas, const QString &filePath, bool cropping) const;

private:
    /**
     * @brief Get the QRect corresponding to the cropped image.
     *
     * @param image The image to crop.
     * @return A QRect corresponding to the cropped image.
     */
    QRect getCroppingRect(QImage &image) const;

Q_SIGNALS:
    /**
     * @brief Tells QML that the image has been saved.
     *
     * @param success Whether the image was successfully saved.
     */
    void saved(bool success) const;
};
