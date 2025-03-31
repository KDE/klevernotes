/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "logic/parser/md4qtDataGetter.hpp"

namespace HeaderLinkingPlugin
{
class HeaderLinkingItem : public MD::ItemWithOpts<MD::QStringTrait>
{
public:
    HeaderLinkingItem() = default;
    ~HeaderLinkingItem() override = default;

    MD::ItemType type() const override;

    void applyBase(const HeaderLinkingItem &other);
    std::shared_ptr<MD::Item<MD::QStringTrait>> clone(MD::Document<MD::QStringTrait> *doc = nullptr) const override;

    const QString &text() const;
    void setText(const QString &text);

    int level() const;
    void setLevel(int level);

    const MD::WithPosition &delim() const;
    void setDelim(const MD::WithPosition &pos);

    const MD::WithPosition &textPos() const;
    void setTextPos(const MD::WithPosition &pos);

private:
    QString m_text;
    int m_level;
    MD::WithPosition m_delim = {};
    MD::WithPosition m_textPos = {};
};

inline void processHeaderLinking(MDParagraphPtr p, MDParsingOpts &po);

void headerLinkingHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options);
}
