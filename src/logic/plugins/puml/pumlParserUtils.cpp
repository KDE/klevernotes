/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "pumlParserUtils.h"

#include "kleverconfig.h"
#include "logic/parser/renderer.h"
#include "logic/plugins/puml/pumlHelper.h"

void PUMLParserUtils::clearInfo()
{
    m_notePUMLBlocks.clear();
}

void PUMLParserUtils::clearPreviousInfo()
{
    m_previousPUMLDiag.clear();
}

void PUMLParserUtils::preTok()
{
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

void PUMLParserUtils::addToNotePUMLBlock(const QString &pumlBlock)
{
    m_notePUMLBlocks.append(pumlBlock);
}

void PUMLParserUtils::pumlDarkChanged()
{
    m_pumlDarkChanged = true;
}

QString PUMLParserUtils::renderCode(const QString &_text)
{
    QString returnValue;
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

    return returnValue;
}
