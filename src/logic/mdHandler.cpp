// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "mdHandler.h"
#include <QRegularExpression>
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

QString MDHandler::getTabbedLine(const QString &line, const QString &givenChar, const int goalCharsRep, const bool remove) const
{
    const QString regexStr = QStringLiteral("^[") + givenChar + QStringLiteral("]*"); // puts it inside [] to avoid the need to escape \t
    const QRegularExpression reg = QRegularExpression(regexStr);
    const QRegularExpressionMatch match = reg.match(line);

    int charRep = goalCharsRep;
    if (match.hasMatch()) {
        const int givenCharsRep = match.capturedLength();
        const bool isMultiple = (givenCharsRep % goalCharsRep) == 0;
        if (!isMultiple) {
            const int closestUpperMultiple = ((givenCharsRep - 1) | (goalCharsRep - 1)) + 1;
            // closest lower multiple, or closest upper multiple
            const int closestMultiple = remove ? std::max(0, closestUpperMultiple - goalCharsRep) : closestUpperMultiple;
            charRep = remove ? givenCharsRep - closestMultiple : closestMultiple - givenCharsRep;
        } else if (remove && givenCharsRep == 0) { // There's nothing to remove at the beginning of the line
            charRep = 0;
        }
    }

    QString newLine = line;
    if (remove) {
        newLine.remove(0, charRep);
    } else {
        const QString newChars = givenChar.repeated(charRep);
        newLine.prepend(newChars);
    }

    return newLine;
}

QString MDHandler::getTextByLine(const QStringList &lines,
                                 const QStringList &charsList,
                                 const bool multiPlaceApply,
                                 const bool applyIncrement,
                                 const int goalCharsRep,
                                 const int forcedInstruction) const
{
    QString toReturn;
    static const QString lineBr = QStringLiteral("\n");
    const QString choosenSpecialChars = charsList[0];
    QList<int> instructions;
    if (!forcedInstruction) {
        instructions = getPerLineInstructions(lines, charsList, multiPlaceApply, applyIncrement);
    }

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
        QString line = lines[i];
        const int instruction = forcedInstruction ? forcedInstruction : instructions[i];

        if (!line.trimmed().isEmpty() || !hasNonEmptyStrings) {
            const QString appliedSpecialChars = applyIncrement ? QString::number(counter++) + choosenSpecialChars : choosenSpecialChars;

            switch (instruction) {
            case Instructions::Apply: {
                if (goalCharsRep) {
                    line = getTabbedLine(line, appliedSpecialChars, goalCharsRep, false);
                    break;
                }
                if (multiPlaceApply) {
                    line.append(appliedSpecialChars);
                }
                line.prepend(appliedSpecialChars);
                break;
            }
            case Instructions::Remove: {
                if (goalCharsRep) {
                    line = getTabbedLine(line, appliedSpecialChars, goalCharsRep, true);
                    break;
                }
                const int specialCharsSize = appliedSpecialChars.size();
                int sliceEnd = line.size() - specialCharsSize;
                if (multiPlaceApply) {
                    sliceEnd -= specialCharsSize;
                }
                line = line.sliced(specialCharsSize, sliceEnd);
                break;
            }
            default:
                break;
            }
        }

        toReturn.append(line);
        if (i != linesCount - 1) {
            toReturn.append(lineBr);
        }
    }

    return toReturn;
}

QString MDHandler::getTextByBlock(const QString &baseText, const QStringList &charsList) const
{
    const QString choosenSpecialChars = charsList[0];
    const int choosenCharsSize = choosenSpecialChars.size();
    const int instruction = getBlockInstruction(baseText, charsList);
    if (instruction == Instructions::Apply) {
        return choosenSpecialChars + baseText + choosenSpecialChars;
    }
    const int end = baseText.size() - choosenCharsSize * 2;
    return baseText.sliced(choosenCharsSize, end);
}

QString MDHandler::getNewText(const QString &baseText,
                              const QStringList &charsList,
                              const bool multiPlaceApply,
                              const bool applyIncrement,
                              const bool checkByBlock,
                              const int goalCharsRep,
                              const int forcedInstruction) const
{
    if (checkByBlock) {
        return getTextByBlock(baseText, charsList);
    }
    const QStringList lines = baseText.split(QStringLiteral("\n"));
    return getTextByLine(lines, charsList, multiPlaceApply, applyIncrement, goalCharsRep, forcedInstruction);
}

QString MDHandler::getLineFromPrevious(const QString &previousLine) const
{
    static const QString regexStr = QStringLiteral("^([ \t]*)([\\*\\+\\-] )*(\\d+\\. )*");

    static const QRegularExpression reg = QRegularExpression(regexStr);

    const QRegularExpressionMatch match = reg.match(previousLine);

    QString newLine = QStringLiteral("\n");
    if (match.hasMatch()) {
        const QString tab = match.captured(1);
        newLine.append(tab);

        const QString unorderedList = match.captured(2);
        const QString orderedList = match.captured(3);
        if (!unorderedList.isEmpty()) {
            newLine.append(unorderedList);
        } else if (!orderedList.isEmpty()) {
            const int orderedLength = orderedList.length();
            const QString currentNumberStr = orderedList.sliced(0, orderedLength - 2);
            bool ok;
            int nextVal = currentNumberStr.toInt(&ok, 10);

            if (ok) {
                const QString nextValStr = QString::number(++nextVal) + QStringLiteral(". ");
                newLine.append(nextValStr);
            }
        }
    }

    return newLine;
}
