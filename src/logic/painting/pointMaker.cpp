// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

/*
 * Adapted by Louis Schul <schul9louis@gmail.com>
 * in 2023 for Klevernotes
 */

#include "pointMaker.h"

#include <QTabletEvent>

Event TabletEvent::create(QTabletEvent *event)
{
    Event point;
    point.x = static_cast<float>(event->posF().x());
    point.y = static_cast<float>(event->posF().y());
    point.pressure = static_cast<float>(event->pressure());

    return point;
}

Event MouseEvent::create(const float x, const float y)
{
    Event point;
    point.x = x;
    point.y = y;
    point.pressure = static_cast<float>(0.5f);

    return point;
}
