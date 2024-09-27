// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

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
class EditorHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument *document READ qQuickDocument WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)

    Q_PROPERTY(QString notePath READ getNotePath WRITE setNotePath)

public:
    explicit EditorHandler(QObject *parent = nullptr);

    // QTextDocument info
    QTextDocument *document() const;
    QQuickTextDocument *qQuickDocument() const;
    QTextCursor textCursor() const;
    int cursorPosition() const;
    int selectionStart() const;
    int selectionEnd() const;

    // Parser
    void parseDoc();
    Q_INVOKABLE void parse(const QString &src);
    QString getNotePath() const;
    void setNotePath(const QString &notePath);
    Q_INVOKABLE void usePreviousPath();
    Parser *parser() const;

    // md-editor
    std::shared_ptr<MD::Document<MD::QStringTrait>> currentDoc() const;

    // Highlight
    EditorHighlighter *editorHighlighter() const;

    // Colors
    Q_INVOKABLE void changeStyles(const QStringList &styles);

    // Toolbar
    QList<posCacheUtils::DelimsInfo> getSurroundingDelims() const;
    Q_INVOKABLE void handleDelims(const bool addDelims, const int delimType);

    // Editor nice to have
    Q_INVOKABLE void handleTabPressed(const bool backtab);
    Q_INVOKABLE void handleReturnPressed(const int modifier);

Q_SIGNALS:
    void documentChanged();
    void cursorPositionChanged(const int position);
    void selectionStartChanged(const int position);
    void selectionEndChanged(const int position);

    void parsingFinished(const QString &content);

    // Toolbar
    void surroundingDelimsChanged(const QList<int> &delimsTypes);
    void focusEditor();
    void uncheckAction(const int delimType);

    // NoteMapper
    void newLinkedNotesInfos(const QSet<QStringList> &linkedNotesInfos);

public Q_SLOTS:
    // Font
    void editorFontChanged();

private Q_SLOTS:
    // Code highlight
    void codeHighlightEnabledChanged();
    void newHighlightStyle();

    // PUML
    void pumlEnabledChanged();
    void pumlDarkChanged();

    // Highlight
    void editorHighlightEnabledChanged();
    void adaptiveTagSizeChanged();
    void tagScaleChanged();
    void cursorMovedTimeOut();

private:
    // Config Connections
    void connectPlugins();
    void connectHighlight();
    void connectTimer();

    // QTextDocument info
    void setDocument(QQuickTextDocument *document);
    void setCursorPosition(const int cursorPosition);
    void setSelectionStart(const int position);
    void setSelectionEnd(const int position);

    // Render
    void renderDoc();

    // ExtendedSyntax
    void addExtendedSyntax(const QStringList &details);
    void addExtendedSyntaxs(const QList<QStringList> &syntaxsDetails);

    // Highlight
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
    QString m_previousPath;
    QString m_notePath;
    Parser *m_parser = nullptr;
    std::shared_ptr<MD::Document<MD::QStringTrait>> m_currentMdDoc = nullptr;

    // Rendering
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
