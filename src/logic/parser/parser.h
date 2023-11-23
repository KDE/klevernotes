/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QObject>
#include <QSet>
#include <QString>
#include <qobjectdefs.h>

#include "blockLexer.h"
#include "inlineLexer.h"

class Parser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString notePath WRITE setNotePath)
    Q_PROPERTY(QVariantList headerInfo WRITE setHeaderInfo)
    Q_PROPERTY(int headerLevel READ headerLevel)
public:
    explicit Parser(QObject *parent = nullptr);

    Q_INVOKABLE QString parse(QString src);

    QString getNotePath();
    void setNotePath(QString &notePath);
    void setHeaderInfo(const QVariantList &headerInfo);
    int headerLevel();
    QPair<QString, bool> sanitizePath(QString path);

    QVector<QVariantMap> tokens;
    QMap<QString, QMap<QString, QString>> links;

    // NoteMapper
    void addToLinkedNoteInfos(const QStringList &infos);
    void addToNoteHeaders(const QString &header);

signals:
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
    QString m_mapperNotePath;
    QString m_groupPath;
    QString m_categPath;

    QVariantMap m_token;

    // NoteMapper
    QString m_header;
    int m_headerLevel;
    QStringList m_noteHeaders;
    QStringList m_previousNoteHeaders;
    QStringList m_linkedNoteInfos;
    QStringList m_previousLinkedNoteInfos;
};
