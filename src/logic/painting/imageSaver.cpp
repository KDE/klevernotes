// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2026 Louis Schul <schul9louis@gmail.com>

// KleverNotes include
#include "imageSaver.h"

// Qt include
#include <QQuickItemGrabResult>
#include <QSharedPointer>

ImageSaver::ImageSaver(QObject *parent)
    : QObject(parent)
{
}

QRect ImageSaver::getCroppingRect(QImage &image) const
{
    int minX = 0;
    int maxX = image.width();
    int minY = 0;
    int maxY = image.height();

    int startX = maxX;
    int endX = minX;
    int startY = maxY;
    int endY = minY;
    for (int y = 0; y < image.height(); ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(image.scanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            if (qAlpha(line[x]) != 0) {
                startX = std::min(startX, x);
                endX = std::max(endX, x);
                startY = std::min(startY, y);
                endY = std::max(endY, y);
            }
        }
    }

    const QRect originalRect = QRect(0, 0, 1024, 1024);
    const int padding = 10;
    const QRect croppedRect = QRect((startX - padding), (startY - padding), (endX - startX + padding), (endY - startY + padding));

    return croppedRect.intersected(originalRect);
}

void ImageSaver::saveImage(QQuickItem *item, const QString &filePath, bool cropping) const
{
    if (!item) {
        Q_EMIT saved(false);
        return;
    }

    QSharedPointer<const QQuickItemGrabResult> grabResult = item->grabToImage();
    connect(grabResult.data(), &QQuickItemGrabResult::ready, this, [this, grabResult, filePath, cropping]() {
        QImage fullImage = grabResult->image();
        if (fullImage.isNull()) {
            Q_EMIT saved(false);
            return;
        }

        if (cropping) {
            QRect croppedRect = this->getCroppingRect(fullImage);
            QImage croppedImage = fullImage.copy(croppedRect);

            Q_EMIT this->saved(croppedImage.save(filePath));
        } else {
            Q_EMIT this->saved(fullImage.save(filePath));
        }
    });
}
