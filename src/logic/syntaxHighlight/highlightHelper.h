// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QMap>
#include <QObject>
#include <QRegularExpression>
#include <QStandardPaths>

class HighlightHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList highlighters READ getHighlighters CONSTANT)
public:
    explicit HighlightHelper(QObject *parent = nullptr);

    QStringList getHighlighters() const;
    Q_INVOKABLE QStringList getHighlighterStyle(const QString &highlighter) const;
    static QString getHighlightedString(const QString &inputStr, const QString &lang);

private:
    inline static const QString m_chromaName = QStringLiteral("chroma");
    inline static const QString m_pygmentizeName = QStringLiteral("pygmentize");
    inline static const QString m_kSyntaxName = QStringLiteral("ksynthaxhighlighter6");
    inline static const QString m_kateSyntaxName = QStringLiteral("kate-syntax-highlighter");
    inline static QMap<QString, QStringList> m_highlightersCommands = {
        {
            m_chromaName,
            {" --list", " --style=\"nord\" --lexer=%1 --html --html-inline-styles"},
        },
        {
            m_pygmentizeName,
            {" -L styles", " -l %1 -f html -O style=nord -O noclasses=True"},
        },
#if QT_VERSION > QT_VERSION_CHECK(6, 0, 0)
        {
            m_kSyntaxName,
            {" --list-themes", " --stdin -s %1 -f html -t 'Nord' -b"},
        },
#else
        {
            m_kateSyntaxName,
            {" --list-themes", " --stdin -s %1 -f html -t 'Nord' -b"},
        },
#endif
    }; // nord style by default, will be replace by the given style if it exists

    inline static const QRegularExpression m_pygmentizeRegex = QRegularExpression(QStringLiteral("(\\* )(.+)(:)"));

    QStringList getHighlighterStyleFromCmd(const QString &highlighter) const;
    void setAvailableHighlighters();
    inline static QMap<QString, QStringList> m_availableHighlighters;
};
