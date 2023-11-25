/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/
#pragma once

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

private:
    inline static QRegularExpression m_heading_checker = QRegularExpression("^(#*)( *)(.*)");
    inline static QRegularExpression m_heading_level = QRegularExpression("^(#{1,6}) *");
    inline static QRegularExpression m_heading_text = QRegularExpression("^(#{1,6})( +)(.+)");
};
