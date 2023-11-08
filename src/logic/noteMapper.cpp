#include "noteMapper.h"

NoteMapper::NoteMapper()
{
}

// Treeview
void NoteMapper::addGlobalPath(const QString &path)
{
    qDebug() << path;
    // m_treeViewPaths.insert(path);
}

void NoteMapper::updateGlobalPath(const QString &oldPath, const QString &newPath)
{
    if (!m_treeViewPaths.contains(oldPath))
        return;

    m_treeViewPaths.erase(m_treeViewPaths.find(oldPath));
    m_treeViewPaths.insert(newPath);
}

void NoteMapper::removeGlobalPath(const QString &path)
{
    if (!m_treeViewPaths.contains(path))
        return;

    m_treeViewPaths.erase(m_treeViewPaths.find(path));
}

// Parser
QString NoteMapper::sanitizePath(QString &path)
{
    return path;
}
