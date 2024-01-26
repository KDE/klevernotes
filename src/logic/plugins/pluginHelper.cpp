/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "pluginHelper.h"

#include "logic/parser/renderer.h"
#include "logic/plugins/puml/pumlHelper.h"

PluginHelper::PluginHelper(Parser *parser)
    : m_highlightParserUtils(new HighlightParserUtils)
{
    m_mapperParserUtils = new NoteMapperParserUtils(parser);
}

void PluginHelper::clearPluginsInfo()
{
    // Syntax highlight
    m_highlightParserUtils->clearInfo();
    // NoteMapper
    m_mapperParserUtils->clearInfo();
    // PUML
    m_notePUMLBlocks.clear();
}

void PluginHelper::clearPluginsPreviousInfo()
{
    // Syntax highlight
    m_highlightParserUtils->clearPreviousInfo();
    // NoteMapper
    m_mapperParserUtils->clearPreviousInfo();
    // PUML
    m_previousPUMLDiag.clear();
}

void PluginHelper::preTokChanges()
{
    m_highlightParserUtils->preTok();

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
    // NoteMapper
    m_mapperParserUtils->postTok();
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

        returnValue = m_highlightParserUtils->renderCode(highlight, _text, lang);
    }

    return returnValue;
}

// NoteMapper
NoteMapperParserUtils *PluginHelper::getMapperParserUtils() const
{
    return m_mapperParserUtils;
}

// Syntax highlight
HighlightParserUtils *PluginHelper::getHighlightParserUtils() const
{
    return m_highlightParserUtils;
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
