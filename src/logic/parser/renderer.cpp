/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#include "renderer.h"
#include "logic/parser/plugins/syntaxHighlight/highlightHelper.h"

#include <QRegularExpression>
#include <QUrl>

QString Renderer::code(QString &code, const QString &lang, const bool highlight)
{
    if (highlight) {
        code = HighlightHelper::getHighlightedString(code, lang);
    }

    return QStringLiteral("<pre><code>") + (highlight ? code : escape(code, true)) + QStringLiteral("</code></pre>\n");
}

QString Renderer::blockquote(const QString &quote)
{
    return QStringLiteral("<blockquote>\n") + quote + QStringLiteral("</blockquote>\n");
}

QString Renderer::html(const QString &html)
{
    return html;
}

QString Renderer::heading(const QString &text, const QString &lvl, const QString &raw, const bool scrollTo)
{
    static const QRegularExpression rawReg = QRegularExpression(QStringLiteral("[^\\w]+"));
    const QString id = scrollTo ? QStringLiteral("noteMapperScrollTo") : raw.toLower().replace(rawReg, QStringLiteral("-"));
    return QStringLiteral("<h") + lvl + QStringLiteral(" id=\"") + id + QStringLiteral("\">") + text + QStringLiteral("</h") + lvl + QStringLiteral(">\n");
}

QString Renderer::hr()
{
    return QStringLiteral("<hr>\n");
}

QString Renderer::list(const QString &body, bool ordered, const QString &start)
{
    const QString type = ordered ? QStringLiteral("ol") : QStringLiteral("ul");
    QString startat;
    if (ordered && start != QString::number(1)) {
        startat = (QStringLiteral(" start=\"") + start + QStringLiteral("\""));
    } else {
        startat = QLatin1String();
    }

    return QStringLiteral("<") + type + startat + QStringLiteral(">\n") + body + QStringLiteral("</") + type + QStringLiteral(">\n");
}

QString Renderer::listItem(const QString &text, const bool hasCheck)
{
    const QString out = hasCheck ? QStringLiteral("<li class=\"hasCheck\"> <label class=\"form-control\">\n") + text + QStringLiteral("</label></li>\n")
                                 : QStringLiteral("<li>") + text + QStringLiteral("</li>\n");
    return out;
}

QString Renderer::checkbox(bool checked)
{
    const QString checkedString = checked ? QStringLiteral("checked=\"\" ") : QLatin1String();

    return QStringLiteral("<input ") + checkedString + QStringLiteral("disabled=\"\" type=\"checkbox\">");
}

QString Renderer::paragraph(const QString &text)
{
    return QStringLiteral("<p>") + text + QStringLiteral("</p>\n");
}

QString Renderer::table(const QString &header, QString &body)
{
    if (!body.isEmpty())
        body = QStringLiteral("<tbody>") + body + QStringLiteral("</tbody>");

    return QStringLiteral("<table>\n") + QStringLiteral("<thead>\n") + header + QStringLiteral("</thead>\n") + body + QStringLiteral("</table>\n");
}

QString Renderer::tableRow(const QString &content)
{
    return QStringLiteral("<tr>\n") + content + QStringLiteral("</tr>\n");
}

QString Renderer::tableCell(const QString &content, const QVariantMap &flags)
{
    const QString type = flags[QStringLiteral("header")].toBool() ? QStringLiteral("th") : QStringLiteral("td");
    const QString align = flags[QStringLiteral("align")].toString();
    QString tag;
    if (align.isEmpty()) {
        tag = QStringLiteral("<") + type + QStringLiteral(">");
    } else {
        tag = QStringLiteral("<") + type + QStringLiteral(" style=\"text-align:") + align + QStringLiteral(";\">");
    }
    return tag + content + QStringLiteral("</") + type + QStringLiteral(">\n");
}

QString Renderer::strong(const QString &text)
{
    return QStringLiteral("<strong>") + text + QStringLiteral("</strong>");
}

QString Renderer::em(const QString &text)
{
    return QStringLiteral("<em>") + text + QStringLiteral("</em>");
}

QString Renderer::codeSpan(const QString &text)
{
    return QStringLiteral("<code>") + text + QStringLiteral("</code>");
}

QString Renderer::br()
{
    return QStringLiteral("<br>");
}

QString Renderer::del(const QString &text)
{
    return QStringLiteral("<del>") + text + QStringLiteral("</del>");
}

