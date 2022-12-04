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
#include <QJsonArray>
#include <QList>

View::View(QObject *parent)
    : QObject(parent)
{

}

void View::print(QString path)
{
    QJsonObject truc = hierarchy(path,0);
    qDebug() << truc;
}

QJsonObject View::hierarchy(QString path, int lvl)
{
    QJsonObject final = QJsonObject();
    QFileInfo testingFile(path);
    final["name"] = QJsonValue(testingFile.fileName());
    if (final["name"] == ".BaseCategory") final["name"] = QJsonValue("Notes");
    final["path"] = QJsonValue(testingFile.filePath());
    final["lvl"] = QJsonValue(lvl);

    switch (lvl) {
        case 0:
            final["useCase"] = QJsonValue("Category");
            break;
        case 1:
            final["useCase"] = QJsonValue("Group");
            break;
        case 2:
            final["useCase"] = QJsonValue("Note");
            break;
        default:
            final["useCase"] = QJsonValue("Root");
            break;
    }

    QJsonArray content;

    if (testingFile.isDir())
    {
        QDir dir(path);

        QFileInfoList fileList = dir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries | QDir::Filter::AccessMask);

        foreach(const QFileInfo & file, fileList){
            QString nextPath = file.filePath();
            QString name = file.fileName();

            if (name == ".directory") continue;

            QJsonObject entryInfo = hierarchy(nextPath,lvl+1);
            if (name == ".BaseGroup")
            {
                QJsonArray groupContent = entryInfo["content"].toArray();

                foreach (const QJsonValue & value, groupContent) {
                    QJsonObject object = value.toObject() ;
                    object["lvl"] = QJsonValue(object["lvl"].toInt()-1);
                    content.append(QJsonValue(object));
                }
            }
            else
            {
                content.append(QJsonValue(entryInfo));
            }
        }
    }
    final["content"] = QJsonValue(content);

    return final;
}
