// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#include "highlightHelper.h"
#include "kleverconfig.h"
// #include <QDebug>
#include <QProcess>

QString findExecutable(const QString &command)
{
    return QStandardPaths::findExecutable(command);
}

QString execCommand(const QString &input)
{
    QProcess process;

    const QString sh = findExecutable("sh");
    if (sh.isEmpty()) {
        return {};
    }

    // We let sh handle the pipes and all the args, easier than using multiple QProcess
    process.start(sh, QStringList() << "-c" << input);

    if (!process.waitForStarted()) {
        return {};
    }

    process.waitForFinished(5000); // if it takes more then 5 secs, there's a problem !

    return process.exitCode() == 0 ? process.readAllStandardOutput() : QString();
}

bool commandExists(const QString &command)
{
    return !findExecutable(command).isEmpty();
}

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
    static const QRegularExpression nord = QRegularExpression("[Nn]ord");
    cmd.replace("%1", lang);
    if (m_availableHighlighters[highlighter].contains(style)) {
        cmd.replace(nord, style);
    }

    const QString echo = "echo \"" + inputStr + "\" | ";

    cmd = echo + cmd;

    QString output = execCommand(cmd);

    if (output.isEmpty()) {
        return inputStr;
    }

    const bool isChroma = highlighter == m_chromaName;
    const bool isPygmentizeOrKSyntaxHighlither = highlighter == m_pygmentizeName || highlighter == m_kSyntaxName || highlighter == m_kateSyntaxName;

    int startIndex = -1;
    int endIndex = -1;
    if (isChroma) {
        static const auto codeBlockStart = QStringLiteral("<code>");
        startIndex = output.indexOf("<code>") + codeBlockStart.length();
        endIndex = output.lastIndexOf("</code>");
    } else if (isPygmentizeOrKSyntaxHighlither) {
        startIndex = output.indexOf("<span");
        endIndex = output.lastIndexOf("\n</pre>");
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
    const QString output = execCommand(cmd);

    if (output.isEmpty()) {
        return styles;
    }

    if (highlighter == m_pygmentizeName) {
        auto it = m_pygmentizeRegex.globalMatch(output);

        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            styles.append(match.captured(2));
        }
    } else if (highlighter == m_chromaName) {
        static const auto styleStart = QStringLiteral("\nstyles: ");
        const int startIndex = output.lastIndexOf(styleStart) + styleStart.length();
        const int endIndex = output.lastIndexOf("\nformatters: ");

        styles = output.mid(startIndex, endIndex - startIndex).split(" ");
    } else if (highlighter == m_kSyntaxName || highlighter == m_kateSyntaxName) {
        styles = output.split("\n");
        styles.takeLast(); // there's a blank line at the end
    }

    styles.sort();
    return styles;
}

void HighlightHelper::setAvailableHighlighters()
{
    for (auto it = m_highlightersCommands.cbegin(); it != m_highlightersCommands.cend(); it++) {
        if (commandExists(it.key())) {
            m_availableHighlighters[it.key()] = getHighlighterStyleFromCmd(it.key());
        }
    }
}
