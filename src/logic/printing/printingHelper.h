// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QObject>

class PrintingUtility : public QObject
{
    Q_OBJECT

public:
    explicit PrintingUtility(QObject *parent = nullptr);

    Q_INVOKABLE void writePdf(const QString &path) const;
    Q_INVOKABLE void copy(const QString &fromPath, const QString &toPath);

Q_SIGNALS:
    void pdfCopyDone(const bool succes, const QString &message);
};
