// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

#ifndef SKETCHMODEL_H
#define SKETCHMODEL_H

#include <QHash>
#include <QObject>

#include "stroke.h"

class SketchModel : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    void addStroke(const Stroke &stroke);
    QVector<Stroke> strokes(Stroke::Type type) const;
    QVector<Stroke> strokes() const;

    void eraseArea(Stroke::Type type, const QVector2D &center, float radius);

private:
    const QVector<Stroke> &strokesRef(Stroke::Type type) const;
    QVector<Stroke> &strokesRef(Stroke::Type type);

    QVector<Stroke> m_fillStrokes;
    QVector<Stroke> m_outlineStrokes;
};

#endif // SKETCHMODEL_H
