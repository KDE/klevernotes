// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

#include <QQuickTextDocument>

QT_BEGIN_NAMESPACE
class QTextDocument;

QT_END_NAMESPACE

class EditorHandler;
class MarkdownHighlighter : public QSyntaxHighlighter
{
public:
    inline QQuickTextDocument *textDocument() const
    {
        return m_quickDocument;
    };
    void setTextDocument(QQuickTextDocument *textDocument)
    {
        if (!textDocument)
            return;
        m_quickDocument = textDocument;
        setDocument(m_quickDocument->textDocument());
    };

    MarkdownHighlighter(QTextDocument *parent = nullptr, EditorHandler *editorHandler = nullptr);

    bool pathChanged = true;
    void reset();
    void addHighlightToken(const std::tuple<QString, int, int> &token);
    void setNewContent(const QString &content);

protected Q_SLOTS:
    void timerTick();

protected:
    QQuickTextDocument *m_quickDocument = nullptr;
    EditorHandler *m_editorHandler = nullptr;

    // When using setPlainText, the doc will be erased completely before adding the new text back up.
    // This means that the highlightBlock will be called with an empty doc and then called again with
    // the content.
    // With this set to true, the call with the empty doc will be skiped
    bool m_skipFirst = true;

    void highlightBlock(const QString &text) override;

    bool _highlightingFinished;
    QTimer *_timer;

    void nextToken();
    int m_currentTokenPtr = 0;
    int m_currentTokenStart = 0;
    int m_currentTokenEnd = 0;
    QString m_currentTokenName = QLatin1String();
    QList<std::tuple<QString, int, int>> m_highlighterTokens;
};
