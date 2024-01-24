/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#pragma once

#include <QObject>
#include <QSet>

#include "../plugins/pluginHelper.h"
#include "blockLexer.h"
#include "inlineLexer.h"

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

    PluginHelper *getPluginHelper();

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
    PluginHelper *pluginHelper = new PluginHelper();

    QString m_notePath;
    QVariantMap m_token;
    /*
        // Synthax highlight
        bool m_newHighlightStyle = true;
        bool m_sameCodeBlocks = false;
        int m_currentBlockIndex = 0;
        QStringList m_noteCodeBlocks;
        QStringList m_previousHighlightedBlocks;
        QStringList m_previousNoteCodeBlocks;

        // NoteMapper
        QString m_mapperNotePath;
        QString m_groupPath;
        QString m_categPath;
        QString m_header;

        QString m_headerLevel;
        bool m_headerFound = false;
        bool m_emptyHeadersSent = false;

        // Valid to use QSet since, in any case, linking will be done on the first instance of a duplicated header
        QSet<QString> m_noteHeaders;
        QSet<QString> m_previousNoteHeaders;
        bool m_noteHeadersChanged = false;

        QSet<QStringList> m_linkedNotesInfos;
        QSet<QStringList> m_previousLinkedNotesInfos;
        bool m_linkedNotesChanged = false;
        bool m_notePathChanged = true;

        // Emoji support
        QString m_emojiTone = KleverConfig::emojiTone();

        // PUML
        bool m_pumlDarkChanged = true;
        bool m_samePUMLBlocks = false;
        int m_currentPUMLBlockIndex = 0;
        QStringList m_notePUMLBlocks;
        QStringList m_previousNotePUMLBlocks;
        QStringList m_previousPUMLDiag;
     */
};
