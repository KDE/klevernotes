// SPDX-FileCopyrightText 2023 Louis Schul <schul9louis@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QObject>
#include <QString>
#include <QVariantMap>

class QmlLinker : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER changedText NOTIFY textChanged FINAL)
    Q_PROPERTY(QVariantMap cssVar MEMBER changedCssVar NOTIFY cssVarChanged FINAL)
    Q_PROPERTY(QString cssStyle MEMBER changedCssStyle NOTIFY cssStyleChanged FINAL)
    Q_PROPERTY(QString homePath MEMBER changedHomePath NOTIFY homePathChanged FINAL)
    Q_PROPERTY(QString notePath MEMBER changedNotePath NOTIFY notePathChanged FINAL)

public:
    explicit QmlLinker(QObject *parent = nullptr);

signals:
    void textChanged(const QString &text);
    void cssVarChanged(const QVariantMap &cssVar);
    void cssStyleChanged(const QString &cssStyle);
    void homePathChanged(const QString &homePath);
    void notePathChanged(const QString &notePath);

private:
    QString changedText;
    QVariantMap changedCssVar;
    QString changedCssStyle;
    QString changedHomePath;
    QString changedNotePath;
};
