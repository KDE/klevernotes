/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#include "blockLexer.h"

#include <QJsonArray>
#include <QMap>

#include "kleverconfig.h"
#include "parser.h"

BlockLexer::BlockLexer(Parser *parser)
    : m_parser(parser)
{
}

void BlockLexer::lex(QString &src)
{
    src = preprocess(src);
    tokenize(src, true);
}

QString BlockLexer::preprocess(QString &src) const
{
    QRegularExpressionMatch cap;

    for (auto &pat : preprocessRegex.toStdMap()) {
        cap = pat.second.match(src);
        while (cap.hasMatch()) {
            src = src.replace(pat.second, pat.first);
            cap = pat.second.match(src);
        }
    }

    return src;
};

void BlockLexer::tokenize(QString &remaining, const bool top)
{
    static const QString emptyStr = QLatin1String();
    QRegularExpressionMatch cap;

    static PluginHelper *pluginHelper = m_parser->getPluginHelper();
    static NoteMapperParserUtils *mapperParserUtils = pluginHelper->getMapperParserUtils();
    static HighlightParserUtils *highlightParserUtils = pluginHelper->getHighlightParserUtils();
    static PUMLParserUtils *pumlParserUtils = pluginHelper->getPUMLParserUtils();

    while (!remaining.isEmpty()) {
        cap = block_newline.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            if (cap.capturedLength() > 1) {
                static const QVariantMap tok{{QStringLiteral("type"), QStringLiteral("space")}};
                m_parser->tokens.append(tok);
            }
        }

        cap = block_code.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            QString cap0 = cap.captured(0);
            static const QRegularExpression fourSpaceBlockReg = QRegularExpression(QStringLiteral("^ {4}"), QRegularExpression::MultilineOption);
            cap0.replace(fourSpaceBlockReg, emptyStr);
            static const QRegularExpression newLineReg = QRegularExpression(QStringLiteral("\n+$"));
            const QString text = cap0.replace(newLineReg, emptyStr);

            const QVariantMap tok{{QStringLiteral("type"), QStringLiteral("code")}, {QStringLiteral("text"), text}};
            m_parser->tokens.append(tok);
            continue;
        }

        cap = block_fences.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            const QString text = cap.captured(3);
            const QString lang = cap.captured(2).trimmed();
            const QVariantMap tok{{QStringLiteral("type"), QStringLiteral("code")}, {QStringLiteral("text"), text}, {QStringLiteral("lang"), lang}};
            m_parser->tokens.append(tok);
            if (KleverConfig::pumlEnabled() && (lang.toLower() == QStringLiteral("puml") || lang.toLower() == QStringLiteral("plantuml"))) {
                pumlParserUtils->addToNotePUMLBlock(text);
            } else if (KleverConfig::codeSynthaxHighlightEnabled() && !lang.isEmpty()) { // Send only the value that will be highlighted
                highlightParserUtils->addToNoteCodeBlocks(text);
            }
            continue;
        }

        cap = block_heading.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            if (KleverConfig::noteMapEnabled()) {
                mapperParserUtils->addToNoteHeaders(cap.captured(0).trimmed());
            }

            const QVariantMap tok{{QStringLiteral("type"), QStringLiteral("heading")},
                                  {QStringLiteral("depth"), cap.capturedLength(1)},
                                  {QStringLiteral("text"), cap.captured(2)}};
            m_parser->tokens.append(tok);
            continue;
        }

        cap = block_nptable.match(remaining);
        if (top && cap.hasMatch()) {
            static const QRegularExpression headerPipeReg = QRegularExpression(QStringLiteral("^ *| *\\| *$"));
            const QStringList headerList = splitCells(cap.captured(1).replace(headerPipeReg, emptyStr));

            static const QRegularExpression alignReg = QRegularExpression(QStringLiteral("^ *|\\| *$"));
            static const QRegularExpression alignSplitterReg = QRegularExpression(QStringLiteral(" *\\| *"));
            QStringList alignList = cap.captured(2).replace(alignReg, emptyStr).split(alignSplitterReg);
            if (alignList.last().isEmpty())
                alignList.removeLast();

            static const QRegularExpression endingNewLineReg = QRegularExpression(QStringLiteral("\n$"));
            QStringList allCells = cap.captured(3).replace(endingNewLineReg, emptyStr).split(QStringLiteral("\n"));
            if (allCells.last().isEmpty())
                allCells.removeLast();

            QJsonArray cells;

            const int headerSize = headerList.size();
            const int alignSize = alignList.size();
            if (headerSize == alignSize) {
                remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

                for (int i = 0; i < alignSize; i++) {
                    static const QRegularExpression rightAlignReg = QRegularExpression(QStringLiteral("^ *-+: *$"));
                    static const QRegularExpression centerAlignReg = QRegularExpression(QStringLiteral("^ *:-+: *$"));
                    static const QRegularExpression leftAlignReg = QRegularExpression(QStringLiteral("^ *:-+ *$"));
                    if (rightAlignReg.match(alignList[i]).hasMatch()) {
                        alignList[i] = QStringLiteral("right");
                    } else if (centerAlignReg.match(alignList[i]).hasMatch()) {
                        alignList[i] = QStringLiteral("center");
                    } else if (leftAlignReg.match(alignList[i]).hasMatch()) {
                        alignList[i] = QStringLiteral("left");
                    } else {
                        alignList[i] = emptyStr;
                    }
                }
                for (int i = 0; i < allCells.size(); i++) {
                    const auto cellsList = QJsonArray::fromStringList(splitCells(allCells[i], headerSize));
                    cells.append(QJsonValue(cellsList));
                }
                const QVariantMap item{{QStringLiteral("type"), QStringLiteral("table")},
                                       {QStringLiteral("header"), headerList},
                                       {QStringLiteral("align"), alignList},
                                       {QStringLiteral("cells"), cells}};
                m_parser->tokens.append(item);
                continue;
            }
        }

        cap = block_hr.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            static const QVariantMap tok{{QStringLiteral("type"), QStringLiteral("hr")}};
            m_parser->tokens.append(tok);
            continue;
        }

        cap = block_blockquote.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            static const QVariantMap startingTok{{QStringLiteral("type"), QStringLiteral("blockquote_start")}};
            m_parser->tokens.append(startingTok);

            QString cap0 = cap.captured(0);
            static const QRegularExpression quoteBlockReg = QRegularExpression(QStringLiteral("^ *> ?"), QRegularExpression::MultilineOption);
            cap0.replace(quoteBlockReg, emptyStr);

            tokenize(cap0, top);

            static const QVariantMap endingTok{{QStringLiteral("type"), QStringLiteral("blockquote_end")}};
            m_parser->tokens.append(endingTok);
            continue;
        }

        cap = block_list.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.captured(0), emptyStr);

            QString bull = cap.captured(2);
            const bool isOrdered = bull.length() > 1;
            static const QString dotStr = QStringLiteral(".");
            if (bull.endsWith(dotStr))
                bull.remove(dotStr);

            const QVariantMap tok{{QStringLiteral("type"), QStringLiteral("list_start")},
                                  {QStringLiteral("ordered"), isOrdered},
                                  {QStringLiteral("start"), isOrdered ? bull : emptyStr}};
            m_parser->tokens.append(tok);

            QRegularExpressionMatchIterator globalCap = block_item.globalMatch(cap.captured(0));
            bool next = false;

            while (globalCap.hasNext()) {
                const auto matchedItem = globalCap.next();
                QString item = matchedItem.captured();

                int space = item.length();
                static const QRegularExpression bulletReg = QRegularExpression(QStringLiteral("^ *([*+-]|\\d+\\.) +"));
                const QRegularExpressionMatch firstBulletCatch = bulletReg.match(item);
                item.replace(firstBulletCatch.capturedStart(), firstBulletCatch.capturedLength(), emptyStr);

                if (item.indexOf(QStringLiteral("\n ")) != -1) {
                    space -= item.length();
                    static const QRegularExpression multiSpaceBlockReg =
                        QRegularExpression(QStringLiteral("^ {1,") + QString::number(space) + QStringLiteral("}"), QRegularExpression::MultilineOption);
                    item.replace(multiSpaceBlockReg, emptyStr);
                }

                static const QRegularExpression looseItemReg = QRegularExpression(QStringLiteral("\n\n(?!\\s*$)"));
                bool loose = next || looseItemReg.match(item).hasMatch();

                if (globalCap.hasNext()) {
                    next = !item.isEmpty() && item[item.length() - 1] == QChar::fromLatin1('\n');
                    if (!loose) {
                        loose = next;
                    }
                }

                static const QRegularExpression taskCatcherReg = QRegularExpression(QStringLiteral("(^\\[[ xX]\\] )"));
                const QRegularExpressionMatch taskCatcher = taskCatcherReg.match(item);
                const bool istask = taskCatcher.hasMatch();
                bool ischecked = false;
                if (istask) {
                    ischecked = item[1] != QChar::fromLatin1(' ');
                    item.replace(taskCatcher.capturedStart(1), taskCatcher.capturedLength(1), emptyStr);
                }
                const QVariantMap startingTok{{QStringLiteral("type"), loose ? QStringLiteral("loose_item_start") : QStringLiteral("list_item_start")},
                                              {QStringLiteral("task"), istask},
                                              {QStringLiteral("checked"), ischecked}};
                m_parser->tokens.append(startingTok);

                tokenize(item, false);

                static const QVariantMap endingTok{{QStringLiteral("type"), QStringLiteral("list_item_end")}};
                m_parser->tokens.append(endingTok);
            }
            static const QVariantMap endingTok{{QStringLiteral("type"), QStringLiteral("list_end")}};
            m_parser->tokens.append(endingTok);

            continue;
        }

        cap = block_html.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            const bool pre =
                (cap.captured(1) == QStringLiteral("pre") || cap.captured(1) == QStringLiteral("script") || cap.captured(1) == QStringLiteral("type"));

            const QVariantMap tok{{QStringLiteral("type"), QStringLiteral("html")}, {QStringLiteral("pre"), pre}, {QStringLiteral("text"), cap.captured(0)}};
            m_parser->tokens.append(tok);
            continue;
        }

        cap = block_def.match(remaining);
        if (top && cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            static const QRegularExpression whiteSpaceReg = QRegularExpression(QStringLiteral("\\s+"));
            const QString tag = cap.captured(1).toLower().replace(whiteSpaceReg, emptyStr);
            if (!m_parser->links.contains(tag)) {
                const QMap<QString, QString> link{{QStringLiteral("href"), cap.captured(2)}, {QStringLiteral("title"), cap.captured(3)}};
                m_parser->links.insert(tag, link);
            }
            continue;
        }

        cap = block_table.match(remaining);
        if (top && cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            static const QRegularExpression headerPipeReg = QRegularExpression(QStringLiteral("^ *| *$"));
            const QStringList headerList = splitCells(cap.captured(1).replace(headerPipeReg, emptyStr));

            static const QRegularExpression alignReg = QRegularExpression(QStringLiteral("^ *|\\| *$"));
            static const QRegularExpression alignSplitterReg = QRegularExpression(QStringLiteral(" *\\| *"));
            QStringList alignList = cap.captured(2).replace(alignReg, emptyStr).split(alignSplitterReg);
            if (alignList.last().isEmpty())
                alignList.removeLast();

            static const QRegularExpression cellReg = QRegularExpression(QStringLiteral("(?: *\\| *)?\n$"));
            QStringList allCells = cap.captured(3).replace(cellReg, emptyStr).split(QStringLiteral("\n"));
            if (allCells.last().isEmpty())
                allCells.removeLast();

            QJsonArray cells;

            const int headerSize = headerList.size();
            const int alignSize = alignList.size();
            if (headerSize == alignSize) {
                for (int i = 0; i < alignSize; i++) {
                    static const QRegularExpression rightAlignReg = QRegularExpression(QStringLiteral("^ *-+: *$"));
                    static const QRegularExpression centerAlignReg = QRegularExpression(QStringLiteral("^ *:-+: *$"));
                    static const QRegularExpression leftAlignReg = QRegularExpression(QStringLiteral("^ *:-+ *$"));
                    if (rightAlignReg.match(alignList[i]).hasMatch()) {
                        alignList[i] = QStringLiteral("right");
                    } else if (centerAlignReg.match(alignList[i]).hasMatch()) {
                        alignList[i] = QStringLiteral("center");
                    } else if (leftAlignReg.match(alignList[i]).hasMatch()) {
                        alignList[i] = QStringLiteral("left");
                    } else {
                        alignList[i] = emptyStr;
                    }
                }

                QJsonArray cellsList;
                const int cellsSize = allCells.size();
                for (int i = 0; i < cellsSize; i++) {
                    static const QRegularExpression tableCellReg = QRegularExpression(QStringLiteral("^ *\\| *| *\\| *$"));
                    cellsList = QJsonArray::fromStringList(splitCells(allCells[i].replace(tableCellReg, emptyStr), headerSize));
                    cells.append(QJsonValue(cellsList));
                }

                const QVariantMap item{{QStringLiteral("type"), QStringLiteral("table")},
                                       {QStringLiteral("header"), headerList},
                                       {QStringLiteral("align"), alignList},
                                       {QStringLiteral("cells"), cells}};
                m_parser->tokens.append(item);
                continue;
            }
        }

        cap = block_lheading.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            const int depth = cap.captured(2) == QStringLiteral("=") ? 1 : 2;

            const QVariantMap tok{{QStringLiteral("type"), QStringLiteral("heading")},
                                  {QStringLiteral("depth"), depth},
                                  {QStringLiteral("text"), cap.captured(1)}};
            m_parser->tokens.append(tok);
            continue;
        }

        cap = block_paragraph.match(remaining);
        if (top && cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            const QString text = cap.captured(1).endsWith(QStringLiteral("\n")) ? cap.captured(1).left(cap.captured(1).length() - 1) : cap.captured(1);

            const QVariantMap tok{{QStringLiteral("type"), QStringLiteral("paragraph")}, {QStringLiteral("text"), text}};
            m_parser->tokens.append(tok);
            continue;
        }

        cap = block_text.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            const QVariantMap tok{{QStringLiteral("type"), QStringLiteral("text")}, {QStringLiteral("text"), cap.captured(0)}};
            m_parser->tokens.append(tok);
            continue;
        }

        if (!remaining.isEmpty()) {
            qFatal("Infinite loop on byte: %d", remaining[0].unicode());
        }
    }
}

QStringList BlockLexer::splitCells(QString &tableRow, const int count) const
{
    static const QRegularExpression cellReg = QRegularExpression(QStringLiteral("([^\\\\])\\|"));
    static const QRegularExpression cellSplitterReg = QRegularExpression(QStringLiteral(" +\\| *"));
    QStringList cells = tableRow.replace(cellReg, QStringLiteral("\\1 |")).split(cellSplitterReg);
    if (cells.last().isEmpty())
        cells.removeLast();

    if (cells.length() > count && count > -1) {
        cells.erase(cells.end() - count, cells.end());
    } else {
        while (cells.length() < count) {
            cells.append(QLatin1String());
        }
    }

    for (int i = 0; i < cells.length(); i++) {
        static const QRegularExpression pipeReg = QRegularExpression(QStringLiteral("\\\\\\|"));
        cells[i] = cells[i].replace(pipeReg, QStringLiteral("|"));
    }

    return cells;
}
