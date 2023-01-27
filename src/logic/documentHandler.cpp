// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "documentHandler.h"
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QString>


DocumentHandler::DocumentHandler(QObject* parent)
    : QObject(parent)
{

}

QString DocumentHandler::readNote(QString path)
{
    QFile file(path);

    QString line;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream stream(&file);
        while (!stream.atEnd()){
            line.append(stream.readLine()+"\n");
        }
    }
    file.close();
    return line;
}

void DocumentHandler::writeNote(QString note, QString path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        stream << note << Qt::endl;
    }
    file.close();
}
