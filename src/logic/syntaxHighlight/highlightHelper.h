// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QMap>
#include <QObject>
#include <QRegularExpression>
#include <QString>
#include <QVariantMap>

class HighlightHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList highlighters READ getHighlighters NOTIFY neverChangingProperty)
public:
    explicit HighlightHelper(QObject *parent = nullptr);

    QStringList getHighlighters();
    Q_INVOKABLE QStringList getHighlighterStyle(const QString &highlighter);

signals:
    void neverChangingProperty();

private:
    QMap<QString, QStringList> m_highlightersCommands = {
        {"chroma", {"chroma --list", "chroma --style=\"%1\" --lexer=%2 --html --html-inline-styles input.txt"}},
        {"pygmentize", {"pygmentize -L styles", "pygmentize -l %1 -f html -O style=%2 -O noclasses=True input.txt"}},
    };

    QRegularExpression m_chromaRegex = QRegularExpression("");
    QRegularExpression m_pygmentizeRegex = QRegularExpression("(\\* )(.+)(:)");

    QStringList getHighlighterStyleFromCmd(const QString &highlighter);
    void setAvailableHighlighters();
    QMap<QString, QStringList> m_availableHighlighters;
};
