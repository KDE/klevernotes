// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "logic/documentHandler.h"
#include <QMap>
#include <QObject>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QString>
#include <QVariantMap>
#include <qstandardpaths.h>

class HighlightHelper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList highlighters READ getHighlighters NOTIFY neverChangingProperty)
public:
    explicit HighlightHelper(QObject *parent = nullptr);

    QStringList getHighlighters();
    Q_INVOKABLE QStringList getHighlighterStyle(const QString &highlighter);
    static QString getHighlightedString(const QString &inputStr, const QString &lang);

signals:
    void neverChangingProperty();

private:
    inline static QString m_tempInputFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/KleverNotesSyntaxHighlightInput.txt";
    inline static QString m_tempOutputFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/KleverNotesSyntaxHighlightOutput.txt";
    inline static QMap<QString, QStringList> m_highlightersCommands = {
        {
            "chroma",
            {"chroma --list", "chroma --style=\"nord\" --lexer=%1 --html --html-inline-styles " + m_tempInputFilePath},
        },
        {
            "pygmentize",
            {"pygmentize -L styles", "pygmentize -l %1 -f html -O style=nord -O noclasses=True " + m_tempInputFilePath},
        },
    }; // nord style by default, will be replace by the given style if it exists

    QRegularExpression m_chromaRegex = QRegularExpression("");
    QRegularExpression m_pygmentizeRegex = QRegularExpression("(\\* )(.+)(:)");

    QStringList getHighlighterStyleFromCmd(const QString &highlighter);
    void setAvailableHighlighters();
    inline static QMap<QString, QStringList> m_availableHighlighters;

    inline static DocumentHandler *m_documentHandler = new DocumentHandler;
};
