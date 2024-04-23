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

void BlockLexer::reset()
{
    m_offSetPosHolder.clear();
    m_tokenEndPos = 0;
    m_overallOffSetSize = 0;
}

void BlockLexer::lex(QString &src)
{
    reset();
    src = preprocess(src);
    // qDebug() << src.length();
    setNextOffSetPos();
    tokenize(src, true);
    // qDebug() << m_tokenEndPos;
}

void BlockLexer::setNextOffSetPos()
{
    m_currentOffSetPos = m_offSetPosHolder.isEmpty() ? -1 : m_offSetPosHolder.takeLast();
}

QString BlockLexer::preprocess(QString &src)
{
    int currentOffSet = 0;
    for (const auto &pat : preprocessRegex.toStdMap()) {
        int matchPos = src.indexOf(pat.second, 0);
        if (matchPos != -1) {
            while (matchPos != -1) {
                const int offSetPos = matchPos + currentOffSet;
                m_offSetPosHolder.append(offSetPos);
                currentOffSet += m_offSetSize;

                matchPos = src.indexOf(pat.second, matchPos + 1);
            }
            src = src.replace(pat.second, pat.first);
        }
    }

    // This way we can pop
    std::reverse(m_offSetPosHolder.begin(), m_offSetPosHolder.end());

    return src;
};

