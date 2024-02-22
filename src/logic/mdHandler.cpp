// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "mdHandler.h"
// #include <QDebug>

MDHandler::MDHandler(QObject *parent)
    : QObject(parent)
{
}

QList<int> MDHandler::getBlockLimits(const int selectionStart, const int selectionEnd, const QString &text) const
{
    const int startingIndex = selectionStart > 0 ? text.lastIndexOf(QStringLiteral("\n"), selectionStart - 1) + 1 : 0;
    const int endingIndex = selectionEnd < text.size() ? text.indexOf(QStringLiteral("\n"), selectionEnd) : selectionEnd;

    return {startingIndex, endingIndex};
}

int MDHandler::getBlockInstruction(const QString &selectedText, const QStringList &charsList) const
{
    bool apply = false;
    for (int charsIndex = 0; charsIndex < charsList.size(); charsIndex++) {
        const QString chars = charsList.at(charsIndex).trimmed();
        const QString trimmedText = selectedText.trimmed();
        apply = !(trimmedText.startsWith(chars) && trimmedText.endsWith(chars));
    }

    return apply ? Instructions::Apply : Instructions::Remove;
}

QList<int> MDHandler::getPerLineInstructions(const QStringList &lines, const QStringList &charsList, const bool checkLineEnd, const bool applyIncrement) const
{
    QList<int> instructions;
    bool applyToAll = false;
    int counter = 1;
    for (int lineIndex = 0; lineIndex < lines.size(); lineIndex++) {
        const QString line = lines.at(lineIndex);

        instructions.append(Instructions::Remove);

        if (line.isEmpty() && lines.size() > 1) {
            instructions.last() = Instructions::None;
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
                if (checkLineEnd && !line.endsWith(chars)) {
                    skip = false;
                }
                break;
            }
        }

        if (skip) {
            continue;
        }
        applyToAll = true;
        instructions.last() = Instructions::Apply;
    }

    if (applyToAll) {
        for (int i = 0; i < instructions.count(); i++) {
            if (instructions[i] == Instructions::Remove) {
                instructions[i] = Instructions::None;
            }
        }
    }

    return instructions;
}

QString MDHandler::getNewText(const QString &baseText, const QStringList &charsList, bool multiPlaceApply, bool applyIncrement, bool checkByBlock) const
{
    const QString choosenSpecialChars = charsList[0];
    QString toReturn;
    static const QString lineBr = QStringLiteral("\n");
    if (checkByBlock) {
        const int choosenCharsSize = choosenSpecialChars.size();
        int instruction = getBlockInstruction(baseText, charsList);
        if (instruction == Instructions::Apply) {
            toReturn = choosenSpecialChars + baseText + choosenSpecialChars;
        } else {
            const int end = baseText.size() - choosenCharsSize * 2;
            toReturn = baseText.mid(choosenCharsSize, end);
        }
    } else {
        const QStringList lines = baseText.split(lineBr);
        QList<int> instructions = getPerLineInstructions(lines, charsList, multiPlaceApply, applyIncrement);

        // Currently only used for ordered list
        int nonEmptyStrNumber = 0;
        for (const QString &line : lines) {
            if (line.trimmed().length() > 0) {
                nonEmptyStrNumber++;
            }
        }

        const bool hasNonEmptyStrings = nonEmptyStrNumber > 0;
        int counter = 1;

        const int linesCount = lines.length();
        for (int i = 0; i < linesCount; i++) {
            QString appliedSpecialChars = choosenSpecialChars;

            QString line = lines[i];
            const int instruction = instructions[i];

            // Currently only used for ordered list
            if (line.trimmed().length() == 0 && hasNonEmptyStrings) {
                continue;
            }
            if (applyIncrement) {
                appliedSpecialChars = QString::number(counter++) + choosenSpecialChars;
            }

            switch (instruction) {
            case Instructions::Apply: {
                if (multiPlaceApply) {
                    line += appliedSpecialChars;
                }
                line = appliedSpecialChars + line;

                break;
            }
            case Instructions::Remove: {
                const int specialCharsSize = appliedSpecialChars.size();
                int end = line.size() - specialCharsSize;
                if (multiPlaceApply) {
                    end -= specialCharsSize;
                }

                line = line.mid(specialCharsSize, end);
                break;
            }
            default:
                break;
            }
            toReturn += line;
            if (i != linesCount - 1) {
                toReturn += lineBr;
            }
        }
    }

    return toReturn;
}
