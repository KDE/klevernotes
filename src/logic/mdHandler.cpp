// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "mdHandler.h"
// #include <QDebug>

MDHandler::MDHandler(QObject *parent)
    : QObject(parent)
{
}

QJsonObject MDHandler::blockChecker(const QString &selectedText, const QStringList &charsList) const
{
    QJsonObject final = QJsonObject();
    bool apply = false;
    for (int charsIndex = 0; charsIndex < charsList.size(); charsIndex++) {
        const QString chars = charsList.at(charsIndex).trimmed();
        const QString trimmedText = selectedText.trimmed();
        apply = !(trimmedText.startsWith(chars) && trimmedText.endsWith(chars));
    }

    final[QStringLiteral("instructions")] = apply ? QJsonValue(QStringLiteral("apply")) : QJsonValue(QStringLiteral("remove"));

    return final;
}

QJsonObject MDHandler::getInstructions(const QString& selectedText, const QStringList& charsList, const bool checkLineEnd, const bool applyIncrement, const bool checkByBlock) const
{
    if (checkByBlock) {
        return blockChecker(selectedText, charsList);
    }

    QJsonObject final = QJsonObject();
    const QJsonArray selectedLines = QJsonArray::fromStringList(selectedText.split(QStringLiteral("\n")));

    final[QStringLiteral("lines")] = selectedLines;
    final[QStringLiteral("applyBlock")] = false;

    QJsonArray instructions;
    bool applyToAll = false;
    int counter = 1;
    for (int lineIndex = 0; lineIndex < selectedLines.size(); lineIndex++) {
        const QString line = selectedLines.at(lineIndex).toString();

        instructions.append(QStringLiteral("remove"));
        if (line.isEmpty() && selectedLines.size() > 1) {
            instructions[instructions.size() - 1] = QStringLiteral("none");
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
        instructions[instructions.size() - 1] = QStringLiteral("apply");
    }

    if (applyToAll) {
        for (int i = 0; i < instructions.count(); ++i) {
            if (instructions[i] == QStringLiteral("remove")) {
                instructions[i] = QStringLiteral("none");
            }
        }
    }
    final[QStringLiteral("instructions")] = instructions;

    return final;
}
