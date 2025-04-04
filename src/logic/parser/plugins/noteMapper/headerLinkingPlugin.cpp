/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
*/

#include "headerLinkingPlugin.h"

// KleverNotes include
#include "logic/parser/plugins/pluginsSharedValues.h"

// md4qt include
#include "logic/parser/md4qt/doc.h"

// Qt include
#include <QRegularExpression>

namespace HeaderLinkingPlugin
{

// HeaderLinkingItem
MD::ItemType HeaderLinkingItem::type() const
{
    return MD::ItemType{PluginsSharedValues::CustomType::HeaderLinking};
}

void HeaderLinkingItem::applyBase(const HeaderLinkingItem &other)
{
    if (this != &other) {
        MD::ItemWithOpts<MD::QStringTrait>::applyItemWithOpts(other);
        setText(other.text());
        setTextPos(other.textPos());
        setLevel(other.level());
        setDelim(other.delim());
    }
}

std::shared_ptr<MD::Item<MD::QStringTrait>> HeaderLinkingItem::clone(MD::Document<MD::QStringTrait> *doc) const
{
    MD_UNUSED(doc)

    auto t = std::make_shared<HeaderLinkingItem>();
    t->applyBase(*this);

    return t;
}

const QString &HeaderLinkingItem::text() const
{
    return m_text;
}

void HeaderLinkingItem::setText(const QString &text)
{
    m_text = text;
}

int HeaderLinkingItem::level() const
{
    return m_level;
}

void HeaderLinkingItem::setLevel(int level)
{
    m_level = level;
}

const MD::WithPosition &HeaderLinkingItem::delim() const
{
    return m_delim;
}

void HeaderLinkingItem::setDelim(const MD::WithPosition &pos)
{
    m_delim = pos;
}

const MD::WithPosition &HeaderLinkingItem::textPos() const
{
    return m_textPos;
}

void HeaderLinkingItem::setTextPos(const MD::WithPosition &pos)
{
    m_textPos = pos;
}
// !HeaderLinkingItem

inline void processHeaderLinking(MDParagraphPtr p, MDParsingOpts &po)
{
    auto textData = po.m_rawTextData[0];
    QString src = textData.m_str;

    static const QRegularExpression linkingHeaderReg = QRegularExpression(QStringLiteral("^([0-9]+)# (.*){1}"));

    QRegularExpressionMatch cap = linkingHeaderReg.match(src);
    if (cap.hasMatch()) {
        const QString levelStr = cap.captured(1);
        int level = levelStr.toInt();
        level = 6 < level ? 6 : level ? level : 1;

        const QString text = cap.captured(2).trimmed();

        const auto item = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(0));

        auto headerItem = std::make_shared<HeaderLinkingItem>();
        headerItem->applyPositions({item->startColumn(), item->startLine(), item->endColumn(), item->endLine()});

        const long long int delimEnd = item->startColumn() + cap.capturedStart(2) - 1;
        headerItem->setDelim({item->startColumn(), item->startLine(), delimEnd, item->endLine()});
        headerItem->setLevel(level);

        headerItem->setText(text);
        headerItem->setTextPos({delimEnd + 1, item->startLine(), item->endColumn(), item->endLine()});

        po.m_rawTextData.erase(po.m_rawTextData.cbegin());
        p->removeItemAt(0);
        p->insertItem(0, headerItem);
    }
}

void headerLinkingHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options)
{
    Q_UNUSED(options);
    if (!po.m_collectRefLinks) {
        if (po.m_rawTextData.size() == 1 && p->items().size() == 1 && p->items().at(0)->startColumn() == 0) {
            processHeaderLinking(p, po);
        }
    }
}
}
