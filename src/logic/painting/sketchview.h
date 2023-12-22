// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

/*
 * Adapted by Louis Schul <schul9louis@gmail.com>
 * in 2023 for Klevernotes
 */

#ifndef SKETCHVIEW_H
#define SKETCHVIEW_H

#include <QQuickView>

#include "pointMaker.h"

class SketchViewHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Event point READ point NOTIFY pointChanged)
    Q_PROPERTY(bool pressed READ isPressed NOTIFY pressedChanged)
public:
    enum class Pointer {
        Pen,
        Eraser,
    };
    Q_ENUM(Pointer)

    explicit SketchViewHandler(QObject *parent = nullptr);

    Q_INVOKABLE Stroke createStroke(Stroke::Type type, const QColor &color) const;
    Q_INVOKABLE StrokeSample createSample(const QVector2D &position, float width) const;

    Event point() const;
    bool isPressed() const;

    Q_INVOKABLE void changePointer(const int pointerIndex);
    Q_INVOKABLE void changeMousePress(bool pressed);
    Q_INVOKABLE void mouseMoved(const float x, const float y);

Q_SIGNALS:
    void pointChanged(const Event &point);
    void pressedChanged(const bool pressed);
    void isEraserChanged(const bool sketchViewIsEraser);

private Q_SLOTS:
    void onTabletEventReceived(QTabletEvent *event);

private:
    Event m_point;
    bool m_active = false;
    bool m_pressed = false;
    Pointer m_pointerType = Pointer::Pen;
};

class SketchView : public QQuickView
{
    Q_OBJECT
public:
    static SketchView *instance();

    explicit SketchView(QWindow *parent = nullptr);

    void tabletEvent(QTabletEvent *event) override;

Q_SIGNALS:
    void tabletEventReceived(QTabletEvent *event);
    void pressedChanged(bool mousePressed);

private:
    QPointF m_lastGlobalPos;
    QEvent::Type m_lastType = QEvent::None;
};

#endif // SKETCHVIEW_H
