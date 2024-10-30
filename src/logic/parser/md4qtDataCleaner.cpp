/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "md4qtDataCleaner.hpp"
#include "logic/parser/md4qtDataGetter.hpp"

namespace md4qtDataCleaner
{
void removeEmpty(MDParagraphPtr p, MDParsingOpts &po)
{
    long long int rawIdx = 0;
    long long int paraIdx = 0;
    while (paraIdx < p->items().length()) {
        if (p->getItemAt(paraIdx)->type() == MD::ItemType::Text) {
            if (po.m_rawTextData[rawIdx].m_str.trimmed().isEmpty()) {
                po.m_rawTextData.erase(po.m_rawTextData.cbegin() + rawIdx);
                p->removeItemAt(paraIdx);
                continue;
            }
            ++rawIdx;
        }
        ++paraIdx;
    }
}

void dataCleaningFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options)
{
    Q_UNUSED(options);

    removeEmpty(p, po);
}
}
