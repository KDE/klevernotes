/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "noteLinkingExtension.hpp"

#include "kleverconfig.h"
#include "logic/parser/md4qtDataGetter.hpp"
#include "logic/parser/md4qtDataManip.hpp"
#include "noteMapperParserUtils.h"

// Qt include
#include <QRegularExpression>

namespace NoteLinkingExtension
{

inline void checkSpace(MDParagraphPtr p, const long long int linkParaIdx, const long long int captureStart)
{
    if (captureStart) {
        auto previousItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(linkParaIdx - 1));
        if (previousItem->type() == MD::ItemType::Text) { }
        return;
    }
}

inline long long int processNoteLinking(MDParagraphPtr p, MDParsingOpts &po, long long int rawIdx)
{
    if (rawIdx < 0 || rawIdx >= (long long int)po.rawTextData.size()) {
        return rawIdx;
    }

    auto textData = po.rawTextData[rawIdx];
    QString src = textData.str;

    static const QRegularExpression inline_wikilink =
        QRegularExpression(QStringLiteral("\\[\\[([^:\\]\\|\\r\\n]*)(:)?([^:\\]\\|\\r\\n]*)(\\|)?([^:\\]\\|\\r\\n]*)\\]\\]"));

    int offSet = 0;
    while (offSet != src.length()) {
        QRegularExpressionMatch cap = inline_wikilink.matchView(src, offSet);
        bool t = cap.hasMatch();
        if (t) {
            QString u = cap.captured(1).trimmed();
            if (!u.isEmpty()) {
                QString href = cap.captured(1).trimmed();
                const QPair<QString, bool> sanitizedHref = NoteMapperParserUtils::sanitizePath(href, po.workingPath);

                QString cap3 = cap.captured(3).trimmed();

                const bool hasPipe = !cap.captured(4).isEmpty();

                const QString potentitalTitle = cap.captured(5).trimmed();
                QString title = hasPipe && !potentitalTitle.isEmpty() ? potentitalTitle : sanitizedHref.first.split(QStringLiteral("/")).last();

                if (sanitizedHref.second) {
                    auto lineInfo = po.fr.data.at(textData.line);
                    auto paragraphIdx = textAtIdx(p, rawIdx);
                    const auto opts = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paragraphIdx))->opts();

                    const long long int startPos = textData.pos + cap.capturedStart();
                    std::shared_ptr<MD::Link<MD::QStringTrait>> link(new MD::Link<MD::QStringTrait>);
                    link->setStartColumn(lineInfo.first.virginPos(startPos));
                    link->setStartLine(lineInfo.second.lineNumber);
                    link->setEndColumn(lineInfo.first.virginPos(textData.pos + cap.capturedEnd() - 1));
                    link->setEndLine(lineInfo.second.lineNumber);

                    const int urlStart = lineInfo.first.virginPos(textData.pos + cap.capturedStart(1));
                    const int urlEnd = lineInfo.first.virginPos(textData.pos + cap.capturedEnd(3) - 1);
                    link->setUrlPos({urlStart, link->startLine(), urlEnd, link->endLine()});
                    // This hopefuly, is enough to separate the 2 without collinding with user input
                    QString fullUrl = sanitizedHref.first + QStringLiteral("@HEADER@") + cap3; // <Note path>@HEADER@<header ref>
                    link->setUrl(fullUrl);

                    const int textStart = lineInfo.first.virginPos(textData.pos + cap.capturedStart(5));
                    const int textEnd = lineInfo.first.virginPos(textData.pos + cap.capturedEnd(5) - 1);
                    link->setTextPos({textStart, link->startLine(), textEnd, link->endLine()});
                    link->setText(title);

                    link->setOpts(opts);

                    const int addedData = md4qtHelperFunc::splitItem(p, po, paragraphIdx, startPos, cap.capturedLength(), false, 0);

                    const long long int linkParaIdx = cap.capturedStart() ? paragraphIdx + 1 : paragraphIdx;
                    p->insertItem(linkParaIdx, link);

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
    // TODO: make this useless with removeTextPluginh
    if (!KleverConfig::noteMapEnabled()) {
        return;
    }
    Q_UNUSED(options);
    if (!po.collectRefLinks) {
        long long int i = 0;

        while (0 <= i && i < (long long int)po.rawTextData.size()) {
            i = processNoteLinking(p, po, i);

            /* ++i; */
        }
    }
}
}
