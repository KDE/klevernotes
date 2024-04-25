/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#pragma once

#include <QList>
#include <QRegularExpression>

class Parser;

class BlockLexer
{
public:
    explicit BlockLexer(Parser *parser);

    void lex(QString &src);
    void setOffSetSize();

private:
    QString preprocess(QString &src);
    void tokenize(QString &src, const bool top);
    QStringList splitCells(QString &tableRow, const int count = -1) const;

    QMap<QString, QRegularExpression> preprocessRegex{
        // {QStringLiteral("\n"), QRegularExpression(QStringLiteral("\r\n|\r|\u2424"))},
        {QStringLiteral("    "), QRegularExpression(QStringLiteral("\t"))},
        // {QStringLiteral(" "), QRegularExpression(QStringLiteral("\u00a0"))},
        // {QStringLiteral(""), QRegularExpression(QStringLiteral("^ +$"))},
    };

    inline static const QRegularExpression block_newline = QRegularExpression(QStringLiteral("^\n+"));

    inline static const QRegularExpression block_fences = QRegularExpression(QStringLiteral("^ *(`{3})[ \\.]*(\\S+)? *\n([\\s\\S]*?)\n? *(`{3}) *(?:\n+|$)"));

    inline static const QRegularExpression block_heading = QRegularExpression(QStringLiteral("^ *(#{1,6}) *([^\n]+?) *(?:#+ *)?(?:\n+|$)"));

    inline static const QRegularExpression block_nptable =
        QRegularExpression(QStringLiteral("^ *([^|\n ].*\\|.*)\n *([-:]+ *\\|[-| :]*)(?:\n((?:.*[^>\n ].*(?:\n|$))*)\n*|$)"));

    inline static const QRegularExpression block_hr = QRegularExpression(QStringLiteral("^ {0,3}((?:- *){3,}|(?:_ *){3,}|(?:\\* *){3,})(?:\n+|$)"));

    inline static const QRegularExpression block_blockquote = QRegularExpression(QStringLiteral(
        "^( {0,3}> ?(^([^\n]+(?:\n(?!^ {0,3}((?:- *){3,}|(?:_ *){3,}|(?:\\* *){3,})(?:\n+|$)"
        "|^ *(#{1,6}) *([^\n]+?) *(?:#+ *)?(?:\n+|$)|^([^\n]+)\n *(=|-){2,} *(?:\n+|$)| {0,3}>"
        "|</?(?:address|article|aside|base|basefont|blockquote|body|caption|center|col|colgroup|dd|details|dialog|dir|div|dl|dt|fieldset|figcaption"
        "|figure|footer|form|frame|frameset|h[1-6]|head|header|hr|html|iframe|legend|li|link|main|menu|menuitem|meta|nav|noframes|ol|optgroup|option"
        "|p|param|section|source|summary|table|tbody|td|tfoot|th|thead|title|tr|track|ul)"
        "(?: +|\n|/?>)|<(?:script|pre|style|!--))[^\n]+)*)"
        "|[^\n]*)(?:\n|$))+"));

    inline static const QRegularExpression block_list = QRegularExpression(QStringLiteral(
        "^( *)((?:[\\*\\+\\-]|\\d+\\.)) [\\s\\S]+?(?:\n+(?=\\1?(?:(?:- *){3,}|(?:_ *){3,}|(?:\\* *){3,})(?:\n+|$))|\n+(?= "
        "{0,3}\\[((?!\\s*\\])(?:\\\\[\\[\\]]|[^\\[\\]])+)\\]: *\n? *<!--?([^\\s\\-\\->]+)>?(?:(?: +\n? *| *\n "
        "*)((?:\"(?:\\\\\"?|[^\"\\\\])*\"|'[^'\n]*(?:\n[^'\n]+)*\n?'|\\([^()]*\\))))? *(?:\n+|$))|\n{2,}(?! )(?!\\1(?:[\\*\\+\\-]|\\d+\\.) )\n*|\\s*$)"));

    inline static const QRegularExpression block_item =
        QRegularExpression(QStringLiteral("^( *)((?:[*+-]|\\d+\\.)) [^\\n]*(?:\\n(?!\\1(?:[*+-]|\\d+\\.) )[^\\n]*)*"), QRegularExpression::MultilineOption);

