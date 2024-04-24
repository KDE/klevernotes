// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "editorHighlighter.h"
#include "../editorHandler.h"

#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QTextDocument>
#include <QTimer>
#include <qlogging.h>
#include <qsyntaxhighlighter.h>
#include <tuple>

/**
 * Markdown syntax highlighting
 * @param parent
 * @return
 */
MarkdownHighlighter::MarkdownHighlighter(QTextDocument *document, EditorHandler *editorHandler)
    : QSyntaxHighlighter(document)
    , m_editorHandler(editorHandler)
{
    // _timer = new QTimer(this);
    // connect(_timer, &QTimer::timeout, this, &MarkdownHighlighter::timerTick);
    //
    // _timer->start(1000);
}

void MarkdownHighlighter::nextToken()
{
    if (m_currentTokenPtr == m_highlighterTokens.length()) {
        m_currentTokenStart = -1;
        m_currentTokenEnd = -1;
        m_currentTokenName = QLatin1String();
        return;
    }
    const auto currentToken = m_highlighterTokens.at(m_currentTokenPtr);
    m_currentTokenName = get<0>(currentToken);
    m_currentTokenStart = get<1>(currentToken);
    m_currentTokenEnd = get<2>(currentToken);

    m_currentTokenPtr++;
}

void MarkdownHighlighter::addHighlightToken(const std::tuple<QString, int, int> &token)
{
    const QString name = get<0>(token);
    const int start = get<1>(token);
    const int end = get<2>(token);
    if (name.startsWith(QStringLiteral("lfj"))) {
        qDebug() << name << start << end;
    }

    m_highlighterTokens.append(token);
}

void MarkdownHighlighter::reset()
{
    m_highlighterTokens.clear();
    qDebug() << "\n";
    m_currentTokenStart = 0;
    m_currentTokenEnd = 0;
    m_currentTokenName = QLatin1String();

    m_currentTokenPtr = 0;
}

/**
 * Does jobs every second
 */
void MarkdownHighlighter::timerTick()
{
    // emit a signal every second if there was some highlighting done
    if (_highlightingFinished) {
        _highlightingFinished = false;
    }
}

void MarkdownHighlighter::setNewContent(const QString &content)
{
    m_skipFirst = true;
    document()->setPlainText(content);
}

// /**
//  * Does the Markdown highlighting
//  *
//  * @param text
//  */
void MarkdownHighlighter::highlightBlock(const QString &text)
{
    if (m_skipFirst) {
        m_skipFirst = false;
        return;
    }
    const QTextBlock block = currentBlock();
    const int blockStart = block.position();
    // The first block is always an empty block of length 0
    // it doesn't matter if we run the parser at this time resulting in it
    // not being highlighted, it is actually pretty convenient
    if (pathChanged && blockStart == 0) {
        m_editorHandler->lex();
        pathChanged = false;
        // New doc, we want the cursor at the end
        const auto last = document()->lastBlock();
        m_editorHandler->setCursorPosition(last.position());
        return;
    }
    const int blockEnd = blockStart + block.length();
    const int cursorPos = m_editorHandler->cursorPosition();
    if (blockStart <= cursorPos && cursorPos < blockEnd) {
        m_editorHandler->parse();
    }
    // qDebug() << m_highlighterTokens.length();
}
