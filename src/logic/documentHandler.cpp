// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#include "documentHandler.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QString>
#include <QTextStream>

DocumentHandler::DocumentHandler(QObject *parent)
    : QObject(parent)
{
}

QString DocumentHandler::readFile(const QString &path) const
{
    QFile file(path);

    QString line("\n"); // The parser will still receive something even if the file is empty
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            line.append(stream.readLine() + "\n");
        }
        if (line.length() > 3)
            line.remove(line.length() - 1, 1); // Remove the last \n

        if (line.length() > 3)
            line.remove(0, 1); // Remove the first \n
    }
    file.close();
    return line;
}

void DocumentHandler::writeFile(const QString &note, const QString &path)
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
        style = readFile(path);
    } else {
        style = readFile(QStringLiteral(":/KleverStyle.css"));
    }

    return style;
}

bool DocumentHandler::checkForHeader(const QString &path, const QString &header)
{
    QFile file(path);
    if (header.isEmpty())
        return false;

    bool found = false;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            if (stream.readLine().trimmed() == header) {
                found = true;
                break;
            }
        }
    }
    file.close();
    return found;
}

// TODO use those method for the todoHandler
bool DocumentHandler::saveMap(const QJsonObject &map, const QString &path)
{
    QJsonDocument doc = QJsonDocument(map);

    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        if (file.write(doc.toJson()) < 0)
            return false;
        file.close();
        return true;
    }

    return false;
}

QJsonObject DocumentHandler::getSavedMap(const QString &mapPath)
{
    QFile file(mapPath);

    QJsonObject json;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        json = QJsonDocument::fromJson(file.readAll()).object();
        file.close();
    }
    return json;
}
