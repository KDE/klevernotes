/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "noteLinkingPlugin.hpp"

#include "kleverconfig.h"
#include "noteMapperParserUtils.h"

// md4qt include.
#include <md4qt/src/inline_context.h>
#include <md4qt/src/reverse_solidus.h>
#include <md4qt/src/text_stream.h>
#include <md4qt/src/utils.h>

namespace NoteLinkingPlugin
{

static const QChar s_leftSquare = QLatin1Char('[');
static const QChar s_colon = QLatin1Char(':');
static const QChar s_rightSquare = QLatin1Char(']');
static const QChar s_pipe = QLatin1Char('|');
static const QChar s_space = QLatin1Char(' ');

inline bool isNotSpecial(const QChar &ch)
{
    return (ch != s_colon && ch != s_rightSquare && ch != s_pipe);
}

inline void readPart(MD::Line &line, QString &receiver, qsizetype &spacesAtEnd)
{
    while (line.position() < line.length()) {
        if (isNotSpecial(line.currentChar())) {
            if (line.currentChar() == s_space) {
                ++spacesAtEnd;
            } else {
                spacesAtEnd = 0;
            }

            receiver.append(line.currentChar());
            line.nextChar();
        } else {
            break;
        }
    }

    receiver = receiver.trimmed();
}

bool NoteLinkingParser::check(MD::Line &line,
                              MD::ParagraphStream &,
                              MD::InlineContext &ctx,
                              QSharedPointer<MD::Document>,
                              const QString &path,
                              const QString &,
                              QStringList &,
                              MD::Parser &,
                              const MD::ReverseSolidusHandler &rs)
{
    if (!rs.isPrevReverseSolidus()) {
        const auto st = line.currentState();

        line.nextChar();

        if (line.currentChar() == s_leftSquare) {
            line.nextChar();

            QString href;
            QString header;
            QString title;

            qsizetype hrefSpacesAtEnd = 0;
            qsizetype headerSpacesAtEnd = 0;
            qsizetype titleSpacesAtEnd = 0;

            const auto urlStart = line.position();
            qsizetype headerStart = -1;
            qsizetype titleStart = -1;

            readPart(line, href, hrefSpacesAtEnd);

            if (!href.isEmpty()) {
                if (line.currentChar() == s_colon) {
                    line.nextChar();

                    MD::skipSpaces(line);

                    headerStart = line.position();

                    readPart(line, header, headerSpacesAtEnd);
                }

                if (line.currentChar() == s_pipe) {
                    line.nextChar();

                    MD::skipSpaces(line);

                    titleStart = line.position();

                    readPart(line, title, titleSpacesAtEnd);
                }

                if (line.currentChar() == s_rightSquare) {
                    line.nextChar();

                    if (line.currentChar() == s_rightSquare) {
                        line.nextChar();

                        QString relativeNoteDir = QString(path);
                        if (relativeNoteDir.startsWith(KleverConfig::storagePath())) {
                            relativeNoteDir.remove(0, KleverConfig::storagePath().length());
                        }
                        const QString sanitizedHref = NoteMapperParserUtils::sanitizePath(href, relativeNoteDir);

                        if (!sanitizedHref.isEmpty()) {
                            const auto potentitalTitle = title;

                            if (title.isEmpty()) {
                                title = sanitizedHref.split(QStringLiteral("/")).last();
                            }

                            auto link = QSharedPointer<MD::Link>::create();
                            link->setStartColumn(st.m_pos);
                            link->setStartLine(line.lineNumber());
                            link->setEndColumn(line.position() - 1);
                            link->setEndLine(line.lineNumber());

                            const auto urlEnd = (header.isEmpty() ? urlStart + href.length() : headerStart + header.length()) - 1;
                            link->setUrlPos({urlStart, link->startLine(), urlEnd, link->endLine()});
                            // This hopefuly, is enough to separate the 2 without collinding with user input
                            const QString fullUrl = sanitizedHref + QStringLiteral("@HEADER@") + header; // <Note path>@HEADER@<header ref>
                            link->setUrl(fullUrl);

                            if (titleStart != -1) {
                                const auto textEnd = titleStart + potentitalTitle.length() - 1;
                                link->setTextPos({titleStart, link->startLine(), textEnd, link->endLine()});
                            }

                            link->setText(title);

                            ctx.inlines().append(link);

                            return true;
                        }
                    }
                }
            }
        }

        line.restoreState(&st);
    }

    return false;
}

QString NoteLinkingParser::startDelimiterSymbols() const
{
    return QStringLiteral("[");
}
}
