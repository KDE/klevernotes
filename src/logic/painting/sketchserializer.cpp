// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

/*
 * Adapted by Louis Schul <schul9louis@gmail.com>
 * in 2023 for Klevernotes
 */

#include "sketchserializer.h"

#include <functional>
#include <memory>

#include <QImageWriter>
#include <QPainter>
#include <QSvgGenerator>
#include <QUrl>

#include "sketchmodel.h"

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

// See: https://stackoverflow.com/a/17902439
namespace std
{
template<class T>
struct _Unique_if {
    typedef unique_ptr<T> _Single_object;
};

template<class T>
struct _Unique_if<T[]> {
    typedef unique_ptr<T[]> _Unknown_bound;
};

template<class T, size_t N>
struct _Unique_if<T[N]> {
    typedef void _Known_bound;
};

template<class T, class... Args>
typename _Unique_if<T>::_Single_object make_unique(Args &&...args)
{
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<class T>
typename _Unique_if<T>::_Unknown_bound make_unique(size_t n)
{
    typedef typename remove_extent<T>::type U;
    return unique_ptr<T>(new U[n]());
}

template<class T, class... Args>
typename _Unique_if<T>::_Known_bound make_unique(Args &&...) = delete;
}

class Serializer
{
public:
    Serializer(std::unique_ptr<QPaintDevice> &&device, const std::function<void(QPaintDevice *)> writer = {})
        : m_device(std::move(device))
        , m_writer(writer)
    {
    }

    ~Serializer()
    {
        if (m_writer)
            m_writer(m_device.get());
    }

    QPaintDevice *device() const
    {
        return m_device.get();
    }

private:
    std::unique_ptr<QPaintDevice> m_device;
    std::function<void(QPaintDevice *)> m_writer;
};

std::unique_ptr<Serializer> createSerializer(const QSize &size, const QString &fileName)
{
    if (fileName.endsWith(".svg")) {
        auto generator = std::make_unique<QSvgGenerator>();
        generator->setFileName(fileName);
        generator->setViewBox(QRect{{0, 0}, size});
        return std::make_unique<Serializer>(std::move(generator));
    } else {
        auto image = std::make_unique<QImage>(size, QImage::Format_RGB32);
        image->fill(Qt::white);

        auto writeImage = [fileName](QPaintDevice *image) {
            QImageWriter writer(fileName);
            writer.write(*static_cast<QImage *>(image));
        };

        return std::make_unique<Serializer>(std::move(image), writeImage);
    }
}

void SketchSerializer::serialize(SketchModel *model, const QSize &size, const QUrl &fileUrl)
{
    Q_ASSERT(fileUrl.isLocalFile());

    auto strokes = model->strokes();
    auto serializer = createSerializer(size, fileUrl.toLocalFile());
    auto painter = std::make_unique<QPainter>(serializer->device());
    painter->setRenderHint(QPainter::Antialiasing);

    for (const auto &stroke : qAsConst(strokes)) {
        m_strokepainter.render(stroke, painter.get());
    }
}

QRect SketchSerializer::getCropRect()
{
    int lowestX = (m_strokepainter.lowestX - 50 < 0) ? 0 : m_strokepainter.lowestX - 50;
    int lowestY = (m_strokepainter.lowestY - 50 < 0) ? 0 : m_strokepainter.lowestY - 50;
    int highestX = (m_strokepainter.highestX + 50 > 1024) ? 1024 : m_strokepainter.highestX + 50;
    int highestY = (m_strokepainter.highestY + 50 > 1024) ? 1024 : m_strokepainter.highestY + 50;

    return QRect(QPoint(lowestX, lowestY), QPoint(highestX, highestY));
}