QString Renderer::superscript(const QString &text)
{
    return QStringLiteral("<sup>") + text + QStringLiteral("</sup>");
}

QString Renderer::subscript(const QString &text)
{
    return QStringLiteral("<sub>") + text + QStringLiteral("</sub>");
}

QString Renderer::mark(const QString &text)
{
    return QStringLiteral("<mark>") + text + QStringLiteral("</mark>");
}

QString Renderer::wikilink(const QString &href, const QString &title, const QString &text)
{
    const QString leading = QStringLiteral("<a href=\"") + href + QStringLiteral("\"");
    const QString ending = QStringLiteral(">") + text + QStringLiteral("</a>");
    QString middle = QLatin1String();
    if (!title.isEmpty()) {
        QStringLiteral(" title=\"") + title + QStringLiteral("\"");
    }
    return leading + middle + ending;
}

QString Renderer::link(QString &href, const QString &title, const QString &text)
{
    const QByteArray uri = QUrl::fromUserInput(href).toEncoded();
    if (uri.isEmpty())
        return text;

    static const QRegularExpression uriPercentReg = QRegularExpression(QStringLiteral("%25"));
    href = QString::fromUtf8(uri).replace(uriPercentReg, QStringLiteral("%"));

    const QString leading = QStringLiteral("<a href=\"") + escape(href, false) + QStringLiteral("\"");
    const QString ending = QStringLiteral(">") + text + QStringLiteral("</a>");
    QString middle = QLatin1String();
    if (!title.isEmpty()) {
        QStringLiteral(" title=\"") + title + QStringLiteral("\"");
    }

    return leading + middle + ending;
}

QString Renderer::image(const QString &href, const QString &title, const QString &text)
{
    const QString leading = QStringLiteral("<img src=\"") + href + QStringLiteral("\" alt=\"") + text + QStringLiteral("\"");
    QString middle = QLatin1String();
    if (!title.isEmpty()) {
        QStringLiteral(" title=\"") + title + QStringLiteral("\"");
    }
    static const QString ending = QStringLiteral(">");

    return leading + middle + ending;
}

QString Renderer::text(const QString &text)
{
    return text;
}

QString Renderer::escape(QString &html, bool encode)
{
    static const QRegularExpression replace1 = QRegularExpression(QStringLiteral("&(?!#?\\w+;)"));
    static const QRegularExpression replace2 = QRegularExpression(QStringLiteral("&"));

    const QRegularExpression encodedReplacement = !encode ? replace1 : replace2;

    static const QRegularExpression leftBracketReg = QRegularExpression(QStringLiteral("<"));
    static const QRegularExpression rightBracketReg = QRegularExpression(QStringLiteral(">"));
    static const QRegularExpression quoteReg = QRegularExpression(QStringLiteral("\""));
    static const QRegularExpression apostropheReg = QRegularExpression(QStringLiteral("'"));
    return html.replace(encodedReplacement, QStringLiteral("&amp;"))
        .replace(leftBracketReg, QStringLiteral("&lt;"))
        .replace(rightBracketReg, QStringLiteral("&gt;"))
        .replace(quoteReg, QStringLiteral("&quot;"))
        .replace(apostropheReg, QStringLiteral("&#39;"));
}

QString Renderer::unescape(const QString &html)
{
    // explicitly match decimal, hex, and named HTML entities
    QString result = html;
    static const QRegularExpression regex(QStringLiteral("&(#(?:\\d+)|(?:#x[0-9A-Fa-f]+)|(?:\\w+));?"));
    QRegularExpressionMatchIterator i = regex.globalMatch(result);

    QRegularExpressionMatch match;
    while (i.hasNext()) {
        match = i.next();
        QString entity = match.captured(1).toLower();

        if (entity == QStringLiteral("colon")) {
            result.replace(match.capturedStart(), match.capturedLength(), QStringLiteral(":"));
            continue;
        }
        if (entity.startsWith(QStringLiteral("#"))) {
            bool ok;
            // check for hexadecimal or numerical value
            const int charCode = (entity.at(1) == QChar::fromLatin1('x')) ? entity.remove(0, 2).toInt(&ok, 16) : entity.remove(0, 1).toInt(&ok);

            result.replace(match.capturedStart(), match.capturedLength(), QChar(charCode));
            continue;
        }
        result.replace(match.capturedStart(), match.capturedLength(), QLatin1String());
    }

    return result;
}
