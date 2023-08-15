#pragma once

#include <QString>
#include <QVariantMap>

class Renderer
{
public:
    static QString code(QString &code, QString &lang);
    static QString blockquote(QString &quote);
    static QString html(QString &html);
    static QString heading(QString &text, int lvl, QString &raw);
    static QString hr();
    static QString list(QString &body, bool ordered, QString &start);
    static QString listItem(QString &text);
    static QString checkbox(bool checked);
    static QString paragraph(QString &text);
    static QString table(QString &header, QString &body);
    static QString tableRow(QString &content);
    static QString tableCell(QString &content, QVariantMap flags);
    static QString strong(QString &text);
    static QString em(QString &text);
    static QString codeSpan(QString &text);
    static QString br();
    static QString del(QString &text);
    static QString link(QString &href, QString &title, QString &text);
    static QString image(QString &href, QString &title, QString &text);
    static QString text(QString &text);
    static QString escape(QString &html, bool encode);
    static QString unescape(QString &html);
};
