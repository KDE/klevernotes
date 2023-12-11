/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#include "blockLexer.h"

#include <QJsonArray>
#include <QMap>
#include <QString>

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

    for (auto pat : preprocessRegex.toStdMap()) {
        cap = pat.second.match(src);
        while (cap.hasMatch()) {
            src = src.replace(pat.second, pat.first);
            cap = pat.second.match(src);
        }
    }

    return src;
};

void BlockLexer::tokenize(QString &remaining, bool top)
{
    while (!remaining.isEmpty()) {
        QRegularExpressionMatch cap;

        cap = block_newline.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

            if (cap.capturedLength() > 1) {
                const QVariantMap tok{{"type", "space"}};
                m_parser->tokens.append(tok);
            }
        }

        cap = block_code.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

            QString cap0 = cap.captured(0);
            cap0.replace(QRegularExpression("^ {4}", QRegularExpression::MultilineOption), "");
            const QString text = cap0.replace(QRegularExpression("\n+$"), "");

            const QVariantMap tok{{"type", "code"}, {"text", text}};
            m_parser->tokens.append(tok);
            continue;
        }

        cap = block_fences.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

            const QString text = cap.captured(3);
            const QString lang = cap.captured(2);
            const QVariantMap tok{{"type", "code"}, {"text", text}, {"lang", lang}};
            m_parser->tokens.append(tok);
            if (m_parser->highlightEnabled() && !lang.isEmpty()) { // Send only the value that will be highlighted
                m_parser->addToNoteCodeBlocks(text);
            }
            continue;
        }

        cap = block_heading.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

            if (m_parser->noteMapEnabled()) {
                m_parser->addToNoteHeaders(cap.captured(0).trimmed());
            }

            const QVariantMap tok{{"type", "heading"}, {"depth", cap.capturedLength(1)}, {"text", cap.captured(2)}};
            m_parser->tokens.append(tok);
            continue;
        }

        cap = block_nptable.match(remaining);
        if (top && cap.hasMatch()) {
            const QStringList headerList = splitCells(cap.captured(1).replace(QRegularExpression("^ *| *\\| *$"), ""));

            QStringList alignList = cap.captured(2).replace(QRegularExpression("^ *|\\| *$"), "").split(QRegularExpression(" *\\| *"));
            if (alignList.last().isEmpty())
                alignList.removeLast();

            QStringList allCells = cap.captured(3).replace(QRegularExpression("\n$"), "").split("\n");
            if (allCells.last().isEmpty())
                allCells.removeLast();

            QJsonArray cells;

            const int headerSize = headerList.size();
            const int alignSize = alignList.size();
            if (headerSize == alignSize) {
                remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

                for (int i = 0; i < alignSize; i++) {
                    if (QRegularExpression("^ *-+: *$").match(alignList[i]).hasMatch()) {
                        alignList[i] = "right";
                    } else if (QRegularExpression("^ *:-+: *$").match(alignList[i]).hasMatch()) {
                        alignList[i] = "center";
                    } else if (QRegularExpression("^ *:-+ *$").match(alignList[i]).hasMatch()) {
                        alignList[i] = "left";
                    } else {
                        alignList[i] = "";
                    }
                }
                for (int i = 0; i < allCells.size(); i++) {
                    const auto cellsList = QJsonArray::fromStringList(splitCells(allCells[i], headerSize));
                    cells.append(QJsonValue(cellsList));
                }
                const QVariantMap item{{"type", "table"}, {"header", headerList}, {"align", alignList}, {"cells", cells}};
                m_parser->tokens.append(item);
                continue;
            }
        }

        cap = block_hr.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

            const QVariantMap tok{{"type", "hr"}};
            m_parser->tokens.append(tok);
            continue;
        }

        cap = block_blockquote.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

            const QVariantMap startingTok{{"type", "blockquote_start"}};
            m_parser->tokens.append(startingTok);

            QString cap0 = cap.captured(0);
            cap0.replace(QRegularExpression("^ *> ?", QRegularExpression::MultilineOption), "");

            tokenize(cap0, top);

            const QVariantMap endingTok{{"type", "blockquote_end"}};
            m_parser->tokens.append(endingTok);
            continue;
        }

        cap = block_list.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.captured(0), "");

            QString bull = cap.captured(2);
            const bool isOrdered = bull.length() > 1;
            if (bull.endsWith("."))
                bull.remove(".");

            const QVariantMap tok{{"type", "list_start"}, {"ordered", isOrdered}, {"start", isOrdered ? bull : ""}};
            m_parser->tokens.append(tok);

            QRegularExpressionMatchIterator globalCap = block_item.globalMatch(cap.captured(0));
            bool next = false;

            while (globalCap.hasNext()) {
                auto matchedItem = globalCap.next();
                QString item = matchedItem.captured();

                int space = item.length();
                QRegularExpressionMatch firstBulletCatch = QRegularExpression("^ *([*+-]|\\d+\\.) +").match(item);
                item.replace(firstBulletCatch.capturedStart(), firstBulletCatch.capturedLength(), "");

                if (item.indexOf("\n ") != -1) {
                    space -= item.length();
                    item.replace(QRegularExpression("^ {1," + QString::number(space) + "}", QRegularExpression::MultilineOption), "");
                }

                bool loose = next || QRegularExpression("\n\n(?!\\s*$)").match(item).hasMatch();

                if (globalCap.hasNext()) {
                    next = !item.isEmpty() && item[item.length() - 1] == QChar::fromLatin1('\n');
                    if (!loose) {
                        loose = next;
                    }
                }

                QRegularExpressionMatch taskCatcher = QRegularExpression("(^\\[[ xX]\\] )").match(item);
                const bool istask = taskCatcher.hasMatch();
                bool ischecked = false;
                if (istask) {
                    ischecked = item[1] != QChar::fromLatin1(' ');
                    item.replace(taskCatcher.capturedStart(1), taskCatcher.capturedLength(1), "");
                }
                const QVariantMap startingTok{{"type", loose ? "loose_item_start" : "list_item_start"}, {"task", istask}, {"checked", ischecked}};
                m_parser->tokens.append(startingTok);

                tokenize(item, false);

                const QVariantMap endingTok{{"type", "list_item_end"}};
                m_parser->tokens.append(endingTok);
            }
            const QVariantMap endingTok{{"type", "list_end"}};
            m_parser->tokens.append(endingTok);

            continue;
        }

        cap = block_html.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

            const bool pre = (cap.captured(1) == "pre" || cap.captured(1) == "script" || cap.captured(1) == "type");

            const QVariantMap tok{{"type", "html"}, {"pre", pre}, {"text", cap.captured(0)}};
            m_parser->tokens.append(tok);
            continue;
        }

        cap = block_def.match(remaining);
        if (top && cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

            const QString tag = cap.captured(1).toLower().replace(QRegularExpression("\\s+"), "");
            if (!m_parser->links.contains(tag)) {
                const QMap<QString, QString> link{{"href", cap.captured(2)}, {"title", cap.captured(3)}};
                m_parser->links.insert(tag, link);
            }
            continue;
        }

        cap = block_table.match(remaining);
        if (top && cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

            const QStringList headerList = splitCells(cap.captured(1).replace(QRegularExpression("^ *| *$"), ""));

            QStringList alignList = cap.captured(2).replace(QRegularExpression("^ *|\\| *$"), "").split(QRegularExpression(" *\\| *"));
            if (alignList.last().isEmpty())
                alignList.removeLast();

            QStringList allCells = cap.captured(3).replace(QRegularExpression("(?: *\\| *)?\n$"), "").split("\n");
            if (allCells.last().isEmpty())
                allCells.removeLast();

            QJsonArray cells;

            const int headerSize = headerList.size();
            const int alignSize = alignList.size();
            if (headerSize == alignSize) {
                for (int i = 0; i < alignSize; i++) {
                    if (QRegularExpression("^ *-+: *$").match(alignList[i]).hasMatch()) {
                        alignList[i] = "right";
                    } else if (QRegularExpression("^ *:-+: *$").match(alignList[i]).hasMatch()) {
                        alignList[i] = "center";
                    } else if (QRegularExpression("^ *:-+ *$").match(alignList[i]).hasMatch()) {
                        alignList[i] = "left";
                    } else {
                        alignList[i] = "";
                    }
                }

                QJsonArray cellsList;
                const int cellsSize = allCells.size();
                for (int i = 0; i < cellsSize; i++) {
                    cellsList = QJsonArray::fromStringList(splitCells(allCells[i].replace(QRegularExpression("^ *\\| *| *\\| *$"), ""), headerSize));
                    cells.append(QJsonValue(cellsList));
                }

                const QVariantMap item{{"type", "table"}, {"header", headerList}, {"align", alignList}, {"cells", cells}};
                m_parser->tokens.append(item);
                continue;
            }
        }

        cap = block_lheading.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

            const int depth = cap.captured(2) == "=" ? 1 : 2;

            const QVariantMap tok{{"type", "heading"}, {"depth", depth}, {"text", cap.captured(1)}};
            m_parser->tokens.append(tok);
            continue;
        }

        cap = block_paragraph.match(remaining);
        if (top && cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

            const QString text = cap.captured(1).endsWith('\n') ? cap.captured(1).left(cap.captured(1).length() - 1) : cap.captured(1);

            const QVariantMap tok{{"type", "paragraph"}, {"text", text}};
            m_parser->tokens.append(tok);
            continue;
        }

        cap = block_text.match(remaining);
        if (cap.hasMatch()) {
            remaining.replace(cap.capturedStart(), cap.capturedLength(), "");

            const QVariantMap tok{{"type", "text"}, {"text", cap.captured(0)}};
            m_parser->tokens.append(tok);
            continue;
        }

        if (!remaining.isEmpty()) {
            qFatal("Infinite loop on byte: %d", remaining[0].unicode());
        }
    }
}

QStringList BlockLexer::splitCells(QString &tableRow, int count) const
{
    QStringList cells = tableRow.replace(QRegularExpression("([^\\\\])\\|"), "\\1 |").split(QRegularExpression(" +\\| *"));
    if (cells.last().isEmpty())
        cells.removeLast();

    if (cells.length() > count && count > -1) {
        cells.erase(cells.end() - count, cells.end());
    } else {
        while (cells.length() < count) {
            cells.append("");
        }
    }

    for (int i = 0; i < cells.length(); i++) {
        cells[i] = cells[i].replace(QRegularExpression("\\\\\\|"), "|");
    }

    return cells;
}
