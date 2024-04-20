// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// PARTIALLY BASED ON: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quickcontrols2/texteditor?h=6.2.0

#pragma once

#include "logic/wysiwyg/markdownHighlighter.h"
#include <QObject>
#include <QQuickTextDocument>
#include <QTextCursor>
#include <QTextDocument>

Q_DECLARE_METATYPE(QQuickTextDocument);

class Parser;
class EditorHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument *document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)

    Q_PROPERTY(QString notePath READ parserGetNotePath WRITE parserSetNotePath)

public:
    explicit EditorHandler(QObject *parent = nullptr);

    int cursorPosition() const;
    int selectionEnd() const;
    int selectionStart() const;

    QString parserGetNotePath() const;
    void parserSetNotePath(const QString &notePath);

    Q_INVOKABLE QString parse(QString src);

Q_SIGNALS:
    void documentChanged();
    void modifiedChanged();
    void selectionStartChanged();
    void selectionEndChanged();
    void cursorPositionChanged(const int position);

    // NoteMapper
    void newLinkedNotesInfos(const QSet<QStringList> &linkedNotesInfos);
    void noteHeadersSent(const QString &notePath, const QStringList &noteHeaders);

public Q_SLOTS:
    // Syntax highlight
    void newHighlightStyle();
    // PUML
    void pumlDarkChanged();

private:
    QTextDocument *m_textDocument;
    QQuickTextDocument *m_document;
    QQuickTextDocument *document() const;
    void setDocument(QQuickTextDocument *document);

    void setCursorPosition(const int cursorPosition);

    void setSelectionStart(const int selectionStart);

    void setSelectionEnd(const int selectionEnd);

    int m_cursorPosition;
    int m_selectionStart;
    int m_selectionEnd;

    Parser *m_parser = nullptr;
    MarkdownHighlighter *m_markdownHighlighter = nullptr;
};
