/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "highlightParserUtils.h"

#include "logic/parser/renderer.h"

void HighlightParserUtils::clearInfo()
{
    m_noteCodeBlocks.clear();
}

void HighlightParserUtils::clearPreviousInfo()
{
    m_previousNoteCodeBlocks.clear();
}

void HighlightParserUtils::addToNoteCodeBlocks(const QString &codeBlock)
{
    m_noteCodeBlocks.append(codeBlock);
}

void HighlightParserUtils::newHighlightStyle()
{
    m_newHighlightStyle = true;
}

void HighlightParserUtils::preTok()
{
    m_sameCodeBlocks = m_previousNoteCodeBlocks == m_noteCodeBlocks && !m_noteCodeBlocks.isEmpty();
    if (!m_sameCodeBlocks || m_newHighlightStyle) {
        m_previousNoteCodeBlocks = m_noteCodeBlocks;
        m_previousHighlightedBlocks.clear();
        m_newHighlightStyle = false;
        m_sameCodeBlocks = false;
    }
    m_currentBlockIndex = 0;
}

QString HighlightParserUtils::renderCode(const bool highlight, const QString &_text, const QString &lang)
{
    QString returnValue;
    if (m_sameCodeBlocks && highlight) { // Only the highlighted values are stored in here
        returnValue = m_previousHighlightedBlocks[m_currentBlockIndex];
        m_currentBlockIndex++;
    } else {
        QString text = _text;
        returnValue = Renderer::code(text, lang, highlight);
        if (highlight) { // We want to store only the highlighted values
            m_previousHighlightedBlocks.append(returnValue);
        }
    }
    return returnValue;
}
