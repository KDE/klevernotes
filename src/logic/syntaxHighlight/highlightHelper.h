// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QObject>
#include <QRegularExpression>
#include <QString>
#include <QVariantMap>

class HighlightHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap highlighters READ getHighlighters)
public:
    explicit HighlightHelper(QObject *parent = nullptr);

    QVariantMap getHighlighters();

private:
    QMap<QString, QString> m_highlightersCommands = {
        {"chroma", "chroma --list"},
        {"pygmentize", "pygmentize -L styles"},
    };

    QRegularExpression m_chromaRegex = QRegularExpression("");
    QRegularExpression m_pygmentizeRegex = QRegularExpression("(\\* )(.+)(:)");

    QStringList getHighlighterStyle(const QString &highlighter);
    void setAvailableHighlighters();
    QVariantMap m_availableHighlighters;
};
