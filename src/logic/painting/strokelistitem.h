// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

#ifndef STROKELISTITEM_H
#define STROKELISTITEM_H

#include "strokepainter.h"
#include <QImage>
#include <QQuickPaintedItem>

#include "sketchmodel.h"
#include "stroke.h"

class StrokeListItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(Stroke::Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(SketchModel *model READ model WRITE setModel NOTIFY modelChanged)
public:
    explicit StrokeListItem(QQuickItem *parent = nullptr);

    Q_INVOKABLE void addStroke(const Stroke &stroke);
    Q_INVOKABLE void eraseArea(const QVector2D &center, float radius);

    void paint(QPainter *painter) override;

    Stroke::Type type() const;
    SketchModel *model() const;

public Q_SLOTS:
    void setType(Stroke::Type type);
    void setModel(SketchModel *model);

Q_SIGNALS:
    void typeChanged(Stroke::Type type);
    void modelChanged(SketchModel *model);

private:
    Stroke::Type m_type = Stroke::Type::Outline;
    SketchModel *m_model = nullptr;

    StrokePainter m_strokepainter;
};

#endif // STROKELISTITEM_H
