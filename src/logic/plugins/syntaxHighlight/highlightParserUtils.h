/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QObject>
#include <QSet>

// class Parser;

class HighlightParserUtils
{
public:
    void clearInfo();
    void clearPreviousInfo();
    void preTok();

    void addToNoteCodeBlocks(const QString &codeBlock);
    void newHighlightStyle();

    QString renderCode(const bool highlight, const QString &_text, const QString &lang);

private:
    bool m_newHighlightStyle = true;
    bool m_sameCodeBlocks = false;
    int m_currentBlockIndex = 0;
    QStringList m_noteCodeBlocks;
    QStringList m_previousHighlightedBlocks;
    QStringList m_previousNoteCodeBlocks;
};
