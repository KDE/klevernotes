// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

#ifndef STROKE_H
#define STROKE_H

#include <QColor>
#include <QObject>
#include <QRectF>
#include <QVector2D>
#include <QVector>

class StrokeSample
{
    Q_GADGET
    Q_PROPERTY(QVector2D position MEMBER position)
    Q_PROPERTY(float width MEMBER width)
public:
    QVector2D position;
    float width = 1.0f;

    bool operator==(const StrokeSample &other) const;
};

class Stroke
{
    Q_GADGET
    Q_PROPERTY(Stroke::Type type READ type WRITE setType)
    Q_PROPERTY(QColor color READ color WRITE setColor)
public:
    enum class Type { Outline, Fill };
    Q_ENUM(Type)

    Type type() const;
    void setType(Type type);

    QColor color() const;
    void setColor(const QColor &color);

    QVector<StrokeSample> samples() const;
    Q_INVOKABLE void addSample(const StrokeSample &sample);
    void addSamples(const QVector<StrokeSample> &samples);

    QVector<Stroke> eraseArea(const QVector2D &center, float radius);

    Q_INVOKABLE QRectF boundingRect() const;

private:
    Type m_type = Type::Outline;
    QColor m_color = Qt::black;
    QVector<StrokeSample> m_samples;
    mutable QRectF m_boundingRectCache;
};

#endif // STROKE_H
