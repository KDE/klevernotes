// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024-2025 Louis Schul <schul9louis@gmail.com>

// In part based on: https://github.com/igormironchik/markdown-tools/tree/main 'md-editor'

#pragma once

// KleverNotes include
#include "kleverconfig.h"
#include "logic/editor/editorHighlighter.hpp"
#include "logic/editor/posCacheUtils.hpp"
#include "logic/parser/plugins/pluginHelper.h"
#include "logic/parser/renderer.h"

// md4qt include.
#include <md4qt/doc.h>
#include <md4qt/poscache.h>
#include <md4qt/utils.h>

// Qt include
#include <QObject>
#include <QQuickTextDocument>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTimer>
#include <QtQml>

Q_DECLARE_METATYPE(QQuickTextDocument);

namespace MdEditor
{

/**
 * @brief Returns whether the link is in items.
 *
 * @param items Stack of items.
 */
MD::Link *isLink(const MD::PosCache::Items &items);

class Parser;

/**
 * @class EditorHandler
 * @brief Class giving access to the QML TextArea.
 */
class EditorHandler : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QQuickTextDocument *document READ qQuickDocument WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)
    Q_PROPERTY(int cursorUnderMouse READ cursorUnderMouse WRITE setCursorUnderMouse NOTIFY cursorUnderMouseChanged)

    Q_PROPERTY(QString notePath READ getNotePath WRITE setNotePath)

public:
    explicit EditorHandler(QObject *parent = nullptr);
    ~EditorHandler();

    // QTextDocument info
    /**
     * @brief Get the QTextDocument associated to the QML TextArea.
     *
     * @return The QTextDocument associated to the QML TextArea.
     */
    QTextDocument *document() const;

    /**
     * @brief Get the QQuickTextDocument associated to the QML TextArea.
     * Required by Q_PROPERTY
     *
     * @return The QQuickTextDocument associated to the QML TextArea.
     */
    QQuickTextDocument *qQuickDocument() const;

    /**
     * @brief Get a QTextCursor representation of the current cursor of the TextArea.
     *
     * @return A QTextCursor representing the current cursor of the TextArea.
     */
    QTextCursor textCursor() const;

    /**
     * @brief Get the cursor current position.
     *
     * @return The cursor position.
     */
    int cursorPosition() const;

    /**
     * @brief Get the TextArea current selection starting position.
     * This equals cursorPosition if there's nothing selected.
     *
     * @return The selection starting position.
     */
    int selectionStart() const;

    /**
     * @brief Get the TextArea current selection ending position.
     * This equals cursorPosition if there's nothing selected.
     *
     * @return The selection ending position.
     */
    int selectionEnd() const;

    /**
     * @brief Get the TextArea current position under mouse cursor.
     * This position is set only with Ctrl keyboard modifier.
     *
     * @return The current position under mouse cursor.
     */
    int cursorUnderMouse() const;

    // Parser
    /**
     * @brief Parse the content of the TextArea.
     */
    void parseDoc();

    /**
     * @brief Parse the given src.
     *
     * @param src The string to be parsed.
     */
    void parse(const QString &src);

    /**
     * @brief Get the current note path.
     *
     * @return The current note path.
     */
    QString getNotePath() const;

    /**
     * @brief Set the current note path.
     *
     * @param notePath The current note path.
     */
    void setNotePath(const QString &notePath);

    /**
     * @brief Get the parser.
     *
     * @return The parser.
     */
    Parser *parser() const;

    // md-editor
    /**
     * @brief Get the MD::Document resulting of the parsing.
     */
    QSharedPointer<MD::Document> currentDoc() const;

    // Highlight
    /**
     * @brief Get the highlighter of the TextArea.
     *
     * @return The EditorHighlighter.
     */
    EditorHighlighter *editorHighlighter() const;

    // Colors
    /**
     * @brief Change the current style based on the given information.
     *
     * @param styles A list of string holding the different information of the styles.
     */
    Q_INVOKABLE void changeStyles(const QStringList &styles);

    // Toolbar
    /**
     * @brief Get the delims surrounding the cursor/selected text.
     *
     * @return A list of DelimsInfo.
     */
    QList<posCacheUtils::DelimsInfo> getSurroundingDelims() const;

    /**
     * @brief Add or remove delim based on the type.
     *
     * @param addDelims Whether to add, or remove the delims.
     * @param delimType The type of delim to add or remove.
     */
    Q_INVOKABLE void handleDelims(const bool addDelims, const int delimType);

    // Editor nice to have
    /**
     * @brief Handle the <Tab> key being pressed.
     *
     * @param backtab Whether it was a backtab (<Shift + Tab>).
     */
    Q_INVOKABLE void handleTabPressed(const bool backtab);

    /**
     * @brief Handle the <Return> key being pressed.
     *
     * @param modifier The modifier being pressed at the same time as <Return>.
     */
    Q_INVOKABLE void handleReturnPressed(const int modifier);

    // Render
    Q_INVOKABLE void changeRenderPreviewState(const bool enabled = false);