void BlockLexer::checkOffSet(const int capturedLength)
{
    const int tokenStartPos = m_tokenEndPos;
    m_tokenEndPos += capturedLength;
    while (tokenStartPos <= m_currentOffSetPos && m_currentOffSetPos + m_offSetSize < m_tokenEndPos) {
        setNextOffSetPos();
        m_overallOffSetSize += m_offSetSize;
    }
}

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

            checkOffSet(cap.capturedLength());

            if (cap.capturedLength() > 1) {
                static const QVariantMap tok{{QStringLiteral("type"), QStringLiteral("space")}};
                m_parser->addToken(tok);
            }
            continue;
        }

        cap = block_fences.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            checkOffSet(cap.capturedLength());

            const QString text = cap.captured(3);
            const QString lang = cap.captured(2).trimmed();
            const QVariantMap tok{
                {QStringLiteral("type"), QStringLiteral("code")},
                {QStringLiteral("text"), text},
                {QStringLiteral("lang"), lang},
            };
            m_parser->addToken(tok);
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

            const int adjustedStartOfTag = m_tokenEndPos - m_overallOffSetSize;

            // Take the leading spaces into account
            const int tagLength = cap.capturedStart(1) + cap.capturedLength(1);
            checkOffSet(tagLength);

            static const QString headingTagStr = QStringLiteral("headingTag");
            // The tagLength was is added to m_tokenEndPos in checkOffSet
            const int adjustedEndOfTag = m_tokenEndPos - m_overallOffSetSize;
            const auto tagHighlightInfo = std::make_tuple(headingTagStr, adjustedStartOfTag, adjustedEndOfTag);
            m_parser->addHighlightToken(tagHighlightInfo);

            const int endOfCap = cap.capturedLength() - tagLength;
            checkOffSet(endOfCap);

            static const QString headingStr = QStringLiteral("heading");
            const int adjustedEndOfContent = m_tokenEndPos - m_overallOffSetSize;
            const auto contentHighlightInfo = std::make_tuple(headingStr, adjustedEndOfTag, adjustedEndOfContent);
            m_parser->addHighlightToken(contentHighlightInfo);

            const QVariantMap tok{
                {QStringLiteral("type"), headingStr},
                {QStringLiteral("depth"), cap.capturedLength(1)},
                {QStringLiteral("text"), cap.captured(2)},
            };
            m_parser->addToken(tok);

            // PLUGIN
            if (KleverConfig::noteMapEnabled()) {
                mapperParserUtils->addToNoteHeaders(cap.captured(0).trimmed());
            }
            continue;
        }

        cap = block_nptable.match(remaining);
        if (top && cap.hasMatch()) {
            static const QRegularExpression headerPipeReg = QRegularExpression(QStringLiteral("^ *| *\\| *$"));
            const QStringList headerList = splitCells(cap.captured(1).replace(headerPipeReg, emptyStr));

            static const QRegularExpression alignReg = QRegularExpression(QStringLiteral("^ *|\\| *$"));
            static const QRegularExpression alignSplitterReg = QRegularExpression(QStringLiteral(" *\\| *"));
            QStringList alignList = cap.captured(2).replace(alignReg, emptyStr).split(alignSplitterReg);
            if (alignList.last().isEmpty()) {
                alignList.removeLast();
            }

            static const QRegularExpression endingNewLineReg = QRegularExpression(QStringLiteral("\n$"));
            QStringList allCells = cap.captured(3).replace(endingNewLineReg, emptyStr).split(QStringLiteral("\n"));
            if (allCells.last().isEmpty()) {
                allCells.removeLast();
            }

            QJsonArray cells;

            const int headerSize = headerList.size();
            const int alignSize = alignList.size();
            if (headerSize == alignSize) {
                remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

                checkOffSet(cap.capturedLength());

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
                const QVariantMap item{
                    {QStringLiteral("type"), QStringLiteral("table")},
                    {QStringLiteral("header"), headerList},
                    {QStringLiteral("align"), alignList},
                    {QStringLiteral("cells"), cells},
                };
                m_parser->addToken(item);
                continue;
            }
        }

        cap = block_hr.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            checkOffSet(cap.capturedLength());

            static const QVariantMap tok{{QStringLiteral("type"), QStringLiteral("hr")}};
            m_parser->addToken(tok);
            continue;
        }

        cap = block_blockquote.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            static const QVariantMap startingTok{{QStringLiteral("type"), QStringLiteral("blockquote_start")}};
            m_parser->addToken(startingTok);

            QString cap0 = cap.captured(0);

            static const QRegularExpression quoteBlockReg = QRegularExpression(QStringLiteral("^ *> ?"), QRegularExpression::MultilineOption);
            cap0.replace(quoteBlockReg, emptyStr);

            const int preProcessSize = cap.capturedLength();
            const int postProcessSize = cap0.length();
            m_tokenEndPos += preProcessSize - postProcessSize;

            tokenize(cap0, top);

            static const QVariantMap endingTok{{QStringLiteral("type"), QStringLiteral("blockquote_end")}};
            m_parser->addToken(endingTok);
            continue;
        }

        cap = block_list.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            QString bull = cap.captured(2);

            const bool isOrdered = bull.length() > 1;
            static const QString dotStr = QStringLiteral(".");
            if (bull.endsWith(dotStr)) {
                bull.remove(dotStr);
            }

            const QVariantMap tok{
                {QStringLiteral("type"), QStringLiteral("list_start")},
                {QStringLiteral("ordered"), isOrdered},
                {QStringLiteral("start"), isOrdered ? bull : emptyStr},
            };
            m_parser->addToken(tok);

            QRegularExpressionMatchIterator globalCap = block_item.globalMatch(cap.captured(0));
            bool next = false;

            // Important: mitigate overshoot
            m_tokenEndPos--;
            while (globalCap.hasNext()) {
                m_tokenEndPos += 1;

                const auto matchedItem = globalCap.next();
                QString item = matchedItem.captured();

                const int preProcessSize = item.length();

                int space = item.length();
                static const QRegularExpression bulletReg = QRegularExpression(QStringLiteral("^ *([*+-]|\\d+\\.) +"));
                const QRegularExpressionMatch firstBulletCatch = bulletReg.match(item);
                item.replace(firstBulletCatch.capturedStart(), firstBulletCatch.capturedLength(), emptyStr);

                if (item.indexOf(QStringLiteral("\n ")) != -1) {
                    space -= item.length();
                    // TODO rework this line to prevent static on a changing var (space is not static !!!)
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

                const int postProcessSize = item.length();
                m_tokenEndPos += preProcessSize - postProcessSize;

                const QVariantMap startingTok{
                    {QStringLiteral("type"), loose ? QStringLiteral("loose_item_start") : QStringLiteral("list_item_start")},
                    {QStringLiteral("task"), istask},
                    {QStringLiteral("checked"), ischecked},
                };
                m_parser->addToken(startingTok);

                tokenize(item, false);

                static const QVariantMap endingTok{{QStringLiteral("type"), QStringLiteral("list_item_end")}};
                m_parser->addToken(endingTok);
            }
            static const QVariantMap endingTok{{QStringLiteral("type"), QStringLiteral("list_end")}};
            m_parser->addToken(endingTok);

            continue;
        }

        cap = block_html.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            checkOffSet(cap.capturedLength());

            const bool pre =
                (cap.captured(1) == QStringLiteral("pre") || cap.captured(1) == QStringLiteral("script") || cap.captured(1) == QStringLiteral("type"));

            const QVariantMap tok{
                {QStringLiteral("type"), QStringLiteral("html")},
                {QStringLiteral("pre"), pre},
                {QStringLiteral("text"), cap.captured(0)},
            };
            m_parser->addToken(tok);
            continue;
        }

        cap = block_def.match(remaining);
        if (top && cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            checkOffSet(cap.capturedLength());

            static const QRegularExpression whiteSpaceReg = QRegularExpression(QStringLiteral("\\s+"));
            const QString tag = cap.captured(1).toLower().replace(whiteSpaceReg, emptyStr);
            if (!m_parser->tagExists(tag)) {
                const QMap<QString, QString> link{{QStringLiteral("href"), cap.captured(2)}, {QStringLiteral("title"), cap.captured(3)}};
                m_parser->addLink(tag, link);
            }
            continue;
        }

        cap = block_table.match(remaining);
        if (top && cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            checkOffSet(cap.capturedLength());

            static const QRegularExpression headerPipeReg = QRegularExpression(QStringLiteral("^ *| *$"));
            const QStringList headerList = splitCells(cap.captured(1).replace(headerPipeReg, emptyStr));

            static const QRegularExpression alignReg = QRegularExpression(QStringLiteral("^ *|\\| *$"));
            static const QRegularExpression alignSplitterReg = QRegularExpression(QStringLiteral(" *\\| *"));
            QStringList alignList = cap.captured(2).replace(alignReg, emptyStr).split(alignSplitterReg);
            if (alignList.last().isEmpty()) {
                alignList.removeLast();
            }

            static const QRegularExpression cellReg = QRegularExpression(QStringLiteral("(?: *\\| *)?\n$"));
            QStringList allCells = cap.captured(3).replace(cellReg, emptyStr).split(QStringLiteral("\n"));
            if (allCells.last().isEmpty()) {
                allCells.removeLast();
            }

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

                const QVariantMap item{
                    {QStringLiteral("type"), QStringLiteral("table")},
                    {QStringLiteral("header"), headerList},
                    {QStringLiteral("align"), alignList},
                    {QStringLiteral("cells"), cells},
                };
                m_parser->addToken(item);
                continue;
            }
        }

        cap = block_lheading.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            checkOffSet(cap.capturedLength());

            const int depth = cap.captured(2) == QStringLiteral("=") ? 1 : 2;

            const QVariantMap tok{
                {QStringLiteral("type"), QStringLiteral("heading")},
                {QStringLiteral("depth"), depth},
                {QStringLiteral("text"), cap.captured(1)},
            };
            m_parser->addToken(tok);
            continue;
        }

        cap = block_paragraph.match(remaining);
        if (top && cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            checkOffSet(cap.capturedLength());

            const QString text = cap.captured(1).endsWith(QStringLiteral("\n")) ? cap.captured(1).left(cap.captured(1).length() - 1) : cap.captured(1);

            const QVariantMap tok{
                {QStringLiteral("type"), QStringLiteral("paragraph")},
                {QStringLiteral("text"), text},
            };
            m_parser->addToken(tok);
            continue;
        }

        cap = block_text.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            checkOffSet(cap.capturedLength());

            const QVariantMap tok{
                {QStringLiteral("type"), QStringLiteral("text")},
                {QStringLiteral("text"), cap.captured(0)},
            };
            m_parser->addToken(tok);
            continue;
        }

        if (!remaining.isEmpty()) {
            qFatal("Infinite loop on byte: %d", remaining[0].unicode());
        }
    }
}

QStringList BlockLexer::splitCells(QString &tableRow, const int count) const
{
    static const QString pipeUni = QStringLiteral("&#124");
    static const QString pipeEscape = QStringLiteral("\\|");
    static const QRegularExpression cellSplitterReg = QRegularExpression(QStringLiteral(" *\\| *"));
    QStringList cells = tableRow.replace(pipeEscape, pipeUni).split(cellSplitterReg);
    if (cells.last().isEmpty()) {
        cells.removeLast();
    }

    if (cells.length() > count && count > -1) {
        cells.erase(cells.end() - count, cells.end());
    } else {
        while (cells.length() < count) {
            cells.append(QLatin1String());
        }
    }

    for (int i = 0; i < cells.length(); i++) {
        cells[i] = cells[i].replace(pipeUni, QStringLiteral("|"));
    }

    return cells;
}
