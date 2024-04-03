// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

// PARTIALLY BASED ON: https://code.qt.io/cgit/qt/qtdeclarative.git/tree/examples/quickcontrols2/texteditor?h=6.2.0

#pragma once

#include <QKeyEvent>
#include <QObject>
#include <QQuickTextDocument>
#include <QString>
#include <QTextCursor>
#include <QTextDocument>

Q_DECLARE_METATYPE(QQuickTextDocument);

class VimHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickTextDocument *document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)
    Q_PROPERTY(QQuickItem *textArea READ textArea WRITE setTextArea NOTIFY textAreaChanged)

    Q_PROPERTY(bool vimOn READ getVimOn WRITE setVimOn)
    Q_PROPERTY(int mode MEMBER m_currentMode NOTIFY modeChanged FINAL)

public:
    explicit VimHandler(QObject *parent = nullptr);

    Q_INVOKABLE bool handleKeyPress(const int key, const int modifiers);

Q_SIGNALS:
    void textAreaChanged();
    void documentChanged();
    void cursorPositionChanged(const int position);
    void selectionStartChanged();
    void selectionEndChanged();
    void anchorsPositionChanged();

    void modifiedChanged();

    void modeChanged(const int mode) const;
    void deselect() const;
    void cut() const;

private:
    // VIM calculation
    enum EditorMode {
        Normal = Qt::UserRole + 1,
        Insert,
        Visual,
    };

    bool earlyReturn(const int key, const bool isShift);
    bool handleNormalMode(const int key, const bool isShift);
    bool handleMove(const int key, const bool isShift);

    QTextCursor getCursor() const;
    void moveCursor(const QTextCursor::MoveOperation moveOperation);
    void moveCursorTo(const int newPosition);
    void sendKeyEvent(const int key, const Qt::KeyboardModifier modifiers);

    bool emptyBlock() const;

    bool getVimOn() const;
    void setVimOn(const bool vimOn);
    bool m_vimOn = false;

    int getMode() const;
    void setMode(const int mode);
    int m_currentMode = EditorMode::Normal;
    bool m_isVisual = false;

    int m_tempCursorPosition;
    QList<int> m_keyBuffer;

    // Document manipulation
    QTextDocument *m_textDocument;
    QQuickTextDocument *m_document;
    QQuickTextDocument *document() const;
    void setDocument(QQuickTextDocument *document);

    QQuickItem *m_textArea;
    QQuickItem *textArea() const;
    void setTextArea(QQuickItem *textArea);

    int cursorPosition() const;
    void setCursorPosition(const int cursorPosition);

    int selectionStart() const;
    void setSelectionStart(const int selectionStart);

    int selectionEnd() const;
    void setSelectionEnd(const int selectionEnd);

    int m_cursorPosition;
    int m_selectionStart;
    int m_selectionEnd;
};