Q_SIGNALS:
    /**
     * @brief Signals that the document has changed.
     */
    void documentChanged();

    /**
     * @brief Signals that the cursor position has changed.
     *
     * @param position The new position.
     */
    void cursorPositionChanged(const int position);

    /**
     * @brief Force repaint of text area in QML.
     */
    void repaintTextArea();

    /**
     * @brief Signals that the selection starting position has changed.
     *
     * @param position The new position.
     */
    void selectionStartChanged(const int position);

    /**
     * @brief Signals that the selection ending position has changed.
     *
     * @param position The new position.
     */
    void selectionEndChanged(const int position);

    /**
     * @brief Signals that the position under mouse has changed.
     *
     * @param position The new position.
     */
    void cursorUnderMouseChanged(const int position);

    /**
     * @brief Signals that the editor wants to parse the given `md`.
     *
     * @param md The text to be parsed.
     * @param notePath The current note path.
     * @param noteName The current note name.
     * @param counter The current counter of parse.
     */
    void askForParsing(const QString &md, const QString &notePath, const QString &noteName, unsigned long long int counter);

    /**
     * @brief Signals that the render has finished and the content is available.
     *
     * @param content The rendered content (HTML).
     */
    void renderingFinished(const QString &content);

    // Toolbar
    /**
     * @brief Signals that the delims surrounding the cursor/selected text have changed.
     *
     * @param delimsTypes A list containing the new types of the delims surrounding the cursor/selected text.
     */
    void surroundingDelimsChanged(const QList<int> &delimsTypes);

    /**
     * @brief Ask QML to focus the editor.
     */
    void focusEditor();

    /**
     * @brief Ask for the given delim type to be unchecked in the toolbar.
     *
     * @param delimType The type of delim to be unchecked.
     */
    void uncheckAction(const int delimType);

    // NoteMapper
    /**
     * @brief Signals that a new set of linked notes is available.
     *
     * @param linkedNotesInfos The set of linked notes.
     */
    void newLinkedNotesInfos(const QSet<QStringList> &linkedNotesInfos);

public Q_SLOTS:
    // Font
    /**
     * @brief Receives the info that the editor font has changed.
     */
    void editorFontChanged();

    /**
     * @brief Text area was clicked.
     *
     * @param pos Cursor position.
     */
    Q_INVOKABLE void textClicked(const int pos);

private Q_SLOTS:
    // Code highlight
    /**
     * @brief Receives the info that the code highlighting plugin being enabled has changed.
     */
    void codeHighlightEnabledChanged();

    /**
     * @brief Receives the info that that code highlighting style has changed.
     */
    void newHighlightStyle();

    // PUML
    /**
     * @brief Receives the info that the PUML plugin being enabled has changed.
     */
    void pumlEnabledChanged();

    /**
     * @brief Receives the info that that PUML plugin using dark background has changed.
     */
    void pumlDarkChanged();

    // Highlight
    /**
     * @brief Receives the info that the editor highlighting being enabled has changed.
     */
    void editorHighlightEnabledChanged();

    /**
     * @brief Receives the info that the editor highlighting adaptive tag size being enabled has changed.
     */
    void adaptiveTagSizeChanged();

    /**
     * @brief Receives the info that the editor highlighting adaptive tag scale has changed.
     */
    void tagScaleChanged();

    /**
     * @brief Receives the info that timer tracking the cursor movement has timed out.
     */
    void cursorMovedTimeOut();

    /**
     * @brief Receives the info that the cursor under mouse has been changed.
     */
    void onCursorUnderMouseChanged(const int position);

    // Render
    /**
     * @brief Receives the info the preview being rendered has changed.
     */
    void renderPreviewStateChanged();

    // markdown-tools editor
    /**
     * @brief Receives the info the parsing is finished.
     */
    void onParsingDone(QSharedPointer<MD::Document>, unsigned long long int);

