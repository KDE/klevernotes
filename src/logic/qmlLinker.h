// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QObject>

class QmlLinker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER changedText NOTIFY textChanged FINAL)
    // Q_PROPERTY(QVariantMap css MEMBER changedCss NOTIFY cssChanged FINAL)
    // Q_PROPERTY(QString homePath MEMBER changedHomePath NOTIFY homePathChanged FINAL)
    // Q_PROPERTY(QString notePath MEMBER changedNotePath NOTIFY notePathChanged FINAL)

public:
    explicit QmlLinker(QObject *parent = nullptr);

Q_SIGNALS:
    void textChanged(const QString &text);
    // void cssChanged(const QVariantMap &css);
    // void homePathChanged(const QString &homePath);
    // void notePathChanged(const QString &notePath);

private:
    QString changedText;
    // QVariantMap changedCss;
    // QString changedHomePath;
    // QString changedNotePath;
};
