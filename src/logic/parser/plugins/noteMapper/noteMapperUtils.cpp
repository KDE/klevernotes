/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#include "noteMapperUtils.h"
// #include <QDebug>

NoteMapperUtils::NoteMapperUtils(QObject *parent)
    : QObject(parent)
{
}

QString NoteMapperUtils::cleanHeader(const QString &_header)
{
    // Note: This receive a trimmed string
    if (_header.isEmpty())
        return _header;

    static const QRegularExpression m_heading_checker = QRegularExpression(QStringLiteral("^(#*)( *)(.*)"));
    const auto match = m_heading_checker.match(_header);

    const QString cap1 = match.captured(1);
    const QString cap2 = match.captured(2);
    const QString cap3 = match.captured(3);

    QString header(_header);
    static const QString space = QStringLiteral(" ");
    static const QString hashtag = QStringLiteral("#");
    if (cap1.isEmpty()) { // No # in front
        header = hashtag + space + header;
        return header;
    }

    const int level = cap1.length();
    if (level > 6) { // To much '#': ########## header => ###### #### header
        header = hashtag.repeated(6) + space + header.remove(0, 6);
        return header;
    }

    if (cap3.isEmpty())
        return {}; // All we have is a header level <= 6, nothing to really work with

    if (cap2.isEmpty()) { // Just need a space
        header = cap1 + space + cap3;
        return header;
    }

    return _header; // Everything is fine with it
}

int NoteMapperUtils::headerLevel(const QString &header)
{
    static const QRegularExpression m_heading_level = QRegularExpression(QStringLiteral("^(#{1,6}) *"));
    const auto match = m_heading_level.match(header); // We receive a correctly formed header; max of 6 '#'

    if (match.captured(1).isEmpty()) {
        return InvalidHeaderLevel;
    }

    return match.captured(1).length();
}

QString NoteMapperUtils::headerText(const QString &header)
{
    static const QRegularExpression m_heading_text = QRegularExpression(QStringLiteral("^(#{1,6})( +)(.+)"));
    const auto match = m_heading_text.match(header);

    if (!match.hasMatch()) {
        return {};
    }

    return match.captured(3);
}

QVariantMap NoteMapperUtils::convertSavedMap(const QJsonObject &savedMap)
{
    return savedMap.toVariantMap();
}

bool NoteMapperUtils::entirelyChecked(const QVariantMap &pathInfo)
{
    static const QString entirelyCheckStr = QStringLiteral("entirelyCheck");

    return pathInfo.contains(entirelyCheckStr) ? pathInfo[entirelyCheckStr].toBool() : false;
}

QStringList NoteMapperUtils::getNoteHeaders(const QVariantMap &pathInfo)
{
    static const QStringList emptyList;
    static const QString headersStr = QStringLiteral("headers");
    return pathInfo.contains(headersStr) ? pathInfo[headersStr].toStringList() : emptyList;
}

QList<QVariantMap> NoteMapperUtils::getHeadersComboList(const QStringList &headers)
{
    QList<QVariantMap> headersComboList;
    for (auto &fullHeader : headers) {
        const QString level = QString::number(headerLevel(fullHeader));
        const QString header = headerText(fullHeader);
        const QString text = level + QStringLiteral(": ") + header;

        static const QString textStr = QStringLiteral("text");
        static const QString valueStr = QStringLiteral("value");

        const QVariantMap listElem = {{textStr, text}, {valueStr, fullHeader}};
        headersComboList.append(listElem);
    }
    return headersComboList;
}
