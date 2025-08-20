// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QObject>
#include <QQmlEngine>

class QmlLinker : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString text MEMBER changedText NOTIFY textChanged FINAL)

public:
    explicit QmlLinker(QObject *parent = nullptr);

Q_SIGNALS:
    void textChanged(const QString &text);

private:
    QString changedText;
};
