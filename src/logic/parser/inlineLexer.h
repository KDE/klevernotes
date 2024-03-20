/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#pragma once

#include <QRegularExpression>

class Parser;

class InlineLexer
{
public:
    explicit InlineLexer(Parser *parser);

    QString output(QString &src, bool useInlineText = false);

private:
    QString mangle(const QString &text) const;
    QString escapes(QString &text) const;
    QString outputLink(QRegularExpressionMatch &cap, QMap<QString, QString> linkInfo, bool useInlineText);

    inline static const QRegularExpression inline_escape = QRegularExpression(QStringLiteral("^\\\\([!\"#$%&'()*+,\\-./:;<=>?@\\[\\]\\\\^_`{|}~~|])"));

    inline static const QRegularExpression inline_autolink = QRegularExpression(
        QStringLiteral("^<([a-zA-Z][a-zA-Z0-9+.-]{1,31}:[^\\s\x00-\x1f<>]*|[a-zA-Z0-9.!#$%&'*+/"
                       "=?^_`{|}~-]+(@)[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)+(?![-_]))>"));

    inline static const QRegularExpression inline_url = QRegularExpression(
        QStringLiteral("^((?:ftp|https?):\\/\\/|www\\.)(?:[a-zA-Z0-9\\-]+\\.?)+[^\\s<]*|^[a-zA-Z0-9.!#$%&'*+/"
                       "=?^_`{|}~-]+(@)[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?)+(?![-_])"));

    inline static const QRegularExpression inline_tag =
        QRegularExpression(QStringLiteral("^<!--(?!-?>)[\\s\\S]*?-->|^</"
                                          "[a-zA-Z][\\w:-]*\\s*>|^<[a-zA-Z][\\w-]*(?:\\s+[a-zA-Z:_][\\w.:-]*(?:\\s*=\\s*\"[^\"]*\"|\\s*=\\s*'[^']*'|\\s*=\\s*[^"
                                          "\\s\"'=<>`]+)?)*?\\s*/?>|^<\\?[\\s\\S]*?\\?>|^<![a-zA-Z]+\\s[\\s\\S]*?>|^<!\\[CDATA\\[[\\s\\S]*?\\]\\]>"));

    inline static const QRegularExpression inline_link = QRegularExpression(
        QStringLiteral("^!?\\[((?:\\[[^\\[\\]]*\\]|[\\[\\]]?|`[^`]*`|[^\\[\\]])*?)\\]\\(\\s*(<(?:[<>]?|[^\\s<>])*>|(?:[()]?|\\([^\\s\\x00-\\x1f()]*\\)"
                       "|[^\\s\\x00-\\x1f()])*?)(?:\\s+(\"(?:\"?|[^\"])*\"|'(?:'?|[^'])*'|\\((?:\\)?|[^)])*\\)))?\\s*\\)"));

    inline static const QRegularExpression inline_reflink =
        QRegularExpression(QStringLiteral("^!?\\[((?:\\[[^\\[\\]]*\\]|[\\[\\]]?|`[^`]*`|[^\\[\\]])*?)\\]\\[(?!\\s*\\])((?:\\[\\[\\]]?|[^\\[\\]])+)\\]"));

    inline static const QRegularExpression inline_nolink =
        QRegularExpression(QStringLiteral("^!?\\[(?!\\s*\\])((?:\\[[^\\[\\]]*\\]|[\\[\\]]|[^\\[\\]])*)\\](?:\\[\\])?"));

    inline static const QRegularExpression inline_strong = QRegularExpression(QStringLiteral("^(__|\\*\\*)(.*)(__|\\*\\*)"));

    inline static const QRegularExpression inline_em = QRegularExpression(QStringLiteral("^(_|\\*)(.*)(_|\\*)"));

    inline static const QRegularExpression inline_code = QRegularExpression(QStringLiteral("^(`)(?=\\S)([\\s\\S]*?\\S)(`)"));

    inline static const QRegularExpression inline_br = QRegularExpression(QStringLiteral("^ {2,}\n(?!\\s*$)"));

    inline static const QRegularExpression inline_del = QRegularExpression(QStringLiteral("^(~~)(?=\\S)([\\s\\S]*?\\S)(~~)"));

    inline static const QRegularExpression inline_highlight = QRegularExpression(QStringLiteral("^(==)(?=\\S)([\\s\\S]*?\\S)(==)"));

    inline static const QRegularExpression inline_text =
        QRegularExpression(QStringLiteral("^[\\s\\S]+?(?=[<!\\[`*~_=:\\^]|https?:\\/\\/|ftp:\\/\\/|www\\.|[a-zA-Z0-9.!#$%&'*+/=?_`{\\|}~-]+@|\b_| {2,}\n|$)"));

    inline static const QRegularExpression inline_subscript = QRegularExpression(QStringLiteral("^~(?=\\S)([\\s\\S]*?\\S)~"));

    inline static const QRegularExpression inline_superscript = QRegularExpression(QStringLiteral("^\\^(?=\\S)([\\s\\S]*?\\S)\\^"));

    inline static const QRegularExpression inline_backPedal =
        QRegularExpression(QStringLiteral("(?:[^?!.,:;*_~()&]+|\\([^)]*\\)|&(?![a-zA-Z0-9]+;$)|[?!.,:;*_~)]+(?!$))+"));

    bool m_inLink = false;
    Parser *m_parser;
};
