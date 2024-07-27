// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// KleverNotes include
#include "editorHandler.hpp"
#include "logic/parser/parser.h"
#include "logic/parser/plugins/pluginHelper.h"

// Qt include
#include <QColor>
#include <QRegularExpression>
#include <QTextBlock>

using namespace Qt::Literals::StringLiterals;
namespace MdEditor
{

EditorHandler::EditorHandler(QObject *parent)
    : QObject(parent)
{
    m_renderer = new Renderer();

    m_parser = new Parser();
}

// QTextDocument Info
// ==================
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
        connect(m_qQuickDocument->textDocument(), &QTextDocument::contentsChanged, this, &EditorHandler::parseDoc);
        m_document = m_qQuickDocument->textDocument();
        Q_EMIT documentChanged();
        parseDoc();
    }
}

QTextCursor EditorHandler::textCursor() const
{
    if (!m_document) {
        return QTextCursor();
    }

    QTextCursor cursor = QTextCursor(m_document);
    cursor.setPosition(m_cursorPosition);
    return cursor;
}

int EditorHandler::cursorPosition() const
{
    return m_cursorPosition;
}

void EditorHandler::setCursorPosition(const int cursorPosition)
{
    if (cursorPosition == m_cursorPosition) {
        return;
    }

    m_cursorPosition = cursorPosition;
}
// !QTextDocument Info

// KleverNotes method
// ==================
// Parser
void EditorHandler::parseDoc()
{
    if (m_notePath != QStringLiteral("qrc:")) {
        parse(m_document->toPlainText());
    }
}

void EditorHandler::parse(const QString &src)
{
    static const QString qrcStr = QStringLiteral("qrc:");
    if (m_notePath != qrcStr && !m_notePath.endsWith(QStringLiteral(".md"))) {
        return;
    }

    m_currentMdDoc = m_parser->parse(src);

    renderDoc();
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
    static const QString pathEnd = QStringLiteral("/note.md");
    static const QString qrcStr = QStringLiteral("qrc:");
    if (notePath == qrcStr) {
        m_previousPath = m_notePath;
        m_notePath = notePath;
    } else if (!notePath.endsWith(pathEnd)) {
        m_notePath = QLatin1String();
        return;
    } else {
        m_previousPath = notePath;
    }
    m_notePath = notePath;
    m_parser->setNotePath(notePath);

    QString rendererNotePath = notePath;
    if (notePath != qrcStr) {
        rendererNotePath.chop(pathEnd.length());
    }

    m_renderer->setNotePath(rendererNotePath);
}

void EditorHandler::usePreviousPath()
{
    setNotePath(m_previousPath);
    parseDoc();
}
// !Parser

// Rendering
void EditorHandler::renderDoc()
{
    if (m_currentMdDoc) {
        const auto html = m_renderer->toHtml(m_currentMdDoc, m_notePath);

        Q_EMIT parsingFinished(html);
    }
}
// !Rendering
// !KleverNotes method

// md-editor method
// ================
std::shared_ptr<MD::Document<MD::QStringTrait>> EditorHandler::currentDoc() const
{
    return m_currentMdDoc;
}
// !md-editor method
}
