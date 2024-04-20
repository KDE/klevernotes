// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// PARTIALLY BASED ON: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quickcontrols2/texteditor?h=6.2.0

#include "editorHandler.h"
#include "parser/parser.h"
#include <QRegularExpression>
#include <QTextBlock>

EditorHandler::EditorHandler(QObject *parent)
    : QObject(parent)
{
    m_parser = new Parser(this);
}

// Acces QTextDocument
// ===================

QQuickTextDocument *EditorHandler::document() const
{
    return m_document;
}

void EditorHandler::setDocument(QQuickTextDocument *document)
{
    if (document == m_document)
        return;

    if (m_document)
        m_document->textDocument()->disconnect(this);
    m_document = document;
    if (m_document)
        connect(m_document->textDocument(), &QTextDocument::modificationChanged, this, &EditorHandler::modifiedChanged);

    m_textDocument = m_document->textDocument();
    Q_EMIT documentChanged();

    if (!m_markdownHighlighter) {
        m_markdownHighlighter = new MarkdownHighlighter(m_textDocument);
    } else {
        m_markdownHighlighter->setTextDocument(document);
    }
}

// General Info
// ============

int EditorHandler::cursorPosition() const
{
    return m_cursorPosition;
}

void EditorHandler::setCursorPosition(const int cursorPosition)
{
    if (cursorPosition == m_cursorPosition)
        return;

    m_cursorPosition = cursorPosition;
}

int EditorHandler::selectionStart() const
{
    return m_selectionStart;
}

void EditorHandler::setSelectionStart(const int selectionStart)
{
    if (selectionStart == m_selectionStart)
        return;

    m_selectionStart = selectionStart;
    Q_EMIT selectionStartChanged();
}

int EditorHandler::selectionEnd() const
{
    return m_selectionEnd;
}

void EditorHandler::setSelectionEnd(const int selectionEnd)
{
    if (selectionEnd == m_selectionEnd)
        return;

    m_selectionEnd = selectionEnd;
    Q_EMIT selectionEndChanged();
}

// PARSER
QString EditorHandler::parserGetNotePath() const
{
    return m_parser->getNotePath();
}

void EditorHandler::parserSetNotePath(const QString &notePath)
{
    m_parser->setNotePath(notePath);
}

QString EditorHandler::parse(QString src)
{
    return m_parser->parse(src);
}

void EditorHandler::newHighlightStyle()
{
    m_parser->newHighlightStyle();
}

void EditorHandler::pumlDarkChanged()
{
    m_parser->pumlDarkChanged();
}
