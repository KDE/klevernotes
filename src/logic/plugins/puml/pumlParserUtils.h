/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QObject>
#include <QSet>

class PUMLParserUtils
{
public:
    void clearInfo();
    void clearPreviousInfo();
    void preTok();

    void addToNotePUMLBlock(const QString &pumlBlock);
    void pumlDarkChanged();

    QString renderCode(const QString &_text, const bool pumlDark);

private:
    bool m_pumlDarkChanged = true;
    bool m_samePUMLBlocks = false;
    int m_currentPUMLBlockIndex = 0;
    QStringList m_notePUMLBlocks;
    QStringList m_previousNotePUMLBlocks;
    QStringList m_previousPUMLDiag;
};
