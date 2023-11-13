/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "linkedNotesModel.h"
#include <QDebug>
#include <QMap>
#include <QObject>
#include <QString>

class NoteMapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(LinkedNotesModel *absentLinks READ getAbsentLinkedPaths NOTIFY absentLinksChanged)
    Q_PROPERTY(LinkedNotesModel *existingLinks READ getExistingLinkedPaths NOTIFY existingLinksChanged)
public:
    NoteMapper(QObject *parent = nullptr);

    LinkedNotesModel *getExistingLinkedPaths();
    LinkedNotesModel *getAbsentLinkedPaths();

    // Treeview
    Q_INVOKABLE void addGlobalPath(const QString &path, const QString &displayedPath);
    Q_INVOKABLE void updateGlobalPath(const QString &oldPath, const QString &newPath, const QString &displayedPath);
    Q_INVOKABLE void removeGlobalPath(const QString &path);

    // Parser
    Q_INVOKABLE void addNotePaths(const QVariantMap &notePaths);

signals:
    void absentLinksChanged();
    void existingLinksChanged();

private:
    void filterLinkedPath();

    LinkedNotesModel m_existingLinkedPath;
    LinkedNotesModel m_absentLinkedPath;

    // Parser
    QVariantMap m_linkedNotePaths;

    // Treeview
    QMap<QString, QString> m_treeViewPaths;
};
