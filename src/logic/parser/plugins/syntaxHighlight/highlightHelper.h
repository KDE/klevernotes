// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QMap>
#include <QObject>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QStandardPaths>

/**
 * @class HighlightHelper
 * @brief Helper class to interact with external code highlighters.
 *
 */
class HighlightHelper : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QStringList highlighters READ getHighlighters CONSTANT)
public:
    explicit HighlightHelper(QObject *parent = nullptr);

    /**
     * @brief Get the list of the available highlighters.
     *
     * @return A list of the different highlighters names.
     */
    QStringList getHighlighters() const;

    /**
     * @brief Get the list of all available styles for the given highlighter.
     *
     * @param highlighter The highlighter for which we want to know the name of the themes.
     * @return A list of all the styles names.
     */
    Q_INVOKABLE QStringList getHighlighterStyle(const QString &highlighter) const;

    /**
     * @brief Get an highlighted string based on the given lang and the previously chosen highlighter.
     *
     * @param inputStr The string to be highlighted.
     * @param lang The language of the code to be highlighted.
     * @return The highlighted input string in form of HTML.
     */
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

    /**
     * @brief Get the list of all available styles for the given highlighter using the command line.
     *
     * @param highlighter The highlighter for which we want to know the name of the themes.
     * @return A list of all the styles names.
     */
    QStringList getHighlighterStyleFromCmd(const QString &highlighter) const;

    /**
     * @brief Set the list of available highlighters after checking.
     */
    void setAvailableHighlighters();
    inline static QMap<QString, QStringList> m_availableHighlighters;
};
