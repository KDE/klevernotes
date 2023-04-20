// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

#include "sketchmodel.h"

void SketchModel::addStroke(const Stroke &stroke)
{
    strokesRef(stroke.type()).append(stroke);
}

QVector<Stroke> SketchModel::strokes(Stroke::Type type) const
{
    return strokesRef(type);
}

QVector<Stroke> SketchModel::strokes() const
{
    return m_fillStrokes + m_outlineStrokes;
}

void SketchModel::eraseArea(Stroke::Type type, const QVector2D &center, float radius)
{
    auto &strokes = strokesRef(type);

    auto it = std::begin(strokes);
    auto end = std::end(strokes);

    while (it != end) {
        const auto newStrokes = it->eraseArea(center, radius);

        if (newStrokes.size() == 0) {
            // Remove
            it = strokes.erase(it);
            end = std::end(strokes);

        } else if (newStrokes.size() == 1) {
            const auto newStroke = newStrokes.first();
            if (newStroke.samples() != it->samples()) {
                // Change
                *it = newStroke;
            }
            it++;

        } else {
            // Change + Inserts
            *it = newStrokes.first();

            auto newIt = newStrokes.cbegin();
            const auto newEnd = newStrokes.cend();

            newIt++; // Already in the model due to dataChanged above

            it++;

            while (newIt != newEnd) {
                it = strokes.insert(it, *newIt);
                end = std::end(strokes);

                newIt++;
                it++;
            }
        }
    }
}

const QVector<Stroke> &SketchModel::strokesRef(Stroke::Type type) const
{
    switch (type) {
    case Stroke::Type::Fill:
        return m_fillStrokes;
    case Stroke::Type::Outline:
        return m_outlineStrokes;
    }
    Q_UNREACHABLE();
}

QVector<Stroke> &SketchModel::strokesRef(Stroke::Type type)
{
    switch (type) {
    case Stroke::Type::Fill:
        return m_fillStrokes;
    case Stroke::Type::Outline:
        return m_outlineStrokes;
    }
    Q_UNREACHABLE();
}
