/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#pragma once

#include <QObject>
#include <QSet>

#include "blockLexer.h"
#include "inlineLexer.h"
#include "plugins/pluginHelper.h"

class Parser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString notePath WRITE setNotePath)
    // NoteMapper
    Q_PROPERTY(QStringList headerInfo WRITE setHeaderInfo)
    Q_PROPERTY(QString headerLevel READ headerLevel CONSTANT)
public:
    explicit Parser(QObject *parent = nullptr);

    Q_INVOKABLE QString parse(QString src);

    QString getNotePath() const;
    void setNotePath(const QString &notePath);
    QVector<QVariantMap> tokens;
    QMap<QString, QMap<QString, QString>> links;

    PluginHelper *getPluginHelper() const;

    // NoteMapper
    void setHeaderInfo(const QStringList &headerInfo);
    QString headerLevel() const;

Q_SIGNALS:
    // NoteMapper
    void newLinkedNotesInfos(const QSet<QStringList> &linkedNotesInfos);
    void noteHeadersSent(const QString &notePath, const QStringList &noteHeaders);

public Q_SLOTS:
    // Syntax highlight
    void newHighlightStyle();
    // PUML
    void pumlDarkChanged();

private:
    QString tok();
    QString parseText();
    QString peekType() const;
    bool getNextToken();

    BlockLexer blockLexer = BlockLexer(this);
    InlineLexer inlineLexer = InlineLexer(this);
    PluginHelper *pluginHelper = new PluginHelper(this);

    QString m_notePath;
    QVariantMap m_token;
};
