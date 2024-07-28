/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QObject>
#include <QSet>
#include <tuple>

namespace MdEditor
{
class EditorHandler;
}

class NoteMapperParserUtils
{
public:
    explicit NoteMapperParserUtils(MdEditor::EditorHandler *editorHandler);

    static QPair<QString, bool> sanitizePath(const QString &_path, const QString &notePath);
    void setNotePath(const QString &_path);
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
    static QString getGroupPath(const QString &path);
    QString m_mapperNotePath;
    QString m_groupPath;
    QString m_categPath;
    QString m_header;

    QString m_headerLevel;
    bool m_headerFound = false;
    bool m_emptyHeadersSent = false;

    QStringList m_noteHeaders;
    QSet<QString> m_previousNoteHeaders;
    bool m_noteHeadersChanged = false;

    QSet<QStringList> m_linkedNotesInfos;
    QSet<QStringList> m_previousLinkedNotesInfos;
    bool m_linkedNotesChanged = false;
    bool m_notePathChanged = true;

    MdEditor::EditorHandler *m_editorHandler = nullptr;
};
