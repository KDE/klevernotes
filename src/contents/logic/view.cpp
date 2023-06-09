// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "view.h"
#include "kleverconfig.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>
#include <QUrl>

View::View(QObject *parent)
    : QObject(parent)
{
}

QJsonObject View::getHierarchy(QString path, int lvl)
{
    QJsonObject final = QJsonObject();
    QFileInfo testingFile(path);
    final["name"] = QJsonValue(testingFile.fileName());

    final["displayedName"] = (final["name"] == ".BaseCategory") ? QJsonValue(KleverConfig::categoryDisplayName()) : QJsonValue(final["name"]);

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

    if (testingFile.isDir()) {
        QDir dir(path);

        QFileInfoList fileList = dir.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::AllEntries | QDir::Filter::AccessMask);

        foreach (const QFileInfo &file, fileList) {
            QString nextPath = file.filePath();
            QString name = file.fileName();

            if (name == ".directory")
                continue;

            if (lvl + 1 < 3) {
                QJsonObject entryInfo = getHierarchy(nextPath, lvl + 1);
                // A switch could have been nice here but you can't do that with string ;'(
                if (name == ".BaseGroup") {
                    QJsonArray groupContent = entryInfo["content"].toArray();

                    for (int i = groupContent.size(); i-- > 0;) {
                        QJsonObject object = groupContent[i].toObject();
                        object["lvl"] = QJsonValue(object["lvl"].toInt() - 1);
                        content.insert(0, QJsonValue(object));
                    }
                } else if (name == ".BaseCategory") {
                    content.insert(0, QJsonValue(entryInfo));
                } else {
                    content.append(QJsonValue(entryInfo));
                }
            }
        }
    }

    final["content"] = QJsonValue(content);

    return final;
}

void View::hierarchySupplier(QString path, int lvl)
{
    QJsonObject hierarchy = getHierarchy(path, lvl);
    hierarchySent(hierarchy);
}
