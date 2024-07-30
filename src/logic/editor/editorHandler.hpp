// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// In part based on: https://github.com/igormironchik/markdown-tools/tree/main 'md-editor'

#pragma once

// KleverNotes include
#include "colors.hpp"
#include "logic/parser/plugins/pluginHelper.h"
#include "logic/parser/renderer.h"

// md4qt include.
#define MD4QT_QT_SUPPORT
#include "logic/parser/md4qt/doc.hpp"
#include "logic/parser/md4qt/traits.hpp"

// Qt include
#include <QObject>
#include <QQuickTextDocument>
#include <QTextDocument>
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

    Q_PROPERTY(QString notePath READ getNotePath WRITE setNotePath)

public:
    explicit EditorHandler(QObject *parent = nullptr);

    // Connections
    void connectPlugins();

    // QTextDocument info
    QTextDocument *document() const;
    QQuickTextDocument *qQuickDocument() const;
    QTextCursor textCursor() const;
    int cursorPosition() const;

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

Q_SIGNALS:
    void documentChanged();
    void cursorPositionChanged(const int position);

    void parsingFinished(const QString &content);

    // NoteMapper
    void newLinkedNotesInfos(const QSet<QStringList> &linkedNotesInfos);

private Q_SLOTS:
    // Code highlight
    void codeHighlightEnabledChanged();
    void newHighlightStyle();

    // PUML
    void pumlEnabledChanged();
    void pumlDarkChanged();

private:
    // QTextDocument info
    void setDocument(QQuickTextDocument *document);
    void setCursorPosition(const int cursorPosition);

    // Render
    void renderDoc();

    // ExtendedSyntax
    void addExtendedSyntax(const QStringList &details);
    void addExtendedSyntaxs(const QList<QStringList> &syntaxsDetails);

    // Highlight
    void highlightSyntax(const Colors &colors, std::shared_ptr<MD::Document<MD::QStringTrait>> doc);

private:
    // QTextDocument info
    QQuickTextDocument *m_qQuickDocument = nullptr;
    QTextDocument *m_document = nullptr;
    int m_cursorPosition = 0;

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
    bool m_highlighting = false; // Used as a switch to prevent the highlighting from retriggering the parsing
    Colors m_colors;

    Q_DISABLE_COPY(EditorHandler)
};
}
