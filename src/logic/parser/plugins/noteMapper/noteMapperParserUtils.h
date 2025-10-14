/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QObject>
#include <QSet>

namespace MdEditor
{
class EditorHandler;
}

class NoteMapperParserUtils
{
public:
    explicit NoteMapperParserUtils(MdEditor::EditorHandler *editorHandler);

    /**
     * @brief Sanitize the given path relative to the given note path.
     *
     * @param _path The given path.
     * @param notePath The given note path.
     * @return A valid path relative to the given note path, an empty string if the action has failed.
     */
    static QString sanitizePath(const QString &_path, const QString &notePath);

    /**
     * @brief Set the path to the current note.
     *
     * @param _path The path to the current note.
     */
    void setNotePath(const QString &_path);

    // TODO: Not used => https://invent.kde.org/office/klevernotes/-/issues/17
    void setHeaderInfo(const QStringList &headerInfo);

    QString headerLevel() const;

    void checkHeaderFound(const QString &header, const QString &level);

    bool headerFound() const;
    // ====

    /**
     * @brief Add the given infos to the linked note info to the list of the current note.
     *
     * @param infos The info to add.
     */
    void addToLinkedNoteInfos(const QStringList &infos);

    /**
     * @brief Add the given header to the list of header related to the current note.
     *
     * @param header The header to add.
     */
    void addToNoteHeaders(const QString &header);

    /**
     * @brief Performs actions once the tokenization is finished.
     */
    void postTok();

    /**
     * @brief Clear the current cached info.
     */
    void clearInfo();

    /**
     * @brief Clear the previous round cached info.
     */
    void clearPreviousInfo();

private:
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
