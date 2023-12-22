// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

#include "pressureequation.h"

#include <QDebug>
#include <cmath>

namespace
{
float sigmoid(float x)
{
    const auto lambda = 5.0f;
    return 1.0f / (1.0f + std::exp(-lambda * x));
}

float computeWidth(float minWidth, float maxWidth, float pressure)
{
    const auto alpha = sigmoid(2.0f * pressure - 1.0f);
    return (1.0f - alpha) * minWidth + alpha * maxWidth;
}
}

float PressureEquation::minWidth() const
{
    return m_minWidth;
}

float PressureEquation::maxWidth() const
{
    return m_maxWidth;
}

float PressureEquation::pressure() const
{
    return m_pressure;
}

float PressureEquation::width() const
{
    return m_width;
}

void PressureEquation::setMinWidth(float minWidth)
{
    if (qFuzzyCompare(m_minWidth, minWidth))
        return;

    m_minWidth = minWidth;
    Q_EMIT minWidthChanged(minWidth);
    updateWidth();
}

void PressureEquation::setMaxWidth(float maxWidth)
{
    if (qFuzzyCompare(m_maxWidth, maxWidth))
        return;

    m_maxWidth = maxWidth;
    Q_EMIT maxWidthChanged(maxWidth);
    updateWidth();
}

void PressureEquation::setPressure(float pressure)
{
    if (qFuzzyCompare(m_pressure, pressure))
        return;

    m_pressure = pressure;
    Q_EMIT pressureChanged(pressure);
    updateWidth();
}

void PressureEquation::updateWidth()
{
    m_width = computeWidth(m_minWidth, m_maxWidth, m_pressure);
    Q_EMIT widthChanged(m_width);
}
