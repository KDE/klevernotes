#pragma once

#include <QDebug>
#include <QObject>
#include <QSet>
#include <QString>

class NoteMapper : public QObject
{
public:
    NoteMapper();

    // Treeview
    void addGlobalPath(const QString &path);
    void updateGlobalPath(const QString &oldPath, const QString &newPath);
    void removeGlobalPath(const QString &path);

    // Parser

private:
    // Parser
    QSet<QString> notePaths;
    QString sanitizePath(QString &path);

    // Treeview
    QSet<QString> m_treeViewPaths;
};
