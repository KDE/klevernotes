// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// PARTIALLY BASED ON: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quickcontrols2/texteditor?h=6.2.0

#pragma once

#include <QObject>
#include <QQuickTextDocument>
#include <QTextCursor>
#include <QTextDocument>

Q_DECLARE_METATYPE(QQuickTextDocument);

class VimHandler;
class EditorHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument *document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)

public:
    explicit EditorHandler(QObject *parent = nullptr);

    Q_INVOKABLE bool handleKeyPress(const int key, const int modifiers) const;

    int cursorPosition() const;

    QTextCursor getCursor() const;
    void moveCursorTo(const int position, const bool visual = false);
    int getLastBlockPosition() const;
    int getLastCharPosition() const;
    int getCapturePosition(const QRegularExpression &reg, const int from = -1) const;
    QString trimmedCharAt(const int position) const;

Q_SIGNALS:
    void documentChanged();
    void cursorPositionChanged(const int position);
    void selectionStartChanged();
    void selectionEndChanged();
    void anchorsPositionChanged();

    void moveSelection(const int position);
    void modifiedChanged();

    void modeChanged(const int mode) const;
    void deselect() const;
    void cut() const;

private:
    VimHandler *m_vimHandler;

    QTextDocument *m_textDocument;
    QQuickTextDocument *m_document;
    QQuickTextDocument *document() const;
    void setDocument(QQuickTextDocument *document);

    void setCursorPosition(const int cursorPosition);

    int selectionStart() const;
    void setSelectionStart(const int selectionStart);

    int selectionEnd() const;
    void setSelectionEnd(const int selectionEnd);

    int m_cursorPosition;
    int m_selectionStart;
    int m_selectionEnd;
};
