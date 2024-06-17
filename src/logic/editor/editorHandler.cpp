// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// KleverNotes include
#include "editorHandler.hpp"
#include "logic/parser/parser.h"
#include "syntaxvisitor.hpp"

// Qt include
#include <QRegularExpression>
#include <QTextBlock>
#include <qlogging.h>

namespace MdEditor
{

EditorHandler::EditorHandler(QObject *parent)
    : QObject(parent)
{
    m_parser = new Parser(this);
    m_syntaxvisitor = new SyntaxVisitor(this);
}

// Acces QTextDocument
// ===================

QTextDocument *EditorHandler::document() const
{
    return m_document;
}

QQuickTextDocument *EditorHandler::qQuickDocument() const
{
    return m_qQuickDocument;
}

void EditorHandler::setDocument(QQuickTextDocument *document)
{
    if (document == m_qQuickDocument)
        return;

    if (m_qQuickDocument) {
        m_qQuickDocument->textDocument()->disconnect(this);
    }

    if (document) {
        m_qQuickDocument = document;
        connect(m_qQuickDocument->textDocument(), &QTextDocument::contentsChanged, this, &EditorHandler::parse);
        m_document = m_qQuickDocument->textDocument();
        Q_EMIT documentChanged();
        parse();
    }
}
// ===============

// KleverNotes method
// ==================
void EditorHandler::parse()
{
    const QString content = m_parser->parse(m_document->toPlainText());
    Q_EMIT parsingFinished(content);
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
    static const QString pathEnd = QStringLiteral("note.md");
    parserNotePath.chop(pathEnd.length());
    m_parser->setNotePath(parserNotePath);
}
// ===============

// md-editor method
// ================
std::shared_ptr<MD::Document<MD::QStringTrait>> EditorHandler::currentDoc() const
{
    return m_currentMdDoc;
}

void EditorHandler::applyFont(const QFont &f)
{
    /* setFont( f ); */

    /* d->syntax.setFont( f ); */

    highlightSyntax(colors, m_currentMdDoc);
}

SyntaxVisitor *EditorHandler::syntaxHighlighter() const
{
    return m_syntaxvisitor;
}
// ===============

// KleverNotes slots
// =================
void EditorHandler::newHighlightStyle()
{
    m_parser->newHighlightStyle();
}

void EditorHandler::pumlDarkChanged()
{
    m_parser->pumlDarkChanged();
}

// md-editor slots
// ===============

void EditorHandler::highlightSyntax(const Colors &colors, std::shared_ptr<MD::Document<MD::QStringTrait>> doc)
{
    m_syntaxvisitor->highlight(doc, colors);
}
// ===============
}
