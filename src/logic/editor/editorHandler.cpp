// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "editorHandler.hpp"

// KleverNotes include
#include "editorHighlighter.hpp"
#include "logic/parser/parser.h"
#include "logic/parser/renderer.h"

// Qt include
#include <QColor>
#include <QRegularExpression>
#include <QTextBlock>

using namespace Qt::Literals::StringLiterals;
namespace MdEditor
{

EditorHandler::EditorHandler(QObject *parent)
    : QObject(parent)
    , m_config(KleverConfig::self())
    , m_parser(new Parser())
    , m_renderer(new Renderer())
    , m_pluginHelper(new PluginHelper(this))
    , m_editorHighlighter(new EditorHighlighter(this))
    , m_cursorMoveTimer(new QTimer(this))
{
    m_renderer->addPluginHelper(m_pluginHelper);

    connectPlugins();
    connectHighlight();
    connectTimer();

    static const QList<QStringList> extendedSyntaxsList = {
        // 0. Delim, 1. HTML open, 2. HTML close, 3. size scale,
        // 4. foreground, 5. background, 6. vertical alignment, 7. bold enabled,
        // 8. italic enabled, 9. strikethrough enabled, 10. underline enabled, 11. underline style
        // if empty for bool value => false, if empty for other => default value
        // TODO: make documentation about this
        {u"=="_s, u"<mark>"_s, u"</mark>"_s, u""_s, u"background"_s, u"highlight"_s, u""_s, u""_s, u""_s, u""_s, u""_s, u""_s}, // Highlight
        {u"-"_s, u"<sub>"_s, u"</sub>"_s, u""_s, u""_s, u""_s, u"2"_s, u"y"_s, u"y"_s, u""_s, u""_s, u""_s}, // Subscript
        {u"^"_s, u"<sup>"_s, u"</sup>"_s, u""_s, u""_s, u""_s, u"1"_s, u"y"_s, u"y"_s, u""_s, u""_s, u""_s}, // Superscript
    };
    addExtendedSyntaxs(extendedSyntaxsList);
}

// Connections
// ===========
void EditorHandler::connectPlugins()
{
    // Code Highlight
    connect(m_config, &KleverConfig::codeSynthaxHighlightEnabledChanged, this, &EditorHandler::codeHighlightEnabledChanged);
    codeHighlightEnabledChanged();
    connect(m_config, &KleverConfig::codeSynthaxHighlighterStyleChanged, this, &EditorHandler::newHighlightStyle);
    newHighlightStyle();

    // Puml
    connect(m_config, &KleverConfig::pumlEnabledChanged, this, &EditorHandler::pumlEnabledChanged);
    pumlEnabledChanged();
    connect(m_config, &KleverConfig::pumlDarkChanged, this, &EditorHandler::pumlDarkChanged);
    pumlDarkChanged();
}

void EditorHandler::connectHighlight()
{
    connect(m_config, &KleverConfig::editorFontChanged, this, &EditorHandler::editorFontChanged);
    editorFontChanged();

    connect(m_config, &KleverConfig::editorHighlightEnabledChanged, this, &EditorHandler::editorHighlightEnabledChanged);

    connect(m_config, &KleverConfig::adaptiveTagSizeEnabledChanged, this, &EditorHandler::adaptiveTagSizeChanged);
    adaptiveTagSizeChanged();

    connect(m_config, &KleverConfig::tagSizeScaleChanged, this, &EditorHandler::tagScaleChanged);
    tagScaleChanged();
}

void EditorHandler::connectTimer()
{
    m_cursorMoveTimer->setSingleShot(true);
    m_cursorMoveTimer->setInterval(200);

    connect(m_cursorMoveTimer, &QTimer::timeout, this, &EditorHandler::cursorMovedTimeOut);
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

    if (m_cursorMoveTimer) {
        m_cursorMoveTimer->start();
    }
}
// !QTextDocument Info

// KleverNotes method
// ==================
// Parser
void EditorHandler::parseDoc()
{
    if (!m_highlighting && m_notePath != QStringLiteral("qrc:")) {
        parse(m_document->toPlainText());
    }
}

void EditorHandler::parse(const QString &src)
{
    static const QString qrcStr = QStringLiteral("qrc:");
    if (m_notePath != qrcStr && !m_notePath.endsWith(QStringLiteral(".md"))) {
        return;
    }

    m_textChanged = !m_noteFirstHighlight;
    if (m_noteFirstHighlight) { // Important for textCursor(), QML is to slow to transfer this info
        m_cursorPosition = src.length();
        m_noteFirstHighlight = false;
    }
    if (m_pluginHelper) {
        m_pluginHelper->clearPluginsInfo();
    }
    
    m_currentMdDoc = m_parser->parse(src);
    if (m_config->editorHighlightEnabled()) {
        highlightSyntax(m_colors, m_currentMdDoc);
    }

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
    m_noteFirstHighlight = true;

    QString rendererNotePath = notePath;
    if (notePath != qrcStr) {
        rendererNotePath.chop(pathEnd.length());
    }

    m_renderer->setNotePath(rendererNotePath);

    if (notePath != qrcStr && m_pluginHelper) {
        // We do this here because we're sure to be in another note
        m_pluginHelper->clearPluginsPreviousInfo();

        m_pluginHelper->mapperParserUtils()->setNotePath(rendererNotePath);
    }
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

        if (m_pluginHelper) {
            m_pluginHelper->postTokChanges();
        }
        Q_EMIT parsingFinished(html);
    }
}
// !Rendering

