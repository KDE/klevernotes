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
#include "kleverconfig.h"

class Parser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString notePath WRITE setNotePath)
    // Syntax highlight
    Q_PROPERTY(bool highlightEnabled WRITE setHighlightEnabled)
    // NoteMapper
    Q_PROPERTY(QStringList headerInfo WRITE setHeaderInfo)
    Q_PROPERTY(QString headerLevel READ headerLevel CONSTANT)
    Q_PROPERTY(bool noteMapEnabled WRITE setNoteMapEnabled) // QML will handle the signal and change it for us
    // Emoji
    Q_PROPERTY(bool emojiEnabled WRITE setEmojiEnabled) // QML will handle the signal and change it for us
public:
    explicit Parser(QObject *parent = nullptr);

    Q_INVOKABLE QString parse(QString src);

    QString getNotePath() const;
    void setNotePath(const QString &notePath);
    QVector<QVariantMap> tokens;
    QMap<QString, QMap<QString, QString>> links;

    // NoteMapper
    void setNoteMapEnabled(const bool noteMapEnabled);
    bool noteMapEnabled() const;
    void setHeaderInfo(const QStringList &headerInfo);
    QString headerLevel() const;
    QPair<QString, bool> sanitizePath(const QString &_path) const;
    void addToLinkedNoteInfos(const QStringList &infos);
    void addToNoteHeaders(const QString &header);
    // Syntax highlight
    void setHighlightEnabled(const bool highlightEnabled);
    bool highlightEnabled() const;
    void addToNoteCodeBlocks(const QString &codeBlock);
    // Emoji
    void setEmojiEnabled(const bool emojiEnabled);
    bool emojiEnabled() const;

Q_SIGNALS:
    // NoteMapper
    void newLinkedNotesInfos(const QSet<QStringList> &linkedNotesInfos);
    void noteHeadersSent(const QString &notePath, const QStringList &noteHeaders);

public Q_SLOTS:
    void newHighlightStyle();

private:
    QString tok();
    QString parseText();
    QString peekType() const;
    bool getNextToken();

    BlockLexer blockLexer = BlockLexer(this);
    InlineLexer inlineLexer = InlineLexer(this);

    QString m_notePath;
    QVariantMap m_token;

    // Synthax highlight
    bool m_highlightEnabled = KleverConfig::codeSynthaxHighlightEnabled();
    bool m_newHighlightStyle = true;
    bool m_sameCodeBlocks = false;
    int m_currentBlockIndex = 0;
    QStringList m_noteCodeBlocks;
    QStringList m_previousHighlightedBlocks;
    QStringList m_previousNoteCodeBlocks;

    // NoteMapper
    bool m_noteMapEnabled = KleverConfig::noteMapEnabled();
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
    bool m_emojiEnabled = false;
};
