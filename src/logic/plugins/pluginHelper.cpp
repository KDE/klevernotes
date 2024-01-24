/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "pluginHelper.h"

#include "logic/parser/renderer.h"
#include "logic/plugins/puml/pumlHelper.h"

void PluginHelper::clearPluginsInfo()
{
    // Syntax highlight
    m_noteCodeBlocks.clear();
    // NoteMapper
    m_linkedNotesInfos.clear();
    // PUML
    m_notePUMLBlocks.clear();

    m_noteHeaders.clear();
    m_headerFound = false;
    m_linkedNotesInfos.clear();
    if (!m_notePathChanged) {
        m_linkedNotesChanged = false;
    }
}

void PluginHelper::clearPluginsPreviousInfo()
{
    // Syntax highlight
    m_previousNoteCodeBlocks.clear();
    // NoteMapper
    m_previousLinkedNotesInfos.clear();
    m_previousNoteHeaders.clear();
    m_linkedNotesChanged = true;
    m_notePathChanged = true;
    // PUML
    m_previousPUMLDiag.clear();
}

void PluginHelper::setNoteMapperInfo(const QString &notePath)
{
    // notePath == storagePath/Category/Group/Note/ => /Category/Group/Note
    m_mapperNotePath = notePath.chopped(1).remove(KleverConfig::storagePath());

    // /Category/Group/Note => /Category/Group (no '/' at the end to make it easier for m_categPath)
    const QString groupPath = m_mapperNotePath.chopped(m_mapperNotePath.size() - m_mapperNotePath.lastIndexOf(QStringLiteral("/")));
    if (m_groupPath != groupPath) {
        m_groupPath = groupPath + QStringLiteral("/"); // /Category/Group => /Category/Group/
        m_categPath = groupPath.chopped(groupPath.size() - groupPath.lastIndexOf(QStringLiteral("/")) - 1); // /Category/Group => /Category/
    }
}

void PluginHelper::preTokChanges()
{
    if (KleverConfig::codeSynthaxHighlightEnabled()) {
        m_sameCodeBlocks = m_previousNoteCodeBlocks == m_noteCodeBlocks && !m_noteCodeBlocks.isEmpty();
        if (!m_sameCodeBlocks || m_newHighlightStyle) {
            m_previousNoteCodeBlocks = m_noteCodeBlocks;
            m_previousHighlightedBlocks.clear();
            m_newHighlightStyle = false;
            m_sameCodeBlocks = false;
        }
        m_currentBlockIndex = 0;
    }
    if (KleverConfig::pumlEnabled()) {
        m_samePUMLBlocks = m_previousNotePUMLBlocks == m_notePUMLBlocks && !m_notePUMLBlocks.isEmpty();
        if (!m_samePUMLBlocks || m_pumlDarkChanged) {
            m_previousNotePUMLBlocks = m_notePUMLBlocks;
            m_previousPUMLDiag.clear();
            m_pumlDarkChanged = false;
            m_samePUMLBlocks = false;
        }
        m_currentPUMLBlockIndex = 0;
    }
}

void PluginHelper::postTokChanges()
{
    m_notePathChanged = false;

    if (KleverConfig::noteMapEnabled()) {
        // We try to not spam with signals
        if (m_linkedNotesChanged || !m_previousLinkedNotesInfos.isEmpty()) { // The previous is not empty, some links notes are no longer there
            // Q_EMIT newLinkedNotesInfos(m_linkedNotesInfos);
        }
        m_previousLinkedNotesInfos = m_linkedNotesInfos;

        if (m_linkedNotesChanged || !m_previousNoteHeaders.isEmpty()) { // The previous is not empty, some headers are no longer there
            m_emptyHeadersSent = false;
            // Q_EMIT noteHeadersSent(m_mapperNotePath, m_noteHeaders.values());
        } else if (m_noteHeaders.isEmpty() && !m_emptyHeadersSent) {
            // This way the mapper can receive info about the note (the note has no header), and we still prevent spamming
            m_emptyHeadersSent = true;
            // Q_EMIT noteHeadersSent(m_mapperNotePath, {});
        }
        m_previousNoteHeaders = m_noteHeaders;
    }

    if (!m_headerFound) { // Prevent the TextDisplay.qml scrollToHeader to search an unexisting header
        m_headerLevel = QStringLiteral("0");
        m_header = QLatin1String();
    }
}

