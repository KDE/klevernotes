/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QObject>
#include <QString>

#include "blockLexer.h"
#include "inlineLexer.h"
#include "logic/noteMapper.h"

class Parser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString notePath WRITE setNotePath)
public:
    explicit Parser(QObject *parent = nullptr);

    Q_INVOKABLE QString parse(QString src);

    QString getNotePath();
    void setNotePath(QString &notePath);
    QString sanitizePath(QString &path);

    QVector<QVariantMap> tokens;
    QMap<QString, QMap<QString, QString>> links;
    QSet<QString> linkedNotesPaths;

signals:
    void newLinkedNotesPaths(const QStringList &linkedNotesPaths);

private:
    QString tok();
    QString parseText();
    QString peekType();
    bool getNextToken();

    BlockLexer blockLexer = BlockLexer(this);
    InlineLexer inlineLexer = InlineLexer(this);
    NoteMapper *m_noteMapper;

    QString m_notePath;
    QString m_groupPath;
    QString m_categPath;

    QVariantMap m_token;
};
