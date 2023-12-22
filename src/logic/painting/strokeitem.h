// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

#ifndef STROKEITEM_H
#define STROKEITEM_H

#include "strokepainter.h"
#include <QQuickPaintedItem>

#include "stroke.h"

class StrokeItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Stroke stroke READ stroke WRITE setStroke NOTIFY strokeChanged)
public:
    explicit StrokeItem(QQuickItem *parent = nullptr);

    Stroke stroke() const;
    void setStroke(const Stroke &stroke);

    Q_INVOKABLE void addSample(const StrokeSample &sample);

    void paint(QPainter *painter) override;

Q_SIGNALS:
    void strokeChanged(const Stroke &stroke);

private:
    Stroke m_stroke;
    StrokePainter m_strokepainter;
};

#endif // STROKEITEM_H
