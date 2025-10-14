/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QHash>

/**
 * @class HighlightParserUtils
 * @brief Facilitate the connection between the parser/renderer and the HighlightHelper.
 */
class HighlightParserUtils
{
public:
    /**
     * @brief Clear the previously cached highlighted block of code.
     */
    void clearInfo();

    /**
     * @brief Get a highlighted or escaped code from the input text.
     *
     * @param highlight Whether the code should be highlighted.
     * @param _text The given text to be treated.
     * @param lang The language of the code if it is highlighted.
     * @return A highlighted or escaped code.
     */
    QString getCode(const bool highlight, const QString &_text, const QString &lang);

    /**
     * @brief Tell the HighlightParserUtils that a new style is being used.
     * Prevents issue with cached info.
     */
    void newHighlightStyle();

private:
    bool m_newHighlightStyle = true;

    QHash<QString, QString> m_previousHighlightedBlocks;
    QHash<QString, QString> m_currentHighlightedBlocks;
};
