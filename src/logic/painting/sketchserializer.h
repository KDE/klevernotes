// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

/*
 * Adapted by Louis Schul <schul9louis@gmail.com>
 * in 2023 for Klevernotes
 */

#ifndef SKETCHSERIALIZER_H
#define SKETCHSERIALIZER_H

#include <QObject>
#include <QRect>

#include "strokepainter.h"

class SketchModel;

class SketchSerializer : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    Q_INVOKABLE void serialize(SketchModel *model, const QSize &size, const QUrl &fileUrl);

    Q_INVOKABLE QRect getCropRect();

private:
    StrokePainter m_strokepainter;
};

#endif // SKETCHSERIALIZER_H
