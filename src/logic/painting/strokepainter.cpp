// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

#include "strokepainter.h"

#include <QPainter>
#include <QPainterPath>
#include <QVector2D>

#include <algorithm>
#include <cmath>
using namespace std;

#include "stroke.h"

StrokePainter::StrokePainter(QObject *parent)
    : QObject(parent)
{
}

void StrokePainter::render(const Stroke &stroke, QPainter *painter)
{
    const auto samples = stroke.samples();
    if (samples.size() < 2)
        return;

    auto it = samples.cbegin();
    auto lastSample = *it;
    ++it;

    auto forwardPoints = QVector<QVector2D>{};
    auto backwardPoints = QVector<QVector2D>{};

    while (it != samples.cend()) {
        const auto currentSample = *it;

        const auto penWidth = currentSample.width;

        const auto currentPos = currentSample.position;
        const auto lastPos = lastSample.position;

        const auto direction = currentPos - lastPos;
        const auto ortho = QVector2D(direction.y(), -direction.x()).normalized();

        const auto p1 = (lastPos - penWidth * ortho / 2.0f);
        const auto p2 = (lastPos + penWidth * ortho / 2.0f);
        const auto p3 = (currentPos - penWidth * ortho / 2.0f);
        const auto p4 = (currentPos + penWidth * ortho / 2.0f);

        int minX = (int)min({p1.x(), p2.x(), p3.x(), p4.x()});
        if (!lowestX || minX < lowestX)
            lowestX = minX;

        int maxX = (int)max({p1.x(), p2.x(), p3.x(), p4.x()});
        if (!highestX || maxX > highestX)
            highestX = maxX;

        int minY = (int)min({p1.y(), p2.y(), p3.y(), p4.y()});
        if (!lowestY || minY < lowestY)
            lowestY = minY;

        int maxY = (int)max({p1.y(), p2.y(), p3.y(), p4.y()});
        if (!highestY || maxY > highestY)
            highestY = maxY;

        forwardPoints << p1 << p3;
        backwardPoints << p2 << p4;

        lastSample = currentSample;
        ++it;
    }

    auto path = QPainterPath();
    path.moveTo(static_cast<qreal>(forwardPoints[0].x()), static_cast<qreal>(forwardPoints[0].y()));
    for (auto it = forwardPoints.cbegin() + 1; it != forwardPoints.cend(); ++it) {
        path.lineTo(static_cast<qreal>(it->x()), static_cast<qreal>(it->y()));
    }
    for (auto it = backwardPoints.crbegin(); it != backwardPoints.crend(); ++it) {
        path.lineTo(static_cast<qreal>(it->x()), static_cast<qreal>(it->y()));
    }
    path.closeSubpath();
    path.setFillRule(Qt::WindingFill);

    painter->setPen(Qt::NoPen);
    painter->setBrush(stroke.color());
    painter->drawPath(path);
}
