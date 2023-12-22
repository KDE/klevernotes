// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2018 Kevin Ottens <ervin@kde.org>

#ifndef PRESSUREEQUATION_H
#define PRESSUREEQUATION_H

#include <QObject>

class PressureEquation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(float minWidth READ minWidth WRITE setMinWidth NOTIFY minWidthChanged)
    Q_PROPERTY(float maxWidth READ maxWidth WRITE setMaxWidth NOTIFY maxWidthChanged)

    Q_PROPERTY(float pressure READ pressure WRITE setPressure NOTIFY pressureChanged)
    Q_PROPERTY(float width READ width NOTIFY widthChanged)
public:
    using QObject::QObject;

    float minWidth() const;
    float maxWidth() const;

    float pressure() const;
    float width() const;

public Q_SLOTS:
    void setMinWidth(float minWidth);
    void setMaxWidth(float maxWidth);

    void setPressure(float pressure);

Q_SIGNALS:
    void minWidthChanged(float minWidth);
    void maxWidthChanged(float maxWidth);

    void pressureChanged(float pressure);
    void widthChanged(float width);

private:
    void updateWidth();

    float m_minWidth = 1.0f;
    float m_maxWidth = 1.0f;

    float m_pressure = 0.0f;
    float m_width = m_maxWidth;
};

#endif // PRESSUREEQUATION_H
