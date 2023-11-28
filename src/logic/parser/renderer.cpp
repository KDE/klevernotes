#include "renderer.h"
#include "logic/syntaxHighlight/highlightHelper.h"

#include <QRegularExpression>
#include <QUrl>

QString Renderer::code(QString &code, const QString &lang, const bool highlight)
{
    if (highlight) {
        code = HighlightHelper::getHighlightedString(code, lang);
    }

    return "<pre><code>" + (highlight ? code : escape(code, true)) + QString::fromStdString("</code></pre>\n");
}

QString Renderer::blockquote(const QString &quote)
{
    return QString::fromStdString("<blockquote>\n") + quote + QString::fromStdString("</blockquote>\n");
}

QString Renderer::html(const QString &html)
{
    return html;
}

QString Renderer::heading(const QString &text, int lvl, QString &raw)
{
    return "<h" + QString::number(lvl) + QString::fromStdString(" id=\"") + raw.toLower().replace(QRegularExpression("[^\\w]+"), "-")
        + QString::fromStdString("\">") + text + "</h" + QString::number(lvl) + QString::fromStdString(">\n");
}

QString Renderer::hr()
{
    return "<hr>\n";
}

QString Renderer::list(const QString &body, bool ordered, const QString &start)
{
    QString type = ordered ? "ol" : "ul";
    QString startatt = (ordered && start != QString::number(1)) ? (QString::fromStdString(" start=\"") + start + QString::fromStdString("\"")) : "";

    return "<" + type + startatt + QString::fromStdString(">\n") + body + "</" + type + QString::fromStdString(">\n");
}

QString Renderer::listItem(const QString &text, const bool hasCheck)
{
    QString out;
    if (hasCheck) {
        out = QString::fromStdString("<li class=\"hasCheck\"> <label class=\"form-control\">\n") + text + QString::fromStdString("</label></li>\n");
    } else {
        out = QString::fromStdString("<li>") + text + QString::fromStdString("</li>\n");
    }
    return out;
}

QString Renderer::checkbox(bool checked)
{
    QString checkedString = checked ? QString::fromStdString("checked=\"\" ") : "";

    return "<input " + checkedString + QString::fromStdString("disabled=\"\" type=\"checkbox\"") + ">";
}

QString Renderer::paragraph(const QString &text)
{
    return "<p>" + text + QString::fromStdString("</p>\n");
}

QString Renderer::table(const QString &header, QString &body)
{
    if (!body.isEmpty())
        body = "<tbody>" + body + "</tbody>";

    return QString::fromStdString("<table>\n") + QString::fromStdString("<thead>\n") + header + QString::fromStdString("</thead>\n") + body
        + QString::fromStdString("</table>\n");
}

QString Renderer::tableRow(const QString &content)
{
    return QString::fromStdString("<tr>\n") + content + QString::fromStdString("</tr>\n");
}

QString Renderer::tableCell(const QString &content, const QVariantMap flags)
{
    QString type = flags["header"].toBool() ? "th" : "td";
    QString align = flags["align"].toString();
    QString tag;
    if (!align.isEmpty()) {
        tag = QString::fromStdString("<") + type + QString::fromStdString(" style=\"text-align:") + align + QString::fromStdString(";\">");
    } else {
        tag = QString::fromStdString("<") + type + QString::fromStdString(">");
    }
    return tag + content + "</" + type + QString::fromStdString(">\n");
}

QString Renderer::strong(const QString &text)
{
    return "<strong>" + text + "</strong>";
}

QString Renderer::em(const QString &text)
{
    return "<em>" + text + "</em>";
}

QString Renderer::codeSpan(const QString &text)
{
    return "<code>" + text + "</code>";
}

QString Renderer::br()
{
    return "<br>";
}

QString Renderer::del(const QString &text)
{
    return "<del>" + text + "</del>";
}

QString Renderer::link(QString &href, const QString &title, const QString &text)
{
    QByteArray uri = QUrl::fromUserInput(href).toEncoded();
    if (uri.isEmpty())
        return text;

    href = QString(uri).replace(QRegularExpression("%25"), "%");

    QString out = QString::fromStdString("<a href=\"") + escape(href, false) + QString::fromStdString("\"");

    if (!title.isEmpty()) {
        out += QString::fromStdString(" title=\"") + title + QString::fromStdString("\"");
    }
    out += ">" + text + "</a>";
    return out;
}

QString Renderer::image(const QString &href, const QString &title, const QString &text)
{
    QString out = QString::fromStdString("<img src=\"") + href + QString::fromStdString("\" alt=\"") + text + QString::fromStdString("\"");
    if (!title.isEmpty()) {
        out += QString::fromStdString(" title=\"") + title + QString::fromStdString("\"");
    }
    out += ">";
    return out;
}

QString Renderer::text(const QString &text)
{
    return text;
}

QString Renderer::escape(QString &html, bool encode)
{
    QRegularExpression encodedReplacement = !encode ? QRegularExpression("&(?!#?\\w+;)") : QRegularExpression("&");

    return html.replace(encodedReplacement, "&amp;")
        .replace(QRegularExpression("<"), "&lt;")
        .replace(QRegularExpression(">"), "&gt;")
        .replace(QRegularExpression("\""), "&quot;")
        .replace(QRegularExpression("'"), "&#39;");
}

QString Renderer::unescape(const QString &html)
{
    // explicitly match decimal, hex, and named HTML entities
    QString result = html;
    QRegularExpression regex("&(#(?:\\d+)|(?:#x[0-9A-Fa-f]+)|(?:\\w+));?");
    QRegularExpressionMatchIterator i = regex.globalMatch(result);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString entity = match.captured(1).toLower();

        if (entity == "colon") {
            result.replace(match.capturedStart(), match.capturedLength(), ":");
            continue;
        }
        if (entity.startsWith("#")) {
            bool ok;
            // check for hexadecimal or numerical value
            int code = (entity.at(1) == 'x') ? entity.mid(2).toInt(&ok, 16) : entity.mid(1).toInt(&ok);

            result.replace(match.capturedStart(), match.capturedLength(), QChar(code));
            continue;
        }
        result.replace(match.capturedStart(), match.capturedLength(), "");
    }

    return result;
}
