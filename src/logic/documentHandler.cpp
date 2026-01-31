// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023-2026 Louis Schul <schul9louis@gmail.com>

#include "documentHandler.h"

// Qt includes
#include <QFile>
#include <QJsonDocument>
#include <QTextStream>

DocumentHandler::DocumentHandler(QObject *parent)
    : QObject(parent)
{
}

QString DocumentHandler::readFile(const QString &path)
{
    QFile file(path);

    QString line = {};
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        while (!stream.atEnd()) {
            line.append(stream.readLine() + QStringLiteral("\n"));
        }
        line.removeLast();
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

QString DocumentHandler::getCssStyle(const QString &path)
{
    QString style;
    if (QFile::exists(path)) {
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

bool DocumentHandler::saveJson(const QJsonObject &json, const QString &path)
{
    QJsonDocument doc = QJsonDocument(json);

    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        if (file.write(doc.toJson()) < 0)
            return false;
        file.close();
        return true;
    }

    return false;
}

QJsonObject DocumentHandler::getJson(const QString &jsonPath)
{
    QFile file(jsonPath);

    QJsonObject json;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        json = QJsonDocument::fromJson(file.readAll()).object();
        file.close();
    }
    return json;
}
