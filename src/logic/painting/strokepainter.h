// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

/*
 * Adapted by Louis Schul <schul9louis@gmail.com>
 * in 2023 for Klevernotes
 */

#pragma once

#include <QObject>

class QPainter;
class Stroke;

class StrokePainter : public QObject
{
    Q_OBJECT

public:
    explicit StrokePainter(QObject *parent = nullptr);

    void render(const Stroke &stroke, QPainter *painter);

    int lowestX;
    int highestX;
    int lowestY;
    int highestY;
};
