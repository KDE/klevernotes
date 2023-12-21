// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

/*
 * Adapted by Louis Schul <schul9louis@gmail.com>
 * in 2023 for Klevernotes
 */

#include "sketchview.h"

#include <QPointingDevice>

Q_GLOBAL_STATIC(SketchView *, s_instance);

SketchViewHandler::SketchViewHandler(QObject *parent)
    : QObject(parent)
{
    connect(SketchView::instance(), &SketchView::tabletEventReceived, this, &SketchViewHandler::onTabletEventReceived);
}

Stroke SketchViewHandler::createStroke(Stroke::Type type, const QColor &color) const
{
    auto result = Stroke{};
    result.setType(type);
    result.setColor(color);
    return result;
}

StrokeSample SketchViewHandler::createSample(const QVector2D &position, float width) const
{
    StrokeSample sample;
    sample.position = position;
    sample.width = width;
    return sample;
}

Event SketchViewHandler::point() const
{
    return m_point;
}

bool SketchViewHandler::isPressed() const
{
    return m_pressed;
}

void SketchViewHandler::onTabletEventReceived(QTabletEvent *event)
{
    m_point = TabletEvent::create(event);
    emit pointChanged(m_point);

    int pointerIndex = (event->pointerType() == QPointingDevice::PointerType::Eraser) ? 1 : 0;
    changePointer(pointerIndex);

    if (event->type() == QEvent::TabletPress && !m_pressed) {
        m_pressed = true;
        emit pressedChanged(m_pressed);
    } else if (event->type() == QEvent::TabletRelease && m_pressed) {
        m_pressed = false;
        emit pressedChanged(m_pressed);
    }
}

void SketchViewHandler::mouseMoved(const float x, const float y)
{
    m_point = MouseEvent::create(x, y);
    emit pointChanged(m_point);
    return;
}

void SketchViewHandler::changeMousePress(bool pressed)
{
    m_pressed = pressed;
    emit pressedChanged(pressed);
}

void SketchViewHandler::changePointer(const int pointerIndex)
{
    m_pointerType = Pointer(pointerIndex);
    emit isEraserChanged(m_pointerType == Pointer::Eraser);
}

SketchView *SketchView::instance()
{
    return *s_instance;
}

SketchView::SketchView(QWindow *parent)
    : QQuickView(parent)
{
    if (*s_instance)
        qFatal("There must be only one SketchView instance");

    *s_instance = this;
}

void SketchView::tabletEvent(QTabletEvent *event)
{
    event->accept();

    if (m_lastType == event->type()) {
        const QPointF eventPos = event->globalPosition();
        const auto length = (eventPos - m_lastGlobalPos).manhattanLength();
        constexpr auto lengthThreshold = 4.0;

        if (length < lengthThreshold)
            return;
    }

    m_lastType = event->type();
    m_lastGlobalPos = event->globalPosition();
    emit tabletEventReceived(event);
}
