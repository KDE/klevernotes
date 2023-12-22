// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "kleverUtility.h"
#include "documentHandler.h"
// #include <QDebug>
#include <QDir>
#include <QFontInfo>
#include <QStandardPaths>
#include <kio/global.h>
#include <klocalizedstring.h>

KleverUtility::KleverUtility(QObject *parent)
    : QObject(parent)
{
}

QString KleverUtility::getName(const QString &path) const
{
    return QDir(path).dirName();
}

QString KleverUtility::getPath(const QUrl &url) const
{
    return url.toLocalFile();
}

bool KleverUtility::exists(const QString &path)
{
    return QFile().exists(path);
}

bool KleverUtility::create(const QString &path)
{
    if (!exists(path)) {
        return QDir().mkpath(path);
    }
    return false;
}

QString KleverUtility::getImageStoragingPath(const QString &noteImagesStoringPath, const QString &wantedName, int iteration) const
{
    create(noteImagesStoringPath);

    QString imagePath = noteImagesStoringPath + wantedName;
    if (iteration != 0)
        imagePath += QStringLiteral("(") + QString::number(iteration) + QStringLiteral(")");
    imagePath += QStringLiteral(".png");

    if (exists(imagePath)) {
        return getImageStoragingPath(noteImagesStoringPath, wantedName, iteration + 1);
    }
    return imagePath;
}

bool KleverUtility::isEmptyDir(const QString &path) const
{
    return !exists(path) || QDir(path).isEmpty();
}

QString KleverUtility::isProperPath(const QString &parentPath, const QString &name) const
{
    if (name.startsWith(QStringLiteral(".")))
        return QStringLiteral("dot");

    const QString properName = KIO::encodeFileName(name);

    const QString newPath = parentPath + QStringLiteral("/") + properName;

    return (exists(newPath)) ? QStringLiteral("exist") : QString();
}

QString KleverUtility::getParentPath(const QString &path) const
{
    QDir dir(path);
    dir.cdUp();
    return dir.absolutePath();
}

bool KleverUtility::remove(const QString &path) const
{
    QFile file(path);
    return file.remove();
}

QJsonObject KleverUtility::getCssStylesList() const
{
    QJsonObject styleNameAndPath = {{QStringLiteral("KleverStyle"), QStringLiteral(":/KleverStyle.css")},
                                    {QStringLiteral("Avenir"), QStringLiteral(":/Avenir.css")},
                                    {QStringLiteral("Style7"), QStringLiteral(":/Style7.css")},
                                    {QStringLiteral("Style9"), QStringLiteral(":/Style9.css")}};

    static const QString externalStylesFolderPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + QStringLiteral("/Styles/");
    static const QDir externalStylesFolder(externalStylesFolderPath);

    if (!externalStylesFolder.exists()) {
        create(externalStylesFolderPath);
        static const QString message = i18n(
            "/*\
            \nThis file is a copy of the default CSS style for the note display.\
            \nFeel free to edit it or make your own.\
            \nNote: each style need to have a different name.\
            \n*/\n");

        static const QString defaultCss = DocumentHandler::getCssStyle(QStringLiteral(":/KleverStyle.css"));
        static const QString fileContent = message + defaultCss;
        static const QString filePath = externalStylesFolderPath + QStringLiteral("KleverStyle.css");

        DocumentHandler::writeFile(fileContent, filePath);
    }

    const QFileInfoList fileList = externalStylesFolder.entryInfoList(QDir::Filter::NoDotAndDotDot | QDir::Filter::Files);

    for (const QFileInfo &file : fileList) {
        QString name = file.fileName();

        if (!name.endsWith(QStringLiteral(".css")))
            continue;

        name.chop(4);

        if (styleNameAndPath.contains(name))
            continue;

        styleNameAndPath[name] = file.absoluteFilePath();
    }

    return styleNameAndPath;
}

QJsonObject KleverUtility::fontInfo(const QFont &font) const
{
    QJsonObject fontInfo;

    const QFontInfo info(font);

    fontInfo[QStringLiteral("family")] = info.family();
    fontInfo[QStringLiteral("pointSize")] = info.pointSize();
    return fontInfo;
}
