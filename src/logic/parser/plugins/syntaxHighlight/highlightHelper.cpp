// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#include "highlightHelper.h"
#include "../cliHelper.h"
#include "kleverconfig.h"
// #include <QDebug>

HighlightHelper::HighlightHelper(QObject *parent)
    : QObject(parent)
{
    setAvailableHighlighters();
}

QString HighlightHelper::getHighlightedString(const QString &inputStr, const QString &lang)
{
    const QString highlighter = KleverConfig::codeSynthaxHighlighter();
    const QString style = KleverConfig::codeSynthaxHighlighterStyle();

    if (inputStr.isEmpty() || lang.isEmpty() || !m_availableHighlighters.contains(highlighter)) {
        return inputStr;
    }

    QString cmd = highlighter + m_highlightersCommands[highlighter].last();
    static const QRegularExpression nord = QRegularExpression(QStringLiteral("[Nn]ord"));
    cmd.replace(QStringLiteral("%1"), lang);
    if (m_availableHighlighters[highlighter].contains(style)) {
        cmd.replace(nord, style);
    }

    const QString echo = QStringLiteral("echo \"") + inputStr + QStringLiteral("\" | ");

    cmd = echo + cmd;

    QString output = CLIHelper::execCommand(cmd);

    if (output.isEmpty()) {
        return inputStr;
    }

    const bool isChroma = highlighter == m_chromaName;
    const bool isPygmentizeOrKSyntaxHighlither = highlighter == m_pygmentizeName || highlighter == m_kSyntaxName || highlighter == m_kateSyntaxName;

    int startIndex = -1;
    int endIndex = -1;
    if (isChroma) {
        static const auto codeBlockStart = QStringLiteral("<code>");
        startIndex = output.indexOf(codeBlockStart) + codeBlockStart.length();
        endIndex = output.lastIndexOf(QStringLiteral("</code>"));
    } else if (isPygmentizeOrKSyntaxHighlither) {
        startIndex = output.indexOf(QStringLiteral("<span"));
        endIndex = output.lastIndexOf(QStringLiteral("\n</pre>"));
    }

    const bool correctIndexes = startIndex < endIndex && -1 < startIndex && -1 < endIndex;

    output = correctIndexes ? output.mid(startIndex, endIndex - startIndex) : QString();

    return output.isEmpty() ? inputStr : output;
}

QStringList HighlightHelper::getHighlighters() const
{
    return m_availableHighlighters.keys();
}

QStringList HighlightHelper::getHighlighterStyle(const QString &highlighter) const
{
    return m_availableHighlighters.contains(highlighter) ? m_availableHighlighters[highlighter] : QStringList();
}

QStringList HighlightHelper::getHighlighterStyleFromCmd(const QString &highlighter) const
{
    QStringList styles;

    const QString cmd = highlighter + m_highlightersCommands[highlighter].constFirst();
    const QString output = CLIHelper::execCommand(cmd);

    if (output.isEmpty()) {
        return styles;
    }

    if (highlighter == m_pygmentizeName) {
        auto it = m_pygmentizeRegex.globalMatch(output);

        while (it.hasNext()) {
            const QRegularExpressionMatch match = it.next();
            styles.append(match.captured(2));
        }
    } else if (highlighter == m_chromaName) {
        static const auto styleStart = QStringLiteral("\nstyles: ");
        const int startIndex = output.lastIndexOf(styleStart) + styleStart.length();
        const int endIndex = output.lastIndexOf(QStringLiteral("\nformatters: "));

        styles = output.mid(startIndex, endIndex - startIndex).split(QStringLiteral(" "));
    } else if (highlighter == m_kSyntaxName || highlighter == m_kateSyntaxName) {
        styles = output.split(QStringLiteral("\n"));
        styles.takeLast(); // there's a blank line at the end
    }

    styles.sort();
    return styles;
}

void HighlightHelper::setAvailableHighlighters()
{
    for (auto it = m_highlightersCommands.cbegin(); it != m_highlightersCommands.cend(); it++) {
        if (CLIHelper::commandExists(it.key())) {
            m_availableHighlighters[it.key()] = getHighlighterStyleFromCmd(it.key());
        }
    }
}
