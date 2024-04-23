// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// PARTIALLY BASED ON: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quickcontrols2/texteditor?h=6.2.0

#include "editorHandler.h"
#include "logic/documentHandler.h"
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

    if (m_document) {
        m_document->textDocument()->disconnect(this);
    }
    m_document = document;
    if (m_document) {
        m_markdownHighlighter = new MarkdownHighlighter(m_document->textDocument(), this);
        connect(m_document->textDocument(), &QTextDocument::modificationChanged, this, &EditorHandler::modifiedChanged);
    }

    Q_EMIT documentChanged();
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

QString EditorHandler::getNotePath() const
{
    return m_notePath;
}

void EditorHandler::setNotePath(const QString &notePath)
{
    if (notePath == m_notePath) {
        return;
    }
    m_notePath = notePath;

    QString parserNotePath = notePath;
    parserNotePath.replace(QStringLiteral("note.md"), QLatin1String());
    m_parser->setNotePath(parserNotePath);
}

// PARSER
QString EditorHandler::parse()
{
    m_markdownHighlighter->reset();
    QString test = m_document->textDocument()->toPlainText();
    return m_parser->parse(test);
}

void EditorHandler::newHighlightStyle()
{
    m_parser->newHighlightStyle();
}

void EditorHandler::pumlDarkChanged()
{
    m_parser->pumlDarkChanged();
}

// HIGHLIGHTER
void EditorHandler::addHighlightToken(const std::tuple<QString, int, int> &token)
{
    m_markdownHighlighter->addHighlightToken(token);
}

