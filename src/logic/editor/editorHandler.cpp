// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// KleverNotes include
#include "editorHandler.hpp"
#include "kleverconfig.h"
#include "logic/parser/parser.h"
#include "logic/parser/plugins/pluginHelper.h"
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
    m_renderer = new Renderer();
    m_syntaxvisitor = new SyntaxVisitor(this);

    m_parser = new Parser();
    m_pluginHelper = new PluginHelper(this);
    m_renderer->addPluginHelper(m_pluginHelper);

    connectPlugins();

    static const QList<QStringList> extendedSyntaxsList = {
        {QStringLiteral("=="), QStringLiteral("<mark>"), QStringLiteral("</mark>")}, // Highlight
        {QStringLiteral("-"), QStringLiteral("<sub>"), QStringLiteral("</sub>")}, // Subscript
        {QStringLiteral("^"), QStringLiteral("<sup>"), QStringLiteral("</sup>")}, // Superscript
    };
    /* addExtendedSyntaxs(extendedSyntaxsList); */
}

// Connections
// ===========
void EditorHandler::connectPlugins()
{
    // Code Highlight
    connect(KleverConfig::self(), &KleverConfig::codeSynthaxHighlightEnabledChanged, this, &EditorHandler::codeHighlightEnabledChanged);
    codeHighlightEnabledChanged();
    connect(KleverConfig::self(), &KleverConfig::codeSynthaxHighlighterStyleChanged, this, &EditorHandler::newHighlightStyle);
    newHighlightStyle();

    // Puml
    connect(KleverConfig::self(), &KleverConfig::pumlEnabledChanged, this, &EditorHandler::pumlEnabledChanged);
    pumlEnabledChanged();
    connect(KleverConfig::self(), &KleverConfig::pumlDarkChanged, this, &EditorHandler::pumlDarkChanged);
    pumlDarkChanged();
}
// !Connections

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
    if (!m_highlighting) {
        parse(m_document->toPlainText());
    }
}

void EditorHandler::parse(const QString &src)
{
    if (m_pluginHelper) {
        m_pluginHelper->clearPluginsInfo();
    }

    m_currentMdDoc = m_parser->parse(src);
    highlightSyntax(colors, m_currentMdDoc);

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

    QString parserNotePath = notePath;
    static const QString pathEnd = QStringLiteral("/");
    if (notePath.endsWith(pathEnd)) {
        parserNotePath.chop(pathEnd.length());
    }

    m_notePath = parserNotePath;
    m_parser->setNotePath(parserNotePath);
    m_renderer->setNotePath(m_notePath);

    if (m_notePath == QStringLiteral("qrc:") && m_pluginHelper) {
        // We do this here because we're sure to be in another note
        m_pluginHelper->clearPluginsPreviousInfo();

        m_pluginHelper->mapperParserUtils()->setNotePath(notePath);
    }
}
// !Parser

// Rendering
void EditorHandler::renderDoc()
{
    if (m_currentMdDoc) {
        const auto html = m_renderer->toHtml(m_currentMdDoc, m_notePath);

        if (m_pluginHelper) {
            m_pluginHelper->postTokChanges();
        }
        Q_EMIT parsingFinished(html);
    }
}
// !Rendering

// ExtendedSyntax
void EditorHandler::addExtendedSyntax(const QStringList &details)
{
    const long long int opts = MD::TextOption::StrikethroughText << (m_extendedSyntaxCount + 1);
    m_renderer->addExtendedSyntax(opts, details[1], details[2]);

    const QStringList options = {details[0], QString::number(opts), QString::number(ExtensionID::ExtendedSyntax + m_extendedSyntaxCount)};
    m_parser->addExtendedSyntax(options);
    ++m_extendedSyntaxCount;
}

void EditorHandler::addExtendedSyntaxs(const QList<QStringList> &syntaxsDetails)
{
    for (const auto &details : syntaxsDetails) {
        addExtendedSyntax(details);
    }
}
// !ExtendedSyntax

// NoteMapper method
void EditorHandler::setHeaderInfo(const QStringList &headerInfo)
{
    m_pluginHelper->mapperParserUtils()->setHeaderInfo(headerInfo);
}

QString EditorHandler::headerLevel() const
{
    return m_pluginHelper->mapperParserUtils()->headerLevel();
};
// !NoteMapper method

// !KleverNotes method

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
// !md-editor method

// KleverNotes slots
// =================
void EditorHandler::codeHighlightEnabledChanged()
{
    m_renderer->setCodeHighlightEnable(KleverConfig::codeSynthaxHighlightEnabled());
    renderDoc();
}

void EditorHandler::newHighlightStyle()
{
    m_pluginHelper->highlightParserUtils()->newHighlightStyle();
    renderDoc();
}

void EditorHandler::pumlEnabledChanged()
{
    m_renderer->setPUMLenable(KleverConfig::pumlEnabled());
    renderDoc();
}

void EditorHandler::pumlDarkChanged()
{
    m_renderer->setPUMLdark(KleverConfig::pumlDark());
    renderDoc();
}
// !KleverNotes slots

// md-editor slots
// ===============

void EditorHandler::highlightSyntax(const Colors &colors, std::shared_ptr<MD::Document<MD::QStringTrait>> doc)
{
    m_highlighting = true;
    m_syntaxvisitor->highlight(doc, colors);
    m_highlighting = false;
}
// !md-editor slots
}
