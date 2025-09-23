// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024-2025 Louis Schul <schul9louis@gmail.com>

#include "editorHandler.hpp"

// KleverNotes include
#include "editorHighlighter.hpp"
#include "logic/editor/editorTextManipulation.hpp"
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
    , m_parsingThread(new QThread(this))
    , m_renderer(new Renderer())
    , m_pluginHelper(new PluginHelper(this))
    , m_editorHighlighter(new EditorHighlighter(this))
    , m_cursorMoveTimer(new QTimer(this))
{
    m_renderer->addPluginHelper(m_pluginHelper);

    connectParser();

    connect(m_config, &KleverConfig::previewVisibleChanged, this, &EditorHandler::renderPreviewStateChanged);
    changeRenderPreviewState();

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
        {u"--"_s, u"<sub>"_s, u"</sub>"_s, u""_s, u""_s, u""_s, u"2"_s, u"y"_s, u"y"_s, u""_s, u""_s, u""_s}, // Subscript
        {u"^"_s, u"<sup>"_s, u"</sup>"_s, u""_s, u""_s, u""_s, u"1"_s, u"y"_s, u"y"_s, u""_s, u""_s, u""_s}, // Superscript
    };
    addExtendedSyntaxs(extendedSyntaxsList);
}

EditorHandler::~EditorHandler()
{
    m_parsingThread->quit();
    m_parsingThread->wait();
}

// Connections
// ===========
void EditorHandler::connectParser()
{
    m_parser->moveToThread(m_parsingThread);
    connect(this, &EditorHandler::askForParsing, m_parser, &Parser::onData, Qt::QueuedConnection);
    connect(m_parser, &Parser::done, this, &EditorHandler::onParsingDone, Qt::QueuedConnection);
    m_parsingThread->start();
}

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

int EditorHandler::selectionStart() const
{
    return m_selectionStart;
}

void EditorHandler::setSelectionStart(const int position)
{
    if (position == m_selectionStart) {
        return;
    }

    m_selectionStart = position;

    if (m_cursorMoveTimer && m_selectionStart == m_selectionEnd) {
        m_cursorMoveTimer->start();
    }
}

int EditorHandler::selectionEnd() const
{
    return m_selectionEnd;
}

void EditorHandler::setSelectionEnd(const int position)
{
    if (position == m_selectionEnd) {
        return;
    }

    m_selectionEnd = position;

    if (m_cursorMoveTimer && m_selectionStart == m_selectionEnd) {
        m_cursorMoveTimer->start();
    }
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
    m_textChanged = !m_noteFirstHighlight;
    if (m_pluginHelper) {
        m_pluginHelper->clearPluginsInfo();
    }

    if (m_parseCount == std::numeric_limits<unsigned long long int>::max()) {
        m_parseCount = 0;
    }

    ++m_parseCount;

    Q_EMIT askForParsing(src, m_noteDir, m_noteName, m_parseCount);
}

QString EditorHandler::getNotePath() const
{
    return m_noteDir + QStringLiteral("+") + m_noteName;
}

void EditorHandler::setNotePath(const QString &notePath)
{
    QDir note(notePath);
    setNoteName(note.dirName());
    note.cdUp();
    setNoteDir(note.absolutePath());
}

QString EditorHandler::getNoteName() const
{
    return m_noteName;
}

void EditorHandler::setNoteName(const QString &noteName)
{
    m_noteName = noteName;
}

QString EditorHandler::getNoteDir() const
{
    return m_renderer->getNoteDir();
}

void EditorHandler::setNoteDir(const QString &noteDir)
{
    m_noteDir = noteDir;
    m_renderer->setNoteDir(noteDir);

    if (m_pluginHelper) {
        // We do this here because we're sure to be in another note
        m_pluginHelper->clearPluginsPreviousInfo();

        m_pluginHelper->mapperParserUtils()->setNotePath(noteDir);
    }
    m_noteFirstHighlight = true;

    Q_EMIT renderingFinished({});
    parseDoc();
}

// !Parser

// Rendering
void EditorHandler::renderDoc()
{
    if (m_currentMdDoc) {
        const auto html = m_renderer->toHtml(m_currentMdDoc, QStringLiteral("&nbsp;&hookleftarrow;&nbsp;"));

        if (m_pluginHelper) {
            m_pluginHelper->postTokChanges();
        }
        Q_EMIT renderingFinished(html);
    }
}

