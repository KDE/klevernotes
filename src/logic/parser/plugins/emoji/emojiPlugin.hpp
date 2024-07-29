/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "logic/parser/md4qtDataGetter.hpp"

// md4qt include
#include "logic/parser/md4qt/doc.hpp"

namespace EmojiPlugin
{
class EmojiItem : public MD::ItemWithOpts<MD::QStringTrait>
{
public:
    EmojiItem() = default;
    ~EmojiItem() override = default;

    MD::ItemType type() const override;

    void applyEmojiBase(const EmojiItem &other);
    std::shared_ptr<MD::Item<MD::QStringTrait>> clone(MD::Document<MD::QStringTrait> *doc = nullptr) const override;

    void setEmoji(const QString &emoji);
    const QString &emoji() const;

    const MD::WithPosition &emojiNamePos() const;
    void setEmojiNamePos(const MD::WithPosition &pos);

    const MD::WithPosition &optionsPos() const;
    void setOptionsPos(const MD::WithPosition &pos);

private:
    QString m_emoji;
    WithPosition m_emojiNamePos = {};
    WithPosition m_optionsPos = {};
};

inline long long int processEmoji(MDParagraphPtr p, MDParsingOpts &po, long long int rawIdx);

void emojiHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options);
}
