/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "noteLinkingPlugin.hpp"

#include "logic/parser/md4qtDataManip.hpp"
#include "noteMapperParserUtils.h"

// Qt include
#include <QRegularExpression>

namespace NoteLinkingPlugin
{

inline long long int processNoteLinking(MDParagraphPtr p, MDParsingOpts &po, long long int rawIdx)
{
    if (rawIdx < 0 || rawIdx >= static_cast<long long>(po.m_rawTextData.size())) {
        return rawIdx;
    }

    auto textData = po.m_rawTextData[rawIdx];
    QString src = textData.m_str;

    static const QRegularExpression inline_wikilink =
        QRegularExpression(QStringLiteral("\\[\\[ *([^:\\]\\|\\r\\n]*)( *: *)?([^:\\]\\|\\r\\n]*)( *\\| *)?([^:\\]\\|\\r\\n]*) *\\]\\]"));

    int offSet = 0;
    while (offSet != src.length()) {
        QRegularExpressionMatch cap = inline_wikilink.matchView(src, offSet);
        if (cap.hasMatch()) {
            const QString href = cap.captured(1).trimmed();
            if (!href.isEmpty()) {
                const QString sanitizedHref = NoteMapperParserUtils::sanitizePath(href, po.m_workingPath);
                if (!sanitizedHref.isEmpty()) {
                    const QString header = cap.captured(3).trimmed();

                    const bool hasPipe = !cap.captured(4).isEmpty();

                    const QString potentitalTitle = cap.captured(5).trimmed();
                    const QString title = hasPipe && !potentitalTitle.isEmpty() ? potentitalTitle : sanitizedHref.split(QStringLiteral("/")).last();

                    auto lineInfo = po.m_fr.m_data.at(textData.m_line);
                    auto paraIdx = textAtIdx(p, rawIdx);
                    const auto item = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx));
                    const auto opts = item->opts();

                    const long long int virginStartPos = item->startColumn() + cap.capturedStart();
                    const long long int virginEndPos = item->startColumn() + cap.capturedEnd() - 1;
                    std::shared_ptr<MD::Link<MD::QStringTrait>> link(new MD::Link<MD::QStringTrait>);
                    link->setStartColumn(virginStartPos);
                    link->setStartLine(item->startLine());
                    link->setEndColumn(virginEndPos);
                    link->setEndLine(item->endLine());

                    const int urlStart = item->startColumn() + cap.capturedStart(1);
                    const int urlEnd = (header.isEmpty() ? urlStart + href.length() : item->startColumn() + cap.capturedEnd(2) + header.length()) - 1;
                    link->setUrlPos({urlStart, link->startLine(), urlEnd, link->endLine()});
                    // This hopefuly, is enough to separate the 2 without collinding with user input
                    const QString fullUrl = sanitizedHref + QStringLiteral("@HEADER@") + header; // <Note path>@HEADER@<header ref>
                    link->setUrl(fullUrl);

                    const int textStart = item->startColumn() + cap.capturedStart(5);
                    const int textEnd = textStart + potentitalTitle.length() - 1;
                    link->setTextPos({textStart, link->startLine(), textEnd, link->endLine()});
                    link->setText(title);

                    link->setOpts(opts);

                    const int addedData = md4qtHelperFunc::splitItem(p, po, paraIdx, rawIdx, virginStartPos, cap.capturedLength());

                    if (addedData == -1) {
                        link->openStyles() << item->openStyles();
                        link->closeStyles() << item->closeStyles();
                        p->removeItemAt(paraIdx);
                        po.m_rawTextData.erase(po.m_rawTextData.cbegin() + rawIdx);
                    } else if (addedData == 1) {
                        const auto currentItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx));
                        const auto nextItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx + 1));
                        md4qtHelperFunc::transferStyle(currentItem, nextItem, true);
                    }

                    const long long int totalOffSet = offSet + cap.capturedStart();
                    // If capture start at 0, then we stay on the same index
                    const long long int linkParaIdx = totalOffSet ? paraIdx + 1 : paraIdx;
                    p->insertItem(linkParaIdx, link);

                    if (addedData == 0) {
                        const auto currentItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx));
                        const auto nextItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx + 1));
                        if (totalOffSet) {
                            md4qtHelperFunc::transferStyle(currentItem, nextItem, true);
                            md4qtHelperFunc::transferStyle(currentItem, nextItem, false);
                        } else {
                            md4qtHelperFunc::transferStyle(nextItem, currentItem, true);
                            md4qtHelperFunc::transferStyle(nextItem, currentItem, false);
                        }
                    }

                    return addedData < 0 ? rawIdx : ++rawIdx;
                }
            }
            // Not a note path
            offSet = cap.capturedEnd();
            continue;
        }
        break;
    }
    return ++rawIdx;
}

void noteLinkingHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options)
{
    Q_UNUSED(options);
    if (!po.m_collectRefLinks) {
        long long int i = 0;

        while (0 <= i && i < (long long int)po.m_rawTextData.size()) {
            i = processNoteLinking(p, po, i);
        }
    }
}
}