private:
    // Parser
    /**
     * @brief Get the current note name.
     *
     * @return The current note name.
     */
    QString getNoteName() const;

    /**
     * @brief Set the current note name.
     *
     * @param noteName The current note name.
     */
    void setNoteName(const QString &noteName);

    /**
     * @brief Get the current note directory.
     *
     * @return The current note directory.
     */
    QString getNoteDir() const;

    /**
     * @brief Set the current note directory.
     *
     * @param noteDir The current note directory.
     */
    void setNoteDir(const QString &noteDir);

    // Connections
    /**
     * @brief Connect the parser to its different signals handler.
     */
    void connectParser();

    /**
     * @brief Connects the different plugins.
     */
    void connectPlugins();

    /**
     * @brief Connects the editor highlighter.
     */
    void connectHighlight();

    /**
     * @brief Connects the timer tracking the cursor movement.
     */
    void connectTimer();

    // QTextDocument info
    /**
     * @brief Set the QQuickTextDocument from the QML TextArea.
     * Required by Q_PROPERTY
     *
     * @param document The QQuickTextDocument from the TextArea.
     */
    void setDocument(QQuickTextDocument *document);

    /**
     * @brief Set the current cursor position.
     *
     * @param cursorPosition The current cursor position.
     */
    void setCursorPosition(const int cursorPosition);

    /**
     * @brief Set the current selection starting position.
     *
     * @param position The current selection starting position.
     */
    void setSelectionStart(const int position);

    /**
     * @brief Set the current selection ending position.
     *
     * @param position The current selection ending position.
     */
    void setSelectionEnd(const int position);

    /**
     * @brief Set the current position under mouse.
     *
     * @param position The current position under mouse.
     */
    void setCursorUnderMouse(const int position);

    // Render
    /**
     * @brief Render the MD::Document resulting of the parsing.
     */
    void renderDoc();

    // ExtendedSyntax
    /**
     * @brief Add an extended syntax based on its details.
     *
     * @param details The details of the extended syntax.
     */
    void addExtendedSyntax(const QStringList &details);

    /**
     * @brief Helper function to add multiple extended syntax at once.
     * @see addExtendedSyntax
     *
     * @param syntaxsDetails A list of extended syntax details.
     */
    void addExtendedSyntaxs(const QList<QStringList> &syntaxsDetails);

    // Highlight
    /**
     * @brief Cache the current MD::Document and highlight the editor.
     *
     * @param doc The MD::Document resulting of the parsing.
     */
    void cacheAndHighlightSyntax(QSharedPointer<MD::Document> doc);

    /**
     * @brief Force the handler to update the delims surrounding the cursor/selected text.
     */
    void updateSurroundingDelims();

    /**
     * Returns actual URL for the link.
     */
    QString actualUrl(MD::Link *link) const;

    /**
     * @brief Handle link.
     */
    template<class Func>
    bool handleLink(const int position, Func f)
    {
        QTextCursor cursor = QTextCursor(m_document);
        cursor.setPosition(position);

        const auto lineNumber = cursor.block().blockNumber();
        const auto pos = cursor.position() - cursor.block().position();

        const auto link = isLink(m_editorHighlighter->findFirstInCache({pos, lineNumber, pos, lineNumber}));

        if (link) {
            const auto place = actualUrl(link);

            if (!place.startsWith(QLatin1Char('#'))) {
                QUrl u(place);
                const auto scheme = u.scheme().toLower();

                static const QString s_http = QStringLiteral("http");
                static const QString s_https = QStringLiteral("https");
                static const QString s_mailto = QStringLiteral("mailto");
                static const QString s_www = QStringLiteral("www.");

                // Handle hyperlinks only while.
                if (scheme == s_http || scheme == s_https || scheme == s_mailto || place.toLower().startsWith(s_www) || MD::isEmail(place)) {
                    f(link);

                    return true;
                }
            }
        }

        return false;
    }

private:
    // Config Connections
    KleverConfig *m_config;

    // QTextDocument info
    QQuickTextDocument *m_qQuickDocument = nullptr;
    QTextDocument *m_document = nullptr;
    int m_cursorPosition = 0;
    int m_selectionStart = 0;
    int m_selectionEnd = 0;
    int m_cursorUnderMouse = 0;

    // Parsing
    QString m_noteDir;
    QString m_noteName;
    Parser *m_parser = nullptr;
    unsigned long long int m_parseCount;
    QThread *m_parsingThread = nullptr;
    QSharedPointer<MD::Document> m_currentMdDoc = nullptr;

    // Rendering
    bool m_renderEnabled = true;
    Renderer *m_renderer = nullptr;

    // Plugins
    PluginHelper *m_pluginHelper = nullptr;

    // Editor highlight
    EditorHighlighter *m_editorHighlighter = nullptr;
    QTimer *m_cursorMoveTimer = nullptr;
    bool m_highlighting = false; // Used as a switch to prevent the highlighting from retriggering the parsing
    bool m_noteFirstHighlight = true;
    bool m_textChanged = false;

    // Toolbar
    QList<posCacheUtils::DelimsInfo> m_surroundingDelims;

    Q_DISABLE_COPY(EditorHandler)
};
}
