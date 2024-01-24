/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "kleverconfig.h"
#include <QObject>
#include <QSet>

class PluginHelper
{
public:
    void clearPluginsInfo();
    void clearPluginsPreviousInfo();
    void setNoteMapperInfo(const QString &notePath);
    void preTokChanges();
    void postTokChanges();

    QPair<QString, bool> sanitizePath(const QString &_path) const;

    // NoteMapper
    void setHeaderInfo(const QStringList &headerInfo);
    QString headerLevel() const;
    void addToLinkedNoteInfos(const QStringList &infos);
    void addToNoteHeaders(const QString &header);

    // Syntax highlight
    void addToNoteCodeBlocks(const QString &codeBlock);
    void newHighlightStyle();

    // Emoji
    void setEmojiTone(const QString &emojiTone);
    QString emojiTone() const;

    // PUML
    void addToNotePUMLBlock(const QString &pumlBlock);
    void pumlDarkChanged();

private:
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
};
