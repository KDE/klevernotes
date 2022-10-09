// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "view.h"
#include <QDebug>
#include <QDir>
#include <QString>
#include <QUrl>
#include <QDirIterator>
#include <QMap>
#include <QJsonObject>
#include <QStringList>

View::View(QObject *parent)
    : QObject(parent)
{

}

void View::print(QString path)
{
    QJsonObject truc = hierarchy(path);
    qDebug() << truc;
}

QJsonObject View::hierarchy(QString path)
{
    QJsonObject final = QJsonObject();
    QFileInfo testingFile(path);
    final["name"] = QJsonValue(testingFile.fileName());
    final["path"] = QJsonValue(testingFile.filePath());
    final["isFile"] = QJsonValue(testingFile.isFile());

    QJsonObject content;

    if (testingFile.isDir())
    {
        QDir dir(path);

        QFileInfoList fileList = dir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries | QDir::Filter::AccessMask);
        foreach(QFileInfo file, fileList){
            QString nextPath = file.filePath();
            QString name = file.fileName();

            if (name == ".directory") continue;

            QJsonObject entryInfo = hierarchy(nextPath);
            if (name == ".BaseGroup")
            {
                QJsonObject groupContent = entryInfo["content"].toObject();

                QJsonObject::const_iterator i;
                for (i = groupContent.constBegin(); i != groupContent.constEnd(); ++i)
                    content[i.key()] = i.value();
            }
            else
            {
                content[name] = entryInfo;
            }
        }
    }
    final["content"] = QJsonValue(content);
    // qDebug() << final;
    return final;
}
