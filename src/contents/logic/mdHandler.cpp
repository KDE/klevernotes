// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "mdHandler.h"
#include <QDebug>
#include <QJsonObject>
#include <QStringList>
#include <QJsonArray>

MDHandler::MDHandler(QObject *parent)
    : QObject(parent)
{

}

QJsonArray MDHandler::getLines(QString text)
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
    for (int i = 0 ; i < lines.size() ; i++)
    {
        QString line = lines[i].toString();
        cursorLine = line;
        lineIndex = i;
        int nextTextLength = textLength+line.length();
        if (nextTextLength >= position) break;
        textLength = nextTextLength+1;
        inLineCursorPosition = position-textLength;
    }
    info.append(QString::number(lineIndex));
    info.append(cursorLine);
    info.append(QString::number(inLineCursorPosition));
    return info;
}

QJsonObject MDHandler::getInstructions(QString selectedText,QStringList charsList)
{
    QJsonObject final = QJsonObject();


    QJsonArray selectedLines = getLines(selectedText);
    final["lines"] = selectedLines;

    QJsonArray instructions;
    bool allBold = false;
    foreach(const QJsonValue & lineRef, selectedLines){
        QString line = lineRef.toString();
        instructions.append("remove");
        if (line.isEmpty()){
            instructions[instructions.size()-1] = "none";
            continue;
        }
        bool skip = false;
        foreach(const QString & chars, charsList){
            if (line.startsWith(chars) && line.endsWith(chars)){
                skip = true;
                break;
            }
        }
        if (skip) continue;
        allBold = true;
        instructions[instructions.size()-1] = "apply";
    }

    if (allBold){
        for(int i=0; i < instructions.count(); ++i ){
            if (instructions[i] == "remove"){
                instructions[i] = "none";
            }
        }
    }
    final["instructions"] = instructions;


    return final;
}

