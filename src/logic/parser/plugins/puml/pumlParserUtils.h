/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QHash>
#include <QObject>

/**
 * @class PUMLParserUtils
 * @brief Facilitate the connection between the parser/renderer and the PUMLHelper.
 */
class PUMLParserUtils
{
public:
    /**
     * @brief Clear the cached info.
     */
    void clearInfo();

    /**
     * @brief Tell the PUMLParserUtils that the PUML background should now be dark.
     * Prevents issue with cached info.
     */
    void pumlDarkChanged();

    /**
     * @brief Transform the given code into PUML image.
     *
     * @param _text The text to be converted into an image.
     * @param pumlDark Whether or not the resulting image should have a dark background.
     * @return A pair containing the resulting image path and its name.
     */
    QPair<QString, QString> renderCode(const QString &_text, const bool pumlDark);

private:
    bool m_pumlDarkChanged = true;

    QHash<QString, QPair<QString, QString>> m_previousPUMLBlocks;
    QHash<QString, QPair<QString, QString>> m_currentPUMLBlocks;
};