QPair<QString, bool> PluginHelper::sanitizePath(const QString &_path) const
{
    static const QString slashStr = QStringLiteral("/");
    QStringList parts = _path.split(slashStr);

    bool leadingSlashRemnant = false;
    for (int i = 0; i < parts.count(); i++) {
        QString part = parts[i].trimmed();
        if (part.isEmpty()) {
            if (i == 0) {
                leadingSlashRemnant = true;
            } else { // The path is not correctly formed
                return qMakePair(_path, false);
            }
        }
        parts[i] = part;
    }

    if (leadingSlashRemnant)
        parts.removeAt(0);

    if (parts[0] == KleverConfig::defaultCategoryDisplayNameValue())
        parts[0] = QStringLiteral(".BaseCategory");

    QString path = _path;
    switch (parts.count()) {
    case 1: // Note name only
        path = m_groupPath + parts[0];
        break;
    case 2:
        if (parts[0] == QStringLiteral(".")) { // Note name only
            path = m_groupPath + parts[1];
        } else { // Note inside category
            path = slashStr + parts[0] + QStringLiteral("/.BaseGroup/") + parts[1];
        }
        break;
    case 3: // 'Full' path
        path = slashStr + parts.join(slashStr);
        break;
    default: // Not a note path
        return qMakePair(_path, false);
    }

    return qMakePair(path, true);
}

QString PluginHelper::blockCodePlugins(const QString &lang, const QString &_text)
{
    static const QString pumlStr = QStringLiteral("puml");
    static const QString plantUMLStr = QStringLiteral("plantuml");

    QString returnValue;
    if (KleverConfig::pumlEnabled() && (lang.toLower() == pumlStr || lang.toLower() == plantUMLStr)) {
        if (m_samePUMLBlocks) {
            returnValue = m_previousPUMLDiag[m_currentPUMLBlockIndex];
            m_currentPUMLBlockIndex++;
        } else {
            const int diagNbr = m_previousPUMLDiag.size();
            const QString diagName = QStringLiteral("/KleverNotesPUMLDiag") + QString::number(diagNbr) + QStringLiteral(".png");
            const QString diagPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + diagName;
            const QString imgPath = PumlHelper::makeDiagram(_text, diagPath, KleverConfig::pumlDark()) ? diagPath : QLatin1String();
            returnValue = Renderer::image(imgPath, diagName, diagName);
            m_previousPUMLDiag.append(returnValue);
        }
    } else {
        const bool highlightEnabled = KleverConfig::codeSynthaxHighlightEnabled();
        const bool highlight = highlightEnabled && !lang.isEmpty();

        if (m_sameCodeBlocks && highlight) { // Only the highlighted values are stored in here
            returnValue = m_previousHighlightedBlocks[m_currentBlockIndex];
            m_currentBlockIndex++;
        } else {
            QString text = _text;
            returnValue = Renderer::code(text, lang, highlight);
            if (highlightEnabled && highlight) { // We want to store only the highlighted values
                m_previousHighlightedBlocks.append(returnValue);
            }
        }
    }

    return returnValue;
}

// NoteMapper
void PluginHelper::setHeaderInfo(const QStringList &headerInfo)
{
    m_header = headerInfo[0];
    m_headerLevel = m_header.isEmpty() ? QStringLiteral("0") : headerInfo[1];
}

QString PluginHelper::headerLevel() const
{
    return m_headerLevel;
};

void PluginHelper::checkHeaderFound(const QString &header, const QString &level)
{
    if (header == m_header && level == QString(m_headerLevel)) {
        m_headerFound = true;
    }
}

bool PluginHelper::headerFound() const
{
    return m_headerFound;
}

// Syntax highlight
void PluginHelper::addToNoteCodeBlocks(const QString &codeBlock)
{
    m_noteCodeBlocks.append(codeBlock);
}

void PluginHelper::newHighlightStyle()
{
    m_newHighlightStyle = true;
}

// NoteMapper
void PluginHelper::addToLinkedNoteInfos(const QStringList &infos)
{
    if ((!m_previousLinkedNotesInfos.remove(infos) && !m_linkedNotesInfos.contains(infos)) || infos.isEmpty()) {
        m_linkedNotesChanged = true;
    }
    m_linkedNotesInfos.insert(infos);
}

void PluginHelper::addToNoteHeaders(const QString &header)
{
    if (!m_previousNoteHeaders.remove(header) && !m_noteHeaders.contains(header)) {
        m_noteHeadersChanged = true;
    }
    m_noteHeaders.insert(header);
}

// PUML
void PluginHelper::addToNotePUMLBlock(const QString &pumlBlock)
{
    m_notePUMLBlocks.append(pumlBlock);
}

void PluginHelper::pumlDarkChanged()
{
    m_pumlDarkChanged = true;
}
