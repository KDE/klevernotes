/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "highlightParserUtils.h"
#include "kleverconfig.h"
#include "logic/plugins/syntaxHighlight/highlightHelper.h"

void HighlightParserUtils::clearInfo()
{
    m_previousHighlightedBlocks = m_currentHighlightedBlocks;
    m_currentHighlightedBlocks.clear();
}

// void HighlightParserUtils::clearPreviousInfo()
// {
//     m_previousNoteCodeBlocks.clear();
// }
//
// void HighlightParserUtils::addToNoteCodeBlocks(const QString &codeBlock)
// {
//     m_noteCodeBlocks.append(codeBlock);
// }

void HighlightParserUtils::newHighlightStyle()
{
    // m_newHighlightStyle = true;
}

// void HighlightParserUtils::preTok()
// {
//     m_sameCodeBlocks = m_previousNoteCodeBlocks == m_noteCodeBlocks && !m_noteCodeBlocks.isEmpty();
//     if (!m_sameCodeBlocks || m_newHighlightStyle) {
//         m_previousNoteCodeBlocks = m_noteCodeBlocks;
//         m_previousHighlightedBlocks.clear();
//         m_newHighlightStyle = false;
//         m_sameCodeBlocks = false;
//     }
//     m_currentBlockIndex = 0;
// }

QString escape(QString &html, bool encode)
{
    static const QRegularExpression replace1 = QRegularExpression(QStringLiteral("&(?!#?\\w+;)"));
    static const QRegularExpression replace2 = QRegularExpression(QStringLiteral("&"));

    const QRegularExpression encodedReplacement = !encode ? replace1 : replace2;

    static const QRegularExpression leftBracketReg = QRegularExpression(QStringLiteral("<"));
    static const QRegularExpression rightBracketReg = QRegularExpression(QStringLiteral(">"));
    static const QRegularExpression quoteReg = QRegularExpression(QStringLiteral("\""));
    static const QRegularExpression apostropheReg = QRegularExpression(QStringLiteral("'"));
    return html.replace(encodedReplacement, QStringLiteral("&amp;"))
        .replace(leftBracketReg, QStringLiteral("&lt;"))
        .replace(rightBracketReg, QStringLiteral("&gt;"))
        .replace(quoteReg, QStringLiteral("&quot;"))
        .replace(apostropheReg, QStringLiteral("&#39;"));
}

QString HighlightParserUtils::renderCode(const QString &_text, const QString &lang)
{
    if (!KleverConfig::codeSynthaxHighlightEnabled() && !lang.isEmpty()) {
        QString returnValue = _text;
        return escape(returnValue, true);
    }

    QString code;
    if (m_previousHighlightedBlocks.contains(_text)) {
        code = m_previousHighlightedBlocks.value(_text);
    } else {
        code = HighlightHelper::getHighlightedString(_text, lang);
    }
    m_currentHighlightedBlocks.insert(_text, code);
    return code;
}