void EditorHandler::changeRenderPreviewState(const bool _enabled)
{
    // If we close the PrintingDialog (!enabled) but the preview is still visible
    const bool enabled = _enabled || m_config->previewVisible();
    if (m_renderEnabled != enabled) {
        m_renderEnabled = enabled;
        if (m_renderEnabled) {
            renderDoc();
        }
    }
}
// !Rendering

// Highlight
// =========
void EditorHandler::cacheAndHighlightSyntax(std::shared_ptr<MD::Document<MD::QStringTrait>> doc)
{
    if (!m_noteDir.isEmpty()) {
        m_highlighting = true;
        m_editorHighlighter->cacheAndHighlight(doc, m_config->editorHighlightEnabled());
        m_highlighting = false;
    }
}

// Colors
void EditorHandler::changeStyles(const QStringList &styles)
{
    Colors colors;
    colors.backgroundColor = QColor(styles[0]);
    colors.textColor = QColor(styles[1]);
    colors.titleColor = QColor(styles[2]);
    colors.linkColor = QColor(styles[3]);
    colors.codeBgColor = QColor(styles[5]);
    colors.codeColor = QColor(styles[1]).darker(125);
    colors.highlightColor = QColor(styles[6]);
    colors.specialColor = QColor(styles[7]);

    m_editorHighlighter->setColors(colors);

    if (m_config->editorHighlightEnabled()) {
        cacheAndHighlightSyntax(m_currentMdDoc);
    }
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

// Toolbar
QList<posCacheUtils::DelimsInfo> EditorHandler::getSurroundingDelims() const
{
    return m_surroundingDelims;
}

void EditorHandler::handleDelims(const bool addDelims, const int delimType)
{
    if (addDelims) {
        const bool succes = editorTextManipulation::addDelims(this, delimType);
        if (!succes) {
            Q_EMIT uncheckAction(delimType);
        }
    } else {
        editorTextManipulation::removeDelims(this, delimType);
    }

    Q_EMIT focusEditor();
}
// !Toolbar

// Editor nice to have
void EditorHandler::handleTabPressed(const bool backtab)
{
    editorTextManipulation::handleTabPressed(this, m_config->useSpaceForTab(), m_config->spacesForTab(), backtab);
}

void EditorHandler::handleReturnPressed(const int modifier)
{
    const auto cursor = textCursor();
    const int pos = cursor.block().length() - 2;
    const int line = cursor.blockNumber();

    const MD::ListItem<MD::QStringTrait> *listItem = m_editorHighlighter->searchListItem(line, pos);

    editorTextManipulation::handleReturnPressed(this, listItem, m_config->useSpaceForTab(), modifier);
    m_textChanged = false;

    Q_EMIT focusEditor();
}
// !Editor nice to have
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
// Parsing
void EditorHandler::onParsingDone(std::shared_ptr<MD::Document<MD::QStringTrait>> mdDoc, unsigned long long int parseCount)
{
    if (parseCount == m_parseCount) {
        m_currentMdDoc = mdDoc;
        cacheAndHighlightSyntax(m_currentMdDoc);

        if (m_noteFirstHighlight) {
            m_noteFirstHighlight = false;
            Q_EMIT focusEditor();
        }

        if (m_renderEnabled) {
            renderDoc();
        }
    }
}
// !Parsing

// Rendering
void EditorHandler::renderPreviewStateChanged()
{
    changeRenderPreviewState(m_config->previewVisible());
}
// !Rendering

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
    if (!m_textChanged) {
        m_highlighting = true;
        m_surroundingDelims = m_editorHighlighter->showDelimAroundCursor(m_textChanged);
        m_highlighting = false;

        QList<int> delimsTypes;
        for (const auto &delimInfo : m_surroundingDelims) {
            if (!delimsTypes.contains(delimInfo.delimType)) {
                delimsTypes.append(delimInfo.delimType);
            }
        }
        Q_EMIT surroundingDelimsChanged(delimsTypes);
    }

    m_textChanged = false;
}
// !Highlight
// !KleverNotes slots
}
