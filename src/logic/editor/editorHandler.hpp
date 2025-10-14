// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024-2025 Louis Schul <schul9louis@gmail.com>

// In part based on: https://github.com/igormironchik/markdown-tools/tree/main 'md-editor'

#pragma once

// KleverNotes include
#include "colors.hpp"
#include "kleverconfig.h"
#include "logic/editor/posCacheUtils.hpp"
#include "logic/parser/plugins/pluginHelper.h"
#include "logic/parser/renderer.h"

// md4qt include.
#define MD4QT_QT_SUPPORT
#include "logic/parser/md4qt/doc.h"
#include "logic/parser/md4qt/traits.h"

// Qt include
#include <QObject>
#include <QQuickTextDocument>
#include <QTextDocument>
#include <QTimer>
#include <QtQml>

Q_DECLARE_METATYPE(QQuickTextDocument);

namespace MdEditor
{

class Parser;
class EditorHighlighter;
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
    std::shared_ptr<MD::Document<MD::QStringTrait>> currentDoc() const;

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

    // Render
    /**
     * @brief Receives the info the preview being rendered has changed.
     */
    void renderPreviewStateChanged();

    // markdown-tools editor
    /**
     * @brief Receives the info the parsing is finished.
     */
    void onParsingDone(std::shared_ptr<MD::Document<MD::QStringTrait>>, unsigned long long int);

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
    void cacheAndHighlightSyntax(std::shared_ptr<MD::Document<MD::QStringTrait>> doc);

private:
    // Config Connections
    KleverConfig *m_config;

    // QTextDocument info
    QQuickTextDocument *m_qQuickDocument = nullptr;
    QTextDocument *m_document = nullptr;
    int m_cursorPosition = 0;
    int m_selectionStart = 0;
    int m_selectionEnd = 0;

    // Parsing
    QString m_noteDir;
    QString m_noteName;
    Parser *m_parser = nullptr;
    unsigned long long int m_parseCount;
    QThread *m_parsingThread = nullptr;
    std::shared_ptr<MD::Document<MD::QStringTrait>> m_currentMdDoc = nullptr;

    // Rendering
    bool m_renderEnabled = true;
    Renderer *m_renderer = nullptr;

    // Plugins
    PluginHelper *m_pluginHelper = nullptr;
    // md4qt plugins
    enum ExtensionID : int {
        /* Extended syntax
         * ===============*/
        ExtendedSyntax = 256, // MD::TextPlugin::UserDefinedPluginID + 1
        /* Plugins
         * ===============*/
        KleverPlugins = ExtendedSyntax + 64,
    };
    int m_extendedSyntaxCount = 0;

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
