/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QHash>

class HighlightParserUtils
{
public:
    void clearInfo();

    QString getCode(const bool highlight, const QString &_text, const QString &lang);

    void newHighlightStyle();

private:
    bool m_newHighlightStyle = true;

    QHash<QString, QString> m_previousHighlightedBlocks;
    QHash<QString, QString> m_currentHighlightedBlocks;
};