    inline static const QRegularExpression block_html = QRegularExpression(
        QStringLiteral("'^ "
                       "{0,3}(?:<(script|pre|style)[\\s>][\\s\\S]*?(?:</"
                       "\\1>[^\n]*\n+|$)|<!--(?!-?>)[\\s\\S]*?-->[^\n]*(\n+|$)|<\\?[\\s\\S]*?\\?>\n*|<![A-Z][\\s\\S]*?>\n*|<!\\[CDATA\\[[\\s\\S]*?\\]\\]>\n*|</"
                       "?(address|article|aside|base|basefont|blockquote|body|caption|center|col|colgroup|dd|details|dialog|dir|div|dl|dt|fieldset|figcaption|"
                       "figure|footer|form|frame|frameset|h[1-6]|head|header|hr|html|iframe|legend|li|link|main|menu|menuitem|meta|nav|noframes|ol|optgroup|"
                       "option|p|param|section|source|summary|table|tbody|td|tfoot|th|thead|title|tr|track|ul)(?: "
                       "+|\n|/?>)[\\s\\S]*?(?:\n{2,}|$)|<(?!script|pre|style)([a-z][\\w-]*)(?: +[a-zA-Z:_][\\w.:-]*(?: *= *\"[^\"\n]*\"| *= *'[^'\n]*'| *= "
                       "*[^\\s\"'=<>`]+)?)*? */?>(?=\\h*\n)[\\s\\S]*?(?:\n{2,}|$)|</(?!script|pre|style)[a-z][\\w-]*\\s*>(?=\\h*\n)[\\s\\S]*?(?:\n{2,}|$))'"),
        QRegularExpression::CaseInsensitiveOption);

    inline static const QRegularExpression block_def =
        QRegularExpression(QStringLiteral("^ {0,3}\\[((?!\\s*\\])(?:\\\\[\\[\\]]|[^\\[\\]])+)\\]: *\\n? *<?([^\\s>]+)>?(?:(?: +\\n? *| *\\n "
                                          "*)((?:\"(?:\\\\\"?|[^\"\\\\])*\"|'[^'\n]*(?:\n[^'\n]+)*\n?'|\\([^()]*\\))))? *(?:\\n+|$)"));

    inline static const QRegularExpression block_table =
        QRegularExpression(QStringLiteral("^ *\\|(.+)\n *\\|?( *[-:]+[-| :]*)(?:\n((?: *[^>\n ].*(?:\n|$))*)\n*|$)"));

    inline static const QRegularExpression block_lheading = QRegularExpression(QStringLiteral("^([^\n]+)\n *(=|-){2,} *(?:\n+|$)"));

    inline static const QRegularExpression block_paragraph = QRegularExpression(QStringLiteral(
        "^([^\n]+(?:\n(?! *(`{3,}|~{3,})[ \\.]*(\\S+)? *\n([\\s\\S]*?)\n? *\\2 *(?:\n+|$)|( *)((?:[*+-]|\\d+\\.)) [\\s\\S]+?(?:\n+(?=\\3?(?:(?:- "
        "*){3,}|(?:_ *){3,}|(?:\\* *){3,})(?:\n+|$))|\n+(?= {0,3}\\[((?!\\s*\\])(?:\\\\[\\[\\]]|[^\\[\\]])+)\\]: *\n? *<?([^\\s>]+)>?(?:(?: +\n? *| *\n"
        "*)((?:\"(?:\\\\\"?|[^\"\\\\])*\"|'[^'\n]*(?:\n[^'\n]+)*\n?'|\\([^()]*\\))))? *(?:\n+|$))|\n{2,}(?! )(?!\\1(?:[*+-]|\\d+\\.) )\n*|\\s*$)| "
        "{0,3}((?:- *){3,}|(?:_ *){3,}|(?:\\* *){3,})(?:\n+|$)| *(#{1,6}) *([^\n]+?) *(?:#+ *)?(?:\n+|$)|([^\n]+)\n *(=|-){2,} *(?:\n+|$)| "
        "{0,3}>|</"
        "?(?:address|article|aside|base|basefont|blockquote|body|caption|center|col|colgroup|dd|details|dialog|dir|div|dl|dt|fieldset|figcaption|figure|footer|"
        "form|frame|frameset|h[1-6]|head|header|hr|html|iframe|legend|li|link|main|menu|menuitem|meta|nav|noframes|ol|optgroup|option|p|param|section|source|"
        "summary|table|tbody|td|tfoot|th|thead|title|tr|track|ul)(?: +|\n|/?>)|<(?:script|pre|style|!--))[^\n]+)*)"));

    inline static const QRegularExpression block_text = QRegularExpression(QStringLiteral("^[^\n]+"));

    void reset();
    void setNextOffSetPos();
    void checkOffSet(const int capturedLength);

    int m_offSetSize;
    int m_tokenEndPos = 0;
    int m_overallOffSetSize = 0;
    int m_currentOffSetPos;
    QList<int> m_offSetPosHolder;

    Parser *m_parser;
};
