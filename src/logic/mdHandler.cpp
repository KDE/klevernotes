// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "mdHandler.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>

MDHandler::MDHandler(QObject *parent)
    : QObject(parent)
{
}

QJsonArray MDHandler::getLines(const QString &text) const
{
    return QJsonArray::fromStringList(text.split('\n'));
}

QStringList MDHandler::getPositionLineInfo(QJsonArray lines, int position)
{
    QStringList info;

    int textLength = 0;

    int lineIndex = 0;
    QString cursorLine = lines[0].toString();
    int inLineCursorPosition = position;
    for (int i = 0; i < lines.size(); i++) {
        QString line = lines[i].toString();
        cursorLine = line;
        lineIndex = i;
        int nextTextLength = textLength + line.length();
        if (nextTextLength >= position)
            break;
        textLength = nextTextLength + 1;
        inLineCursorPosition = position - textLength;
    }
    info.append(QString::number(lineIndex));
    info.append(cursorLine);
    info.append(QString::number(inLineCursorPosition));
    return info;
}

QJsonObject MDHandler::getInstructions(const QString &selectedText, const QStringList &charsList, const bool checkLineEnd, const bool applyIncrement) const
{
    QJsonObject final = QJsonObject();

    QJsonArray selectedLines = getLines(selectedText);

    final["lines"] = selectedLines;

    QJsonArray instructions;
    bool applyToAll = false;
    int counter = 1;
    for (int lineIndex = 0; lineIndex < selectedLines.size(); lineIndex++) {
        const QString line = selectedLines.at(lineIndex).toString();

        instructions.append("remove");
        if (line.isEmpty() && selectedLines.size() > 1) {
            instructions[instructions.size() - 1] = "none";
            continue;
        }

        bool skip = false;

        for (int charsIndex = 0; charsIndex < charsList.size(); charsIndex++) {
            QString chars = charsList.at(charsIndex);
            if (applyIncrement) {
                chars = QString::number(counter) + chars;
                counter++;
            }

            if (line.startsWith(chars)) {
                skip = true;
                if (checkLineEnd && !line.endsWith(chars))
                    skip = false;
                break;
            }
        }

        if (skip)
            continue;
        applyToAll = true;
        instructions[instructions.size() - 1] = "apply";
    }

    if (applyToAll) {
        for (int i = 0; i < instructions.count(); ++i) {
            if (instructions[i] == "remove") {
                instructions[i] = "none";
            }
        }
    }
    final["instructions"] = instructions;

    return final;
}
