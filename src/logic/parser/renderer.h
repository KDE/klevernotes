#pragma once

#include <QString>
#include <QVariantMap>

class Renderer
{
public:
    static QString code(QString &code, const QString &lang, const bool highlight);
    static QString blockquote(const QString &quote);
    static QString html(const QString &html);
    static QString heading(const QString &text, int lvl, QString &raw);
    static QString hr();
    static QString list(const QString &body, bool ordered, const QString &start);
    static QString listItem(const QString &text, const bool hasCheck = false);
    static QString checkbox(bool checked);
    static QString paragraph(const QString &text);
    static QString table(const QString &header, QString &body);
    static QString tableRow(const QString &content);
    static QString tableCell(const QString &content, const QVariantMap flags);
    static QString strong(const QString &text);
    static QString em(const QString &text);
    static QString codeSpan(const QString &text);
    static QString br();
    static QString del(const QString &text);
    static QString link(QString &href, const QString &title, const QString &text);
    static QString image(const QString &href, const QString &title, const QString &text);
    static QString text(const QString &text);
    static QString escape(QString &html, bool encode);
    static QString unescape(const QString &html);
};
