// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QMap>
#include <QObject>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QStandardPaths>

class HighlightHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QStringList highlighters READ getHighlighters CONSTANT)
public:
    explicit HighlightHelper(QObject *parent = nullptr);

    QStringList getHighlighters() const;
    Q_INVOKABLE QStringList getHighlighterStyle(const QString &highlighter) const;
    static QString getHighlightedString(const QString &inputStr, const QString &lang);

private:
    inline static const QString m_chromaName = QStringLiteral("chroma");
    inline static const QString m_pygmentizeName = QStringLiteral("pygmentize");
    inline static const QString m_kSyntaxName = QStringLiteral("ksyntaxhighlighter6");
    inline static const QString m_kateSyntaxName = QStringLiteral("kate-syntax-highlighter");
    inline static QMap<QString, QStringList> m_highlightersCommands = {
        {
            m_chromaName,
            {QStringLiteral(" --list"), QStringLiteral(" --style=\"nord\" --lexer=%1 --html --html-inline-styles")},
        },
        {
            m_pygmentizeName,
            {QStringLiteral(" -L styles"), QStringLiteral(" -l %1 -f html -O style=nord -O noclasses=True")},
        },
        {
            m_kSyntaxName,
            {QStringLiteral(" --list-themes"), QStringLiteral(" --stdin -s %1 -f html -t 'Nord' -b")},
        },
        {
            m_kateSyntaxName,
            {QStringLiteral(" --list-themes"), QStringLiteral(" --stdin -s %1 -f html -t 'Nord' -b")},
        },
    }; // nord style by default, will be replace by the given style if it exists

    inline static const QRegularExpression m_pygmentizeRegex = QRegularExpression(QStringLiteral("(\\* )(.+)(:)"));

    QStringList getHighlighterStyleFromCmd(const QString &highlighter) const;
    void setAvailableHighlighters();
    inline static QMap<QString, QStringList> m_availableHighlighters;
};
