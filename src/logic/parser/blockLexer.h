/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include <QRegularExpression>
#include <QString>

class Parser;

class BlockLexer
{
public:
    explicit BlockLexer(Parser *parser);

    void lex(QString &src);

private:
    QString preprocess(QString &src) const;
    void tokenize(QString &src, bool top);
    QStringList splitCells(QString &tableRow, int count = -1) const;

    QMap<QString, QRegularExpression> preprocessRegex{{"\n", QRegularExpression("\r\n|\r|\u2424")},
                                                      {"    ", QRegularExpression("\t")},
                                                      {" ", QRegularExpression("\u00a0")},
                                                      {"", QRegularExpression("^ +$")}};

    QRegularExpression block_newline = QRegularExpression("^\n+");

    QRegularExpression block_code = QRegularExpression("^( {4}[^\n]+\n*)+");

    QRegularExpression block_fences = QRegularExpression("^ *(\\`{3,}|~{3,})[ \\.]*(\\S+)? *\n([\\s\\S]*?)\n? *\\1 *(?:\n+|$)");

    QRegularExpression block_heading = QRegularExpression("^ *(#{1,6}) *([^\n]+?) *(?:#+ *)?(?:\n+|$)");

    QRegularExpression block_nptable = QRegularExpression("^ *([^|\n ].*\\|.*)\n *([-:]+ *\\|[-| :]*)(?:\n((?:.*[^&gt;\n ].*(?:\n|$))*)\n*|$)");

    QRegularExpression block_hr = QRegularExpression("^ {0,3}((?:- *){3,}|(?:_ *){3,}|(?:\\* *){3,})(?:\n+|$)");

    QRegularExpression block_blockquote = QRegularExpression(
        "^( {0,3}> ?(([^\n]+(?:\n(?! {0,3}((?:- *){3,}|(?:_ *){3,}|(?:\\* *){3,})(?:\n+|$)| *(#{1,6}) *([^\\n]+?) *(?:#+ *)?(?:\n+|$)|([^\\n]+)\n *(=|-){2,} "
        "*(?:\n+|$)| "
        "{0,3}>|<\\/"
        "?(?:address|article|aside|base|basefont|blockquote|body|caption|center|col|colgroup|dd|details|dialog|dir|div|dl|dt|fieldset|figcaption|figure|footer|"
        "form|frame|frameset|h[1-6]|head|header|hr|html|iframe|legend|li|link|main|menu|menuitem|meta|nav|noframes|ol|optgroup|option|p|param|section|source|"
        "summary|table|tbody|td|tfoot|th|thead|title|tr|track|ul)(?: +|\\n|\\/?>)|<(?:script|pre|style|!--))[^\n]+)*)|[^\n]*)(?:\\n|$))+");

    QRegularExpression block_list = QRegularExpression(
        "^( *)((?:[\\*\\+\\-]|\\d+\\.)) [\\s\\S]+?(?:\n+(?=\\1?(?:(?:- *){3,}|(?:_ *){3,}|(?:\\* *){3,})(?:\n+|$))|\n+(?= "
        "{0,3}\\[((?!\\s*\\])(?:\\\\[\\[\\]]|[^\\[\\]])+)\\]: *\n? *<!--?([^\\s\\-\\->]+)&gt;?(?:(?: +\n? *| *\n "
        "*)((?:\"(?:\\\\\"?|[^\"\\\\])*\"|'[^'\n]*(?:\n[^'\n]+)*\n?'|\\([^()]*\\))))? *(?:\n+|$))|\n{2,}(?! )(?!\1(?:[\\*\\+\\-]|\\d+\\.) )\n*|\\s*$)");

    QRegularExpression block_item =
        QRegularExpression("^( *)((?:[*+-]|\\d+\\.)) [^\\n]*(?:\\n(?!\\1(?:[*+-]|\\d+\\.) )[^\\n]*)*", QRegularExpression::MultilineOption);

