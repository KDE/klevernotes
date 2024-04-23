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

    Q_PROPERTY(QString notePath READ getNotePath WRITE setNotePath)

public:
    explicit EditorHandler(QObject *parent = nullptr);

    QQuickTextDocument *document() const;
    void setDocument(QQuickTextDocument *document);

    int cursorPosition() const;
    int selectionEnd() const;
    int selectionStart() const;

    QString getNotePath() const;
    void setNotePath(const QString &notePath);

    QString parse();

    // HIGHLIGHTER
    void addHighlightToken(const std::tuple<QString, int, int> &token);
Q_SIGNALS:
    void documentChanged();
    void selectionStartChanged();
    void selectionEndChanged();
    void cursorPositionChanged(const int position);

    // NoteMapper
    void newLinkedNotesInfos(const QSet<QStringList> &linkedNotesInfos);
    void noteHeadersSent(const QString &notePath, const QStringList &noteHeaders);
    void modifiedChanged(const bool changed);

public Q_SLOTS:
    // Code highlight
    void newHighlightStyle();
    // PUML
    void pumlDarkChanged();

private:
    QQuickTextDocument *m_document = nullptr;
    void setCursorPosition(const int cursorPosition);
    void setSelectionStart(const int selectionStart);
    void setSelectionEnd(const int selectionEnd);

    int m_cursorPosition;
    int m_selectionStart;
    int m_selectionEnd;

    QString m_notePath;
    Parser *m_parser = nullptr;
    MarkdownHighlighter *m_markdownHighlighter = nullptr;
};
