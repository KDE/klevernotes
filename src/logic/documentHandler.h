// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QObject>
#include <QUrl>

class DocumentHandler : public QObject
{
    Q_OBJECT

public:
    explicit DocumentHandler(QObject *parent = nullptr);

public slots:
    QString readNote(QString path);
    void writeNote(QString note, QString path);
};
