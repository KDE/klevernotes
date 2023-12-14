// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

#include "stroke.h"

#include <QVector2D>

bool StrokeSample::operator==(const StrokeSample &other) const
{
    return position == other.position && qFuzzyCompare(width, other.width);
}

Stroke::Type Stroke::type() const
{
    return m_type;
}

void Stroke::setType(Stroke::Type type)
{
    m_type = type;
}

QColor Stroke::color() const
{
    return m_color;
}

void Stroke::setColor(const QColor &color)
{
    m_color = color;
}

QVector<StrokeSample> Stroke::samples() const
{
    return m_samples;
}

void Stroke::addSample(const StrokeSample &sample)
{
    m_samples << sample;
    m_boundingRectCache = QRectF{};
}

void Stroke::addSamples(const QVector<StrokeSample> &samples)
{
    m_samples += samples;
    m_boundingRectCache = QRectF{};
}

QVector<Stroke> Stroke::eraseArea(const QVector2D &center, float radius)
{
    const auto areaBoundingRect = QRectF{(center - QVector2D{radius, radius}).toPointF(), (center + QVector2D{radius, radius}).toPointF()};
    auto result = QVector<Stroke>{};

    // For sure won't get any hits
    if (!boundingRect().intersects(areaBoundingRect)) {
        result.append(*this);
        return result;
    }

    const auto isHit = [=](const StrokeSample &sample) {
        return QVector2D{sample.position - center}.length() <= radius;
    };

    auto it = m_samples.cbegin();
    const auto end = m_samples.cend();

    while (it != end) {
        auto previous = it;
        // Find first hit
        it = std::find_if(it, end, isHit);

        // Copy [previous, it) in a new stroke if there's more than one sample
        const auto distance = static_cast<int>(std::distance(previous, it));
        if (distance >= 2) {
            auto stroke = *this;
            stroke.m_samples.clear();
            stroke.m_samples.reserve(distance);
            std::copy(previous, it, std::back_inserter(stroke.m_samples));

            result.append(stroke);
        }

        // Jump to next non-hit
        it = std::find_if_not(it, end, isHit);
    }

    return result;
}

QRectF Stroke::boundingRect() const
{
    if (!m_boundingRectCache.isValid()) {
        auto minX = 0.0f;
        auto minY = 0.0f;
        auto maxX = 0.0f;
        auto maxY = 0.0f;

        for (const auto &sample : std::as_const(m_samples)) {
            const auto position = sample.position;
            if (position.x() < minX)
                minX = position.x();
            else if (position.x() > maxX)
                maxX = position.x();

            if (position.y() < minY)
                minY = position.y();
            else if (position.y() > maxY)
                maxY = position.y();
        }

        m_boundingRectCache = {QPointF{static_cast<qreal>(minX), static_cast<qreal>(minY)}, QPointF{static_cast<qreal>(maxX), static_cast<qreal>(maxY)}};
    }

    return m_boundingRectCache;
}