    QRegularExpression block_html = QRegularExpression(
        "^ "
        "{0,3}(?:&lt;(script|pre|style)[\\s&gt;][\\s\\S]*?(?:&lt;\\/"
        "\1&gt;[^\n]*\n+|$)|<!--(?!-?>)[\\s\\S]*?-->[^\n]*(\n+|$)|&lt;\\?[\\s\\S]*?\\?&gt;\n*|<!--[A-Z][\\s\\S]*?-->\n*|<!--\\[CDATA\\[[\\s\\S]*?\\]\\]-->\n*|&"
        "lt;\\/"
        "?(address|article|aside|base|basefont|blockquote|body|caption|center|col|colgroup|dd|details|dialog|dir|div|dl|dt|fieldset|figcaption|figure|footer|"
        "form|frame|frameset|h[1-6]|head|header|hr|html|iframe|legend|li|link|main|menu|menuitem|meta|nav|noframes|ol|optgroup|option|p|param|section|source|"
        "summary|table|tbody|td|tfoot|th|thead|title|tr|track|ul)(?: +|\n|\\/?&gt;)[\\s\\S]*?(?:\n{2,}|$)|&lt;(?!script|pre|style)([a-z][\\w-]*)(?: "
        "+[a-zA-Z:_][\\w.:-]*(?: *= *\"[^\"\n]*\"| *= *'[^'\n]*'| *= *[^\\s\"'=&lt;&gt;`]+)?)*? "
        "*\\/?&gt;(?=\\h*\n)[\\s\\S]*?(?:\n{2,}|$)|&lt;\\/(?!script|pre|style)[a-z][\\w-]*\\s*&gt;(?=\\h*\n)[\\s\\S]*?(?:\n{2,}|$))",
        QRegularExpression::CaseInsensitiveOption);

    QRegularExpression block_def = QRegularExpression(
        "^ {0,3}\\[((?!\\s*\\])(?:\\\\[\\[\\]]|[^\\[\\]])+)\\]: *\\n? *<?([^\\s>]+)>?(?:(?: +\\n? *| *\\n "
        "*)((?:\"(?:\\\\\"?|[^\"\\\\])*\"|'[^'\n]*(?:\n[^'\n]+)*\n?'|\\([^()]*\\))))? *(?:\\n+|$)");

    QRegularExpression block_table = QRegularExpression("^ *\\|(.+)\n *\\|?( *[-:]+[-| :]*)(?:\n((?: *[^&gt;\n ].*(?:\n|$))*)\n*|$)");

    QRegularExpression block_lheading = QRegularExpression("^([^\n]+)\n *(=|-){2,} *(?:\n+|$)");

    QRegularExpression block_paragraph = QRegularExpression(
        "^([^\\n]+(?:\\n(?! *(`{3,}|~{3,})[ \\.]*(\\S+)? *\\n([\\s\\S]*?)\\n? *\\2 *(?:\\n+|$)|( *)((?:[*+-]|\\d+\\.)) [\\s\\S]+?(?:\\n+(?=\\3?(?:(?:- "
        "*){3,}|(?:_ *){3,}|(?:\\* *){3,})(?:\\n+|$))|\\n+(?= {0,3}\\[((?!\\s*\\])(?:\\\\[\\[\\]]|[^\\[\\]])+)\\]: *\\n? *<?([^\\s>]+)>?(?:(?: +\\n? *| *\\n "
        "*)((?:\"(?:\\\\\"?|[^\"\\\\])*\"|'[^'\\n]*(?:\\n[^'\\n]+)*\\n?'|\\([^()]*\\))))? *(?:\\n+|$))|\\n{2,}(?! )(?!\\1(?:[*+-]|\\d+\\.) )\\n*|\\s*$)| "
        "{0,3}((?:- *){3,}|(?:_ *){3,}|(?:\\* *){3,})(?:\\n+|$)| *(#{1,6}) *([^\\n]+?) *(?:#+ *)?(?:\\n+|$)|([^\\n]+)\\n *(=|-){2,} *(?:\\n+|$)| "
        "{0,3}>|<\\/"
        "?(?:address|article|aside|base|basefont|blockquote|body|caption|center|col|colgroup|dd|details|dialog|dir|div|dl|dt|fieldset|figcaption|figure|footer|"
        "form|frame|frameset|h[1-6]|head|header|hr|html|iframe|legend|li|link|main|menu|menuitem|meta|nav|noframes|ol|optgroup|option|p|param|section|source|"
        "summary|table|tbody|td|tfoot|th|thead|title|tr|track|ul)(?: +|\\n|\\/?>)|<(?:script|pre|style|!--))[^\\n]+)*)");

    QRegularExpression block_text = QRegularExpression("^[^\n]+");

    Parser *m_parser;
};
