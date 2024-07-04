/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "highlightParserUtils.h"
#include "highlightHelper.h"

void HighlightParserUtils::clearInfo()
{
    m_previousHighlightedBlocks = m_currentHighlightedBlocks;
    m_currentHighlightedBlocks.clear();
}

void HighlightParserUtils::newHighlightStyle()
{
    m_newHighlightStyle = true;
}

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

QString HighlightParserUtils::getCode(const bool highlight, const QString &_text, const QString &lang)
{
    if (m_newHighlightStyle) {
        m_previousHighlightedBlocks.clear();
        m_newHighlightStyle = false;
    }
    QString code;
    if (!highlight && !lang.isEmpty()) {
        QString text = _text;
        code = escape(text, true);
    } else {
        if (m_previousHighlightedBlocks.contains(_text)) {
            code = m_previousHighlightedBlocks.value(_text);
        } else {
            code = HighlightHelper::getHighlightedString(_text, lang);
        }
        m_currentHighlightedBlocks.insert(_text, code);
    }

    return code;
}
