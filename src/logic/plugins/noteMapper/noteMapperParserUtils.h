/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QObject>
#include <QSet>

class Parser;

class NoteMapperParserUtils
{
public:
    explicit NoteMapperParserUtils(Parser *parser);

    void setPathsInfo(const QString &path);
    QPair<QString, bool> sanitizePath(const QString &_path) const;
    void setHeaderInfo(const QStringList &headerInfo);
    QString headerLevel() const;
    void addToLinkedNoteInfos(const QStringList &infos);
    void addToNoteHeaders(const QString &header);
    void checkHeaderFound(const QString &header, const QString &level);
    bool headerFound() const;
    void postTok();
    void clearInfo();
    void clearPreviousInfo();

private:
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

    Parser *m_parser = nullptr;
};
