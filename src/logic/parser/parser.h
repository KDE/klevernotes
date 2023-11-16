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
public:
    explicit Parser(QObject *parent = nullptr);

    Q_INVOKABLE QString parse(QString src);

    QString getNotePath();
    void setNotePath(QString &notePath);
    QPair<QString, bool> sanitizePath(QString path);

    QVector<QVariantMap> tokens;
    QMap<QString, QMap<QString, QString>> links;
    QStringList linkedNoteInfos;

signals:
    void newLinkedNotesPaths(const QStringList &notePathHeaderPairs);

private:
    QString tok();
    QString parseText();
    QString peekType();
    bool getNextToken();

    BlockLexer blockLexer = BlockLexer(this);
    InlineLexer inlineLexer = InlineLexer(this);

    QString m_notePath;
    QString m_groupPath;
    QString m_categPath;

    QVariantMap m_token;
};
