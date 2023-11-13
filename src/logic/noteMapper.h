/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QDebug>
#include <QObject>
#include <QSet>
#include <QString>

class NoteMapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList absentLinks READ getAbsentLinkedPaths NOTIFY absentLinksChanged)
    Q_PROPERTY(QStringList existingLinks READ getExistingLinkedPaths NOTIFY existingLinksChanged)
public:
    NoteMapper(QObject *parent = nullptr);

    QStringList getExistingLinkedPaths();
    QStringList getAbsentLinkedPaths();

    // Treeview
    Q_INVOKABLE void addGlobalPath(const QString &path);
    Q_INVOKABLE void updateGlobalPath(const QString &oldPath, const QString &newPath);
    Q_INVOKABLE void removeGlobalPath(const QString &path);

    // Parser
    Q_INVOKABLE void addNotePaths(const QStringList &notePaths);

signals:
    void absentLinksChanged();
    void existingLinksChanged();

private:
    void filterLinkedPath();

    QSet<QString> m_existingLinkedPath;
    QSet<QString> m_absentLinkedPath;

    // Parser
    QSet<QString> m_linkedNotePaths;

    // Treeview
    QSet<QString> m_treeViewPaths;
};
