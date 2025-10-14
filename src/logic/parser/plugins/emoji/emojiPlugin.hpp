/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "logic/parser/md4qtDataGetter.hpp"

// md4qt include
#include "logic/parser/md4qt/doc.h"

namespace EmojiPlugin
{
/**
 * @class EmojiItem
 * @brief Custom MD::ItemWithOpts representing an Emoji.
 * This will be used in the `onUserDefined` method of the Renderer.
 */
class EmojiItem : public MD::ItemWithOpts<MD::QStringTrait>
{
public:
    EmojiItem() = default;
    ~EmojiItem() override = default;

    MD::ItemType type() const override;

    /**
     * @brief Apply the `other` base values to this object.
     *
     * @param other Another EmojiItem.
     */
    void applyEmojiBase(const EmojiItem &other);

    std::shared_ptr<MD::Item<MD::QStringTrait>> clone(MD::Document<MD::QStringTrait> *doc = nullptr) const override;

    /**
     * @brief Set the emoji value.
     *
     * @param emoji The emoji value.
     */
    void setEmoji(const QString &emoji);

    /**
     * @brief Get the emoji value.
     *
     * @return The emoji value.
     */
    const QString &emoji() const;

    /**
     * @brief Get the emoji name position in the text.
     * @return A MD::WithPosition object holding the information about the emoji name position in the text.
     */
    const MD::WithPosition &emojiNamePos() const;

    /**
     * @brief Set the emoji name position in the text.
     *
     * @param pos A MD::WithPosition object holding the information about the emoji name position in the text.
     */
    void setEmojiNamePos(const MD::WithPosition &pos);

    /**
     * @brief Get the emoji options position in the text.
     * @return A MD::WithPosition object holding the information about the emoji options position in the text.
     */
    const MD::WithPosition &optionsPos() const;

    /**
     * @brief Set the emoji options position in the text.
     *
     * @param pos A MD::WithPosition object holding the information about the emoji options position in the text.
     */
    void setOptionsPos(const MD::WithPosition &pos);

    /**
     * @brief Get the emoji starting delim position in the text.
     * @return A MD::WithPosition object holding the information about the emoji starting delim position in the text.
     */
    const MD::WithPosition &startDelim() const;

    /**
     * @brief Set the emoji starting delim position in the text.
     *
     * @param pos A MD::WithPosition object holding the information about the emoji starting position in the text.
     */
    void setStartDelim(const MD::WithPosition &pos);

    /**
     * @brief Get the emoji ending delim position in the text.
     * @return A MD::WithPosition object holding the information about the emoji ending delim position in the text.
     */
    const MD::WithPosition &endDelim() const;
    /**
     * @brief Set the emoji ending delim position in the text.
     *
     * @param pos A MD::WithPosition object holding the information about the emoji ending position in the text.
     */
    void setEndDelim(const MD::WithPosition &pos);

private:
    QString m_emoji;
    MD::WithPosition m_emojiNamePos = {};
    MD::WithPosition m_optionsPos = {};
    MD::WithPosition m_startDelim = {};
    MD::WithPosition m_endDelim = {};
};

/**
 * @brief Provide a simple way to execute the whole parsing for the emoji.
 *
 * @param p The current MD::Paragraph being treated
 * @param po The current MD::TextParsingOpts used for `rawTextData`
 * @param rawIdx The index of the current rawTextData item being treated.
 */
inline long long int processEmoji(MDParagraphPtr p, MDParsingOpts &po, long long int rawIdx);

/**
 * @brief Function exposed to md4qt::Parser::addTextPlugin
 *
 * @param p The current MD::Paragraph being treated
 * @param po The current MD::TextParsingOpts used for `rawTextData`
 * @param options Options of this plugins
 */
void emojiHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options);
}
