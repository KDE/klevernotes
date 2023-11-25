/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QObject>
#include <QSet>
#include <QString>

#include "blockLexer.h"
#include "inlineLexer.h"

class Parser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString notePath WRITE setNotePath)
    Q_PROPERTY(QStringList headerInfo WRITE setHeaderInfo)
    Q_PROPERTY(QString headerLevel READ headerLevel)
    Q_PROPERTY(bool noteMapEnabled WRITE setNoteMapEnabled) // QML will handle the signal and change it for us
public:
    explicit Parser(QObject *parent = nullptr);

    Q_INVOKABLE QString parse(QString src);

    QString getNotePath();
    void setNotePath(QString &notePath);
    QVector<QVariantMap> tokens;
    QMap<QString, QMap<QString, QString>> links;

    // NoteMapper
    void setNoteMapEnabled(const bool noteMapEnabled);
    bool noteMapEnabled();
    void setHeaderInfo(const QStringList &headerInfo);
    QString headerLevel();
    QPair<QString, bool> sanitizePath(QString path);
    void addToLinkedNoteInfos(const QStringList &infos);
    void addToNoteHeaders(const QString &header);

signals:
    // NoteMapper
    void newLinkedNotesPaths(const QStringList &notePathHeaderPairs);
    void noteHeadersSent(const QString notePath, const QStringList &noteHeaders);

private:
    QString tok();
    QString parseText();
    QString peekType();
    bool getNextToken();

    BlockLexer blockLexer = BlockLexer(this);
    InlineLexer inlineLexer = InlineLexer(this);

    QString m_notePath;
    QVariantMap m_token;

    // NoteMapper
    bool m_noteMapEnabled;
    QString m_mapperNotePath;
    QString m_groupPath;
    QString m_categPath;
    QString m_header;
    QString m_headerLevel;
    bool m_headerFound;
    bool m_emptyHeadersSent;
    QStringList m_noteHeaders;
    QStringList m_previousNoteHeaders;
    QStringList m_linkedNoteInfos;
    QStringList m_previousLinkedNoteInfos;
};
