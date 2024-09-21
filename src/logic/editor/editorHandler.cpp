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
#include <qlogging.h>
#include <vector>

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
    if (m_pluginHelper) {
        m_pluginHelper->clearPluginsInfo();
    }
    
    m_currentMdDoc = m_parser->parse(src);
    if (m_config->editorHighlightEnabled()) {
        highlightSyntax(m_colors, m_currentMdDoc);
    }
    if (m_noteFirstHighlight) {
        m_noteFirstHighlight = false;
        Q_EMIT focusEditor();
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

// Toolbar
void EditorHandler::removeDelims(const int delimType)
{
    std::vector<MD::WithPosition> toRemove;
    for (const auto &delimInfo : m_surroundingDelims) {
        if (delimInfo.delimType == delimType) {
            toRemove.push_back(delimInfo.opening);
            if (delimInfo.closing.startLine() != -1) {
                toRemove.push_back(delimInfo.closing);
            }
        }
    }

    std::sort(toRemove.begin(), toRemove.end(), md4qtHelperFunc::StartColumnOrder{});

    QTextCursor cursor = textCursor();

    cursor.beginEditBlock();
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        const QTextBlock line = m_document->findBlockByNumber(it->startLine());
        const int startPos = line.position() + it->startColumn();
        const int endPos = line.position() + it->endColumn() + 1;

        cursor.setPosition(startPos, QTextCursor::MoveAnchor);
        cursor.setPosition(endPos, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
    cursor.endEditBlock();

    Q_EMIT focusEditor();
}

void EditorHandler::addDelims(const int delimType)
{
    bool increment = false;
    QString delimText;
    std::vector<int> delimsPos;
    switch (delimType) {
    case posCacheUtils::BlockDelimTypes::Heading1:
        delimsPos = getFuturDelimsPositions(false, true);
        delimText = QStringLiteral("# ");
        break;
    case posCacheUtils::BlockDelimTypes::Heading2:
        delimsPos = getFuturDelimsPositions(false, true);
        delimText = QStringLiteral("## ");
        break;
    case posCacheUtils::BlockDelimTypes::Heading3:
        delimsPos = getFuturDelimsPositions(false, true);
        delimText = QStringLiteral("### ");
        break;
    case posCacheUtils::BlockDelimTypes::Heading4:
        delimsPos = getFuturDelimsPositions(false, true);
        delimText = QStringLiteral("#### ");
        break;
    case posCacheUtils::BlockDelimTypes::Heading5:
        delimsPos = getFuturDelimsPositions(false, true);
        delimText = QStringLiteral("##### ");
        break;
    case posCacheUtils::BlockDelimTypes::Heading6:
        delimsPos = getFuturDelimsPositions(false, true);
        delimText = QStringLiteral("###### ");
        break;
    case posCacheUtils::BlockDelimTypes::CodeBlock:
        delimText = QStringLiteral("\n```\n");
        delimsPos = getFuturDelimsPositions(true);
        break;
    case posCacheUtils::BlockDelimTypes::BlockQuote:
        delimText = QStringLiteral("> ");
        delimsPos = getFuturDelimsPositions(false, true);
        break;
    case posCacheUtils::BlockDelimTypes::OrderedList:
        increment = true;
        delimText = QStringLiteral(". ");
        delimsPos = getFuturDelimsPositions(false, true);
        break;
    case posCacheUtils::BlockDelimTypes::UnorderedList:
        delimText = QStringLiteral("- ");
        delimsPos = getFuturDelimsPositions(false, true);
        break;
    case MD::TextOption::BoldText:
        delimText = QStringLiteral("**");
        delimsPos = getFuturDelimsPositions();
        break;
    case MD::TextOption::ItalicText:
        delimText = QStringLiteral("_");
        delimsPos = getFuturDelimsPositions();
        break;
    case MD::TextOption::StrikethroughText:
        delimText = QStringLiteral("~~");
        delimsPos = getFuturDelimsPositions();
        break;
    }

    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();
    int pos = delimsPos.size();
    for (auto it = delimsPos.rbegin(); it != delimsPos.rend(); ++it) {
        cursor.setPosition(*it);
        cursor.insertText(increment ? QString::number(pos) + delimText : delimText);
    }
    cursor.endEditBlock();

    Q_EMIT focusEditor();
}

// Credit to: https://stackoverflow.com/a/8216059
QString rstrip(const QString &str)
{
    int n = str.size() - 1;
    for (; n >= 0; --n) {
        if (!str.at(n).isSpace()) {
            return str.left(n + 1);
        }
    }
    return {};
}

int getLastInBlockNonEmptyPos(const QTextBlock &block)
{
    return rstrip(block.text()).length();
}

int getFirstInBlockNonEmptyPos(const QTextBlock &block)
{
    return getLastInBlockNonEmptyPos(block) - block.text().trimmed().length();
}

bool isEmptyBlock(const QTextBlock &block)
{
    // This way we also avoid line that only contains white space
    return getLastInBlockNonEmptyPos(block) == 0;
}

bool nextNonEmptyBlock(QTextBlock &block, const int finalPos)
{
    block = block.next();
    while (block.isValid() && isEmptyBlock(block) && !block.contains(finalPos)) {
        block = block.next();
    }
    return block.isValid();
}

std::vector<int> EditorHandler::getFuturDelimsPositions(const bool wrapSelection, const bool isBlockItemDelim)
{
    auto cursor = textCursor();
    if (cursor.position() != m_selectionStart) {
        cursor.setPosition(m_selectionStart);
    }
    auto block = cursor.block();

    std::vector<int> positions;
    if (wrapSelection) {
        const int blockStart = cursor.block().position();
        positions.push_back(blockStart);

        cursor.setPosition(m_selectionEnd);

        const int blockEnd = block.position() + block.length() - 1;
        positions.push_back(blockEnd);

        return positions;
    }

    QTextBlock lastNonEmptyBlock;
    if (!isEmptyBlock(block)) {
        lastNonEmptyBlock = block;
        positions.push_back(isBlockItemDelim ? block.position() : m_selectionStart);
    }

    int currentBlockNum = block.blockNumber();
    if (!block.contains(m_selectionEnd)) {
        while (nextNonEmptyBlock(block, m_selectionEnd) && !block.contains(m_selectionEnd)) {
            if (isBlockItemDelim) {
                positions.push_back(block.position());
            } else if (1 < block.blockNumber() - currentBlockNum) { // Blank line
                // Avoid the white space at the start and the end
                if (lastNonEmptyBlock.isValid()) {
                    positions.push_back(lastNonEmptyBlock.position() + getLastInBlockNonEmptyPos(lastNonEmptyBlock));
                }
                positions.push_back(block.position() + getFirstInBlockNonEmptyPos(block));
            }

            lastNonEmptyBlock = block;
            currentBlockNum = block.blockNumber();
        }
    }

    if (!block.isValid() && !lastNonEmptyBlock.isValid()) {
        return {};
    } else if (!block.isValid() && !isBlockItemDelim) {
        block = lastNonEmptyBlock;
    } else if (!block.isValid()) {
        return positions;
    }

    if (1 < block.blockNumber() - currentBlockNum) { // Blank line
        if (lastNonEmptyBlock.isValid()) {
            positions.push_back(lastNonEmptyBlock.position() + lastNonEmptyBlock.length() - 1);
        }
        positions.push_back(block.position());
    }

    positions.push_back(isBlockItemDelim ? block.position() : m_selectionEnd);
    return positions;
}
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
        m_surroundingDelims = m_editorHighlighter->showDelimAroundCursor(m_textChanged);

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
