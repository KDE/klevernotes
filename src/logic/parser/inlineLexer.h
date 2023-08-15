#pragma once

#include <QRegularExpression>

class Parser;

class InlineLexer
{
public:
    InlineLexer(Parser *parser);

    QString output(QString &src, bool useInlineText = false);

private:
    QString mangle(QString &text);
    QString escapes(QString &text);
    QString outputLink(QRegularExpressionMatch &cap, QMap<QString, QString> linkInfo, bool useInlineText);

    QRegularExpression inline_escape = QRegularExpression("^\\\\([!\"#$%&'()*+,\\-.\\/:;<=>?@\\[\\]\\\\^_`{|}~~|])");

    QRegularExpression inline_autolink = QRegularExpression(
        "^<([a-zA-Z][a-zA-Z0-9+.-]{1,31}:[^\\s\\x00-\\x1f<>]*|[a-zA-Z0-9.!#$%&'*+/"
        "=?_`{|}~-]+(@)[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)+(?![-_]))>");

    QRegularExpression inline_url = QRegularExpression(
        "^((?:ftp|https?):\\/\\/|www\\.)(?:[a-zA-Z0-9\\-]+\\.?)+[^\\s<]*|^[a-zA-Z0-9.!#$%&'*+/"
        "=?_`{|}~-]+(@)[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)+(?![-_])");

    QRegularExpression inline_tag = QRegularExpression(
        "^<!--(?!-?>)[\\s\\S]*?-->|^<\\/"
        "[a-zA-Z][\\w:-]*\\s*>|^<[a-zA-Z][\\w-]*(?:\\s+[a-zA-Z:_][\\w.:-]*(?:\\s*=\\s*\"[^\"]*\"|\\s*=\\s*'[^']*'|\\s*=\\s*[^\\s\"'=<>`]+)?)*?\\s*\\/"
        ">|^<\\?[\\s\\S]*?\\?>|^<!\\[CDATA\\[[\\s\\S]*?\\]\\]>");

    QRegularExpression inline_link = QRegularExpression(
        "^!?\\[((?:\\[[^\\[\\]]*\\]|\\\\[\\[\\]]?|`[^`]*`|[^\\[\\]\\\\])*?)\\]\\(\\s*(<(?:\\\\[<>]?|[^\\s<>\\\\])*>|(?:\\\\[()]?|\\([^\\s\\x00-\\x1f()\\\\]*\\)"
        "|[^\\s\\x00-\\x1f()\\\\])*?)(?:\\s+(\"(?:\\\\\"?|[^\"\\\\])*\"|'(?:\\\\'?|[^'\\\\])*'|\\((?:\\\\\\)?|[^)\\\\])*\\)))?\\s*\\)");

    QRegularExpression inline_reflink =
        QRegularExpression("^!?\\[((?:\\[[^\\[\\]]*\\]|\\\\[\\[\\]]?|`[^`]*`|[^\\[\\]\\\\])*?)\\]\\[(?!\\s*\\])((?:\\[\\[\\]]?|[^\\[\\]\\\\])+)\\]");

    QRegularExpression inline_nolink = QRegularExpression("^!?\\[(?!\\s*\\])((?:\\[[^\\[\\]]*\\]|\\\\[\\[\\]]|[^\\[\\]])*)\\](?:\\[\\])?");

    QRegularExpression inline_strong =
        QRegularExpression("^__([^\\s][\\s\\S]*?[^\\s])__(?!_)|^\\*\\*([^\\s][\\s\\S]*?[^\\s])\\*\\*(?!\\*)|^__([^\\s])__(?!_)|^\\*\\*([^\\s])\\*\\*(?!\\*)");

    QRegularExpression inline_em = QRegularExpression(
        "^_([^\\s][\\s\\S]*?[^\\s_])_(?!_)|^_([^\\s_][\\s\\S]*?[^\\s])_(?!_)|^\\*([^\\s][\\s\\S]*?[^\\s*])\\*(?!\\*)|^\\*([^\\s*][\\s\\S]*?[^\\s])\\*(?!\\*)|^_"
        "([^\\s_])_(?!_)|^\\*([^\\s*])\\*(?!\\*)");

    QRegularExpression inline_code = QRegularExpression("^(\\`{1,})\\s*([\\s\\S]*?[^`]?)\\s*\\1(?!`)");

    QRegularExpression inline_br = QRegularExpression("^ {2,}\n(?!\\s*$)");

    QRegularExpression inline_del = QRegularExpression("^~~(?=\\S)([\\s\\S]*?\\S)~~");

    QRegularExpression inline_text =
        QRegularExpression("^[\\s\\S]+?(?=[\\\\<!\\[`*~]|https?:\\/\\/|ftp:\\/\\/|www\\.|[a-zA-Z0-9.!#$%&'*+/=?_`{\\|}~-]+@|\b_| {2,}\n|$)");

    QRegularExpression inline_backPedal = QRegularExpression("(?:[^?!.,:;*_~()&]+|\\([^)]*\\)|&(?![a-zA-Z0-9]+;$)|[?!.,:;*_~)]+(?!$))+");

    bool m_inLink = false;
    Parser *m_parser;
};
