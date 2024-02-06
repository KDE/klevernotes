/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/
#pragma once

#include <QJsonObject>
#include <QObject>
#include <QRegularExpression>

class NoteMapperUtils : public QObject
{
    Q_OBJECT
public:
    explicit NoteMapperUtils(QObject *parent = nullptr);

    enum SpecialValues {
        InvalidHeaderLevel = -1,
    };

    static QString cleanHeader(const QString &header);
    static int headerLevel(const QString &header);
    static QString headerText(const QString &_header);
    static QVariantMap convertSavedMap(const QJsonObject &savedMap);
    static bool entirelyChecked(const QVariantMap &pathInfo);
    static QStringList getNoteHeaders(const QVariantMap &pathInfo);
    static QList<QVariantMap> getHeadersComboList(const QStringList &headers);
};
