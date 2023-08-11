// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#include "documentHandler.h"
#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>

DocumentHandler::DocumentHandler(QObject *parent)
    : QObject(parent)
{
}

QString DocumentHandler::readNote(const QString &path) const
{
    QFile file(path);

    QString line;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            line.append(stream.readLine() + "\n");
        }
        line.remove(line.length() - 1, 2);
    }
    file.close();
    return line;
}

void DocumentHandler::writeNote(const QString &note, const QString &path)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << note << Qt::endl;
    }
    file.close();
}

QString DocumentHandler::getCssStyle(const QString& path) const
{
    QString style;
    if (QFile::exists(path)){
        style = readNote(path);
    } else {
        style = readNote(QStringLiteral(":/KleverStyle.css"));
    }

    return style;
}
