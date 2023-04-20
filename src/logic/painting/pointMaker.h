// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

/*
 * Adapted by Louis Schul <schul9louis@gmail.com>
 * in 2023 for Klevernotes
 */

#pragma once

#include "stroke.h"

class QTabletEvent;

class Event
{
    Q_GADGET
    Q_PROPERTY(float x MEMBER x)
    Q_PROPERTY(float y MEMBER y)
    Q_PROPERTY(float pressure MEMBER pressure)
public:
    float x = 0.0f;
    float y = 0.0f;
    float pressure = 0.0f;
};

class TabletEvent : Event
{
public:
    static Event create(QTabletEvent *event);
};

class MouseEvent : Event
{
public:
    static Event create(const float x, const float y);
};
