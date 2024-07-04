/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "kleverconfig.h"
#include "noteMapperParserUtils.h"

#include <QRegularExpression>
#include <qlogging.h>

#define MD4QT_QT_SUPPORT
#include "logic/parser/md4qt/parser.hpp"

namespace NoteMapperFunc
{
inline long long int
processWikiLinkExtension(std::shared_ptr<MD::Paragraph<MD::QStringTrait>> paragraphsList, MD::TextParsingOpts<MD::QStringTrait> &po, long long int idx)
{
    if (idx < 0 || idx >= (long long int)po.rawTextData.size()) {
        return idx;
    }

    auto textData = po.rawTextData[idx];
    QString src = textData.str;

    static const QRegularExpression inline_wikilink =
        QRegularExpression(QStringLiteral("\\[\\[([^:\\]\\|\\r\\n]*)(:)?([^:\\]\\|\\r\\n]*)(\\|)?([^:\\]\\|\\r\\n]*)\\]\\]"));
    static const QString emptyStr = {};

    while (src.length()) {
        QRegularExpressionMatch cap = inline_wikilink.match(src);
        if (cap.hasMatch()) {
            if (!cap.captured(1).trimmed().isEmpty()) {
                QString href = cap.captured(1).trimmed();
                const QPair<QString, bool> sanitizedHref = NoteMapperParserUtils::sanitizePath(href, po.workingPath);

                QString cap3 = cap.captured(3).trimmed();

                const bool hasPipe = !cap.captured(4).isEmpty();

                const QString potentitalTitle = cap.captured(5).trimmed();
                QString title = hasPipe && !potentitalTitle.isEmpty() ? potentitalTitle : sanitizedHref.first.split(QStringLiteral("/")).last();

                if (sanitizedHref.second) {
                    auto lineInfo = po.fr.data.at(textData.line);
                    auto paragraphIdx = textAtIdx(paragraphsList, idx);
                    typename MD::ItemWithOpts<MD::QStringTrait>::Styles openStyles, closeStyles;
                    const auto opts = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraphsList->getItemAt(paragraphIdx))->opts();

                    std::shared_ptr<MD::Link<MD::QStringTrait>> link(new MD::Link<MD::QStringTrait>);
                    link->setStartColumn(lineInfo.first.virginPos(textData.pos + cap.capturedStart()));
                    link->setStartLine(lineInfo.second.lineNumber);
                    link->setEndColumn(lineInfo.first.virginPos(textData.pos + cap.capturedEnd()));
                    link->setEndLine(lineInfo.second.lineNumber);

                    // link->openStyles() = openStyles;

                    const int urlStart = lineInfo.first.virginPos(textData.pos + cap.capturedStart(1));
                    const int urlEnd = lineInfo.first.virginPos(textData.pos + cap.capturedEnd(3));
                    link->setUrlPos({urlStart, link->startLine(), urlEnd, link->endLine()});
                    // This hopefuly, is enough to separate the 2 without collinding with user input
                    QString fullUrl = sanitizedHref.first + QStringLiteral("@HEADER@") + cap3; // <Note path>@HEADER@<header ref>
                    link->setUrl(fullUrl);

                    const int textStart = lineInfo.first.virginPos(textData.pos + cap.capturedStart(5));
                    const int textEnd = lineInfo.first.virginPos(textData.pos + cap.capturedEnd(5));
                    link->setTextPos({textStart, link->startLine(), textEnd, link->endLine()});
                    link->setText(title);

                    link->setOpts(opts);

                    if (cap.captured().size() == src.length()) { // The capture is the whole Text
                        auto paragraph = paragraphsList->items().at(paragraphIdx);
                        openStyles = std::static_pointer_cast<MD::ItemWithOpts<MD::QStringTrait>>(paragraph)->openStyles();
                        closeStyles = std::static_pointer_cast<MD::ItemWithOpts<MD::QStringTrait>>(paragraph)->closeStyles();

                        // We remove it from from the raw text
                        po.rawTextData.erase(po.rawTextData.cbegin() + idx);
                        // We remove this Text from the paragraph
                        paragraphsList->removeItemAt(paragraphIdx);
                        // We insert our link instead
                        paragraphsList->insertItem(paragraphIdx, link);
                    } else {
                        auto textParagraph = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraphsList->getItemAt(paragraphIdx));

                        const QString textBeforeCap = src.mid(0, cap.capturedStart());
                        const QString textAfterCap = src.mid(cap.capturedEnd());
                        if (textBeforeCap.isEmpty()) {
                            // Change the OG paragraph for textAfterCap
                            textParagraph->setStartColumn(lineInfo.first.virginPos(textData.pos + cap.capturedEnd()));
                            // openStyles = paragraphTxt->openStyles();
                            // paragraphTxt->closeStyles() = {};
                            po.rawTextData[idx].str = textAfterCap;

                            auto text = MD::replaceEntity<MD::QStringTrait>(textBeforeCap.simplified());
                            text = MD::removeBackslashes<MD::QStringTrait>(text).asString();
                            textParagraph->setText(text);

                            textParagraph->setSpaceBefore(textAfterCap.front().isSpace());
                            textParagraph->setSpaceAfter(textData.pos < (long long int)po.rawTextData.size() - 1 ? lineInfo.first[textData.pos + 1].isSpace()
                                                                                                                 : true);
                            // We insert our link, just before the paragraph
                            paragraphsList->insertItem(paragraphIdx, link);
                        } else {
                            // Change the OG paragraph for textBeforeCap
                            const int previousEnd = textParagraph->endColumn();
                            textParagraph->setEndColumn(lineInfo.first.virginPos(textData.pos + cap.capturedStart())); // TODO Check if -1 is needed
                            // closeStyles = paragraphTxt->closeStyles();
                            // paragraphTxt->closeStyles() = {};
                            po.rawTextData[idx].str = textBeforeCap;

                            auto text = MD::replaceEntity<MD::QStringTrait>(textBeforeCap.simplified());
                            text = MD::removeBackslashes<MD::QStringTrait>(text).asString();
                            textParagraph->setText(text);

                            textParagraph->setSpaceAfter(textBeforeCap.back().isSpace());
                            textParagraph->setSpaceBefore(textBeforeCap.front().isSpace());

                            if (!textAfterCap.isEmpty()) {
                                // Add a new paragraph
                                ++idx;
                                ++paragraphIdx;
                                textData.str = textAfterCap; // We can reuse this struct
                                po.rawTextData.insert(po.rawTextData.cbegin() + idx, textData);
                                // textData
                                auto newTextParagraph = std::make_shared<MD::Text<MD::QStringTrait>>();
                                newTextParagraph->setStartColumn(lineInfo.first.virginPos(textData.pos + cap.capturedEnd()));
                                newTextParagraph->setStartLine(po.fr.data.at(textData.line).second.lineNumber);
                                newTextParagraph->setEndLine(po.fr.data.at(textData.line).second.lineNumber);
                                newTextParagraph->setEndColumn(po.fr.data.at(textData.line).first.virginPos(previousEnd));
                                newTextParagraph->setOpts(opts);

                                auto text = MD::replaceEntity<MD::QStringTrait>(textAfterCap);
                                text = MD::removeBackslashes<MD::QStringTrait>(text).asString();
                                newTextParagraph->setText(text);
                                newTextParagraph->setSpaceAfter(textData.spaceAfter);

                                newTextParagraph->setSpaceBefore(textAfterCap.front().isSpace());
                                // t->closeStyles() = closeStyles;

                                // The link will be before the newTextParagraph
                                paragraphsList->insertItem(paragraphIdx, newTextParagraph);
                            }
                            paragraphsList->insertItem(paragraphIdx, link);
                        }
                    }
                }
            }
            // Not a note path
            // We change the src after that and not repeat ourselve over and over on the same bit
            src = src.mid(cap.capturedEnd());
            continue;
        }
        break;
    }
    return ++idx;
}

inline void noteLinkingExtension(std::shared_ptr<MD::Paragraph<MD::QStringTrait>> p, MD::TextParsingOpts<MD::QStringTrait> &po)
{
    if (!KleverConfig::noteMapEnabled()) {
        return;
    }
    if (!po.collectRefLinks) {
        long long int i = 0;

        while (0 <= i && i < (long long int)po.rawTextData.size()) {
            i = processWikiLinkExtension(p, po, i);

            /* ++i; */
        }
    }
}
} // NoteMapperFunc
