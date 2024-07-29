/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "pumlParserUtils.h"

#include "pumlHelper.h"
#include <QStandardPaths>
#include <QUuid>

void PUMLParserUtils::clearInfo()
{
    m_previousPUMLBlocks = m_currentPUMLBlocks;
    m_currentPUMLBlocks.clear();
}

void PUMLParserUtils::pumlDarkChanged()
{
    m_pumlDarkChanged = true;
}

QPair<QString, QString> PUMLParserUtils::renderCode(const QString &_text, const bool pumlDark)
{
    if (m_pumlDarkChanged) {
        m_previousPUMLBlocks.clear();
        m_pumlDarkChanged = false;
    }
    QPair<QString, QString> pumlInfo;
    if (m_previousPUMLBlocks.contains(_text)) {
        pumlInfo = m_previousPUMLBlocks.value(_text);
    } else {
        const QString diagName = QStringLiteral("/KleverNotesPUMLDiag") + QUuid::createUuid().toString() + QStringLiteral(".png");
        const QString diagPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + diagName;
        const QString imgPath = PumlHelper::makeDiagram(_text, diagPath, pumlDark) ? diagPath : QLatin1String();
        pumlInfo = {imgPath, diagName};
    }
    m_currentPUMLBlocks.insert(_text, pumlInfo);

    return pumlInfo;
}
