/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QHash>
#include <QObject>

class PUMLParserUtils
{
public:
    void clearInfo();

    void pumlDarkChanged();

    QPair<QString, QString> renderCode(const QString &_text, const bool pumlDark);

private:
    bool m_pumlDarkChanged = true;

    QHash<QString, QPair<QString, QString>> m_previousPUMLBlocks;
    QHash<QString, QPair<QString, QString>> m_currentPUMLBlocks;
};