// Highlight
// =========
void EditorHandler::highlightSyntax(const Colors &colors, std::shared_ptr<MD::Document<MD::QStringTrait>> doc)
{
    if (!m_notePath.isEmpty() && m_notePath != QStringLiteral("qrc:")) {
        m_highlighting = true;
        m_editorHighlighter->highlight(doc, colors);
        m_highlighting = false;
    }
}

// Colors
void EditorHandler::changeStyles(const QStringList &styles)
{
    m_colors.backgroundColor = QColor(styles[0]);
    m_colors.textColor = QColor(styles[1]);
    m_colors.titleColor = QColor(styles[2]);
    m_colors.linkColor = QColor(styles[3]);
    m_colors.codeBgColor = QColor(styles[5]);
    m_colors.codeColor = QColor(styles[1]).darker(125);
    m_colors.highlightColor = QColor(styles[6]);
    m_colors.specialColor = QColor(styles[7]);

    highlightSyntax(m_colors, m_currentMdDoc);
}
// !Colors

// Font
void EditorHandler::editorFontChanged()
{
    m_editorHighlighter->setFont(KleverConfig::editorFont());
}
// !Font
// !Highlight

// ExtendedSyntax
void EditorHandler::addExtendedSyntax(const QStringList &details)
{
    const long long int opts = MD::TextOption::StrikethroughText << (m_extendedSyntaxCount + 1);
    m_renderer->addExtendedSyntax(opts, details[1], details[2]);

    m_editorHighlighter->addExtendedSyntax(opts, details);

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
// !KleverNotes method

// md-editor method
// ================
std::shared_ptr<MD::Document<MD::QStringTrait>> EditorHandler::currentDoc() const
{
    return m_currentMdDoc;
}
// !md-editor method

// KleverNotes slots
// =================
// Plugins
void EditorHandler::codeHighlightEnabledChanged()
{
    m_config->save();
    m_renderer->setCodeHighlightEnable(KleverConfig::codeSynthaxHighlightEnabled());
}

void EditorHandler::newHighlightStyle()
{
    m_config->save();
    m_pluginHelper->highlightParserUtils()->newHighlightStyle();
}

void EditorHandler::pumlEnabledChanged()
{
    m_config->save();
    m_renderer->setPUMLenable(KleverConfig::pumlEnabled());
}

void EditorHandler::pumlDarkChanged()
{
    m_config->save();
    m_renderer->setPUMLdark(KleverConfig::pumlDark());
}
// !Plugins

// Highlight
void EditorHandler::editorHighlightEnabledChanged()
{
    m_config->save();
    if (!m_config->editorHighlightEnabled()) {
        m_editorHighlighter->clearHighlighting();
    }
}

void EditorHandler::adaptiveTagSizeChanged()
{
    m_config->save();
    m_editorHighlighter->changeAdaptiveTagSize(m_config->adaptiveTagSizeEnabled());
}

void EditorHandler::tagScaleChanged()
{
    m_config->save();
    m_editorHighlighter->changeTagScale(m_config->tagSizeScale());
}

void EditorHandler::cursorMovedTimeOut()
{
    if (!m_textChanged && m_notePath.endsWith(QStringLiteral(".md"))) {
        m_editorHighlighter->showDelimAroundCursor(m_textChanged);
    }

    m_textChanged = false;
}
// !Highlight
// !KleverNotes slots
}
