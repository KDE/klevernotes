// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

#include "strokeitem.h"

#include <QPainter>

StrokeItem::StrokeItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
{
    setAntialiasing(true);
    setRenderTarget(FramebufferObject);
}

Stroke StrokeItem::stroke() const
{
    return m_stroke;
}

void StrokeItem::setStroke(const Stroke &stroke)
{
    m_stroke = stroke;
    emit strokeChanged(stroke);
    update();
}

void StrokeItem::addSample(const StrokeSample &sample)
{
    m_stroke.addSample(sample);
    emit strokeChanged(m_stroke);
    update();
}

void StrokeItem::paint(QPainter *painter)
{
    m_strokepainter.render(m_stroke, painter);
}
