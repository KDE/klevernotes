// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#include "highlightHelper.h"
#include "kleverconfig.h"
#include <QDebug>
#include <array>
#include <string>

std::string execCommand(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        qDebug() << "Could not run the command" << cmd;
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

bool commandExists(const QString &command)
{
    return !QString::fromStdString(execCommand(("command -v " + command.toStdString()).c_str())).isEmpty();
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

    QString cmd = m_highlightersCommands[highlighter].last();
    cmd.replace("%1", lang);
    if (m_availableHighlighters[highlighter].contains(style)) {
        cmd.replace("nord", style);
    }

    m_documentHandler->writeFile(inputStr, m_tempInputFilePath); // could be really cool to get ride of this part !
    QString output = QString::fromStdString(execCommand(cmd.toStdString().c_str()));

    if (highlighter == QStringLiteral("chroma") && !output.isEmpty()) {
        const int startIndex = output.indexOf("<code>") + 6;
        const int endIndex = output.lastIndexOf("</code>");
        output = output.mid(startIndex, endIndex - startIndex);
    }

    return output.isEmpty() ? inputStr : output;
}

QStringList HighlightHelper::getHighlighters()
{
    return m_availableHighlighters.keys();
}

QStringList HighlightHelper::getHighlighterStyle(const QString &highlighter)
{
    return m_availableHighlighters.contains(highlighter) ? m_availableHighlighters[highlighter] : QStringList();
}

QStringList HighlightHelper::getHighlighterStyleFromCmd(const QString &highlighter)
{
    QStringList styles;

    const QString cmd = m_highlightersCommands[highlighter].constFirst();
    const QString output = QString::fromStdString(execCommand(cmd.toStdString().c_str()));

    if (output.isEmpty()) {
        return styles;
    }

    if (highlighter == QStringLiteral("pygmentize")) {
        auto it = m_pygmentizeRegex.globalMatch(output);

        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            styles.append(match.captured(2));
        }
    }

    if (highlighter == QStringLiteral("chroma")) {
        const int startIndex = output.lastIndexOf("\nstyles: ") + 9;
        const int endIndex = output.lastIndexOf("\nformatters: ");

        styles = output.mid(startIndex, endIndex - startIndex).split(" ");
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
