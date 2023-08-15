#include "inlineLexer.h"

#include <QDir>
#include <QMap>
#include <QRandomGenerator>
#include <QString>

#include "parser.h"
#include "renderer.h"

InlineLexer::InlineLexer(Parser *parser)
    : m_parser(parser)
{
}

QString InlineLexer::output(QString &src, bool useInlineText)
{
    QString out = "", text, href, title, cap0, cap1, cap2, cap3, cap4, outputed;
    QMap<QString, QString> linkInfo;

    while (!src.isEmpty()) {
        QRegularExpressionMatch cap, secondCap;

        cap = inline_escape.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), "");

            out += cap.captured(1);
            continue;
        }

        cap = inline_autolink.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), "");

            if (cap.captured(2) == "@") {
                cap1 = cap.captured(1);
                QString mangled = mangle(cap1);
                text = Renderer::escape(mangled, false);
                href = "mailto:" + text;
            } else {
                cap1 = cap.captured(1);
                text = Renderer::escape(cap1, false);
                href = text;
            }
            title = "";

            out += Renderer::link(href, title, text);
            continue;
        }

        // url (gfm)
        cap = inline_url.match(src);
        if (!m_inLink && cap.hasMatch()) {
            cap0 = inline_backPedal.match(cap.captured(0)).captured(0);
            src.replace(src.indexOf(cap0), cap.capturedLength(), "");

            if (cap.captured(2) == "@") {
                text = Renderer::escape(cap0, false);
                href = "mailto:" + text;
            } else {
                text = Renderer::escape(cap0, false);
                if (cap.captured(1) == "www.") {
                    href = "http://" + text;
                } else {
                    href = text;
                }
            }
            title = "";

            out += Renderer::link(href, title, text);
            continue;
        }

        // tag
        cap = inline_tag.match(src);
        if (cap.hasMatch()) {
            bool hasOpeningLink = QRegularExpression("^<a ", QRegularExpression::CaseInsensitiveOption).match(cap.captured(0)).hasMatch();
            bool hasClosingLink = QRegularExpression("^<\\/a>", QRegularExpression::CaseInsensitiveOption).match(cap.captured(0)).hasMatch();

            if (!m_inLink && hasOpeningLink) {
                m_inLink = true;
            } else if (m_inLink && hasClosingLink) {
                m_inLink = false;
            }
            src.replace(cap.capturedStart(), cap.capturedLength(), "");

            out += cap.captured(0);
            continue;
        }

        // link
        cap = inline_link.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), "");

            m_inLink = true;
            href = cap.captured(2);

            int end = cap.captured(3).length() - 2;
            title = !cap.captured(3).isEmpty() ? cap.captured(3).mid(1, end) : "";

            href = href.trimmed();
            QRegularExpressionMatch tagMatch = QRegularExpression("^<([\\s\\S]*)>$").match(href);
            href = href.replace(tagMatch.capturedStart(), tagMatch.capturedLength(), tagMatch.captured(1));
            linkInfo = {{"href", escapes(href)}, {"title", escapes(title)}};

            out += outputLink(cap, linkInfo, useInlineText);
            m_inLink = false;
            continue;
        }

        // reflink, nolink
        QRegularExpressionMatch tempMatch = inline_reflink.match(src);
        if (tempMatch.hasMatch())
            cap = tempMatch;
        else
            cap = inline_nolink.match(src);

        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), "");

            cap2 = cap.captured(2);
            cap1 = cap.captured(1).replace(QRegularExpression("\\s+"), " ");

            QString linkId = !cap2.isEmpty() ? cap2 : cap1;

            linkInfo = m_parser->links[linkId.toLower()];
            if (linkInfo.isEmpty() || linkInfo["href"].isEmpty()) {
                out += cap.captured(0).at(0);
                src = cap.captured(0).mid(1) + src;
                continue;
            }

            m_inLink = true;
            out += outputLink(cap, linkInfo, useInlineText);
            m_inLink = false;
            continue;
        }

        // strong
        cap = inline_strong.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), "");

            cap4 = cap.captured(4);
            cap3 = cap.captured(3);
            cap2 = cap.captured(2);
            cap1 = cap.captured(1);

            QString toOutput;
            if (!cap4.isEmpty())
                toOutput = cap4;
            else if (!cap3.isEmpty())
                toOutput = cap3;
            else if (!cap2.isEmpty())
                toOutput = cap2;
            else
                toOutput = cap1;

            outputed = output(toOutput);

            out += Renderer::strong(outputed);
            continue;
        }

        // em
        cap = inline_em.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), "");

            QString cap6 = cap.captured(6);
            QString cap5 = cap.captured(5);
            cap4 = cap.captured(4);
            cap3 = cap.captured(3);
            cap2 = cap.captured(2);
            cap1 = cap.captured(1);

            QString toOutput;
            if (!cap6.isEmpty())
                toOutput = cap6;
            else if (!cap5.isEmpty())
                toOutput = cap5;
            else if (!cap4.isEmpty())
                toOutput = cap4;
            else if (!cap3.isEmpty())
                toOutput = cap3;
            else if (!cap2.isEmpty())
                toOutput = cap2;
            else
                toOutput = cap1;

            outputed = output(toOutput);

            out += Renderer::em(outputed);
            continue;
        }

        // code
        cap = inline_code.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), "");

            QString capTrimmed = cap.captured(2).trimmed();
            QString escaped = Renderer::escape(capTrimmed, true);

            out += Renderer::codeSpan(escaped);
            continue;
        }

        // br
        cap = inline_br.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), "");

            if (!useInlineText)
                out += Renderer::br();
            continue;
        }

        // del (gfm)
        cap = inline_del.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), "");

            cap1 = cap.captured(1);
            outputed = output(cap1);

            out += Renderer::del(outputed);
            continue;
        }

        // text
        cap = inline_text.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), "");

            cap0 = cap.captured(0);
            QString escaped = Renderer::escape(cap0, false);

            out += (useInlineText) ? escaped : Renderer::text(escaped);
            continue;
        }

        if (!src.isEmpty()) {
            qFatal("Infinite loop on byte: %d", src[0].unicode());
        }
    }

    return out;
};

QString InlineLexer::mangle(QString &text)
{
    QString out = "";
    int l = text.length();

    for (int i = 0; i < l; i++) {
        QChar ch = text.at(i);
        if (QRandomGenerator::global()->generate() % 2 == 0) {
            out += "x" + QString::number(ch.unicode(), 16);
        } else {
            out += "&#" + QString::number(ch.unicode()) + ";";
        }
    }

    return out;
}

QString InlineLexer::outputLink(QRegularExpressionMatch &cap, QMap<QString, QString> linkInfo, bool useInlineText)
{
    QString href = linkInfo["href"];
    QString title = linkInfo["title"];
    title = !title.isEmpty() ? Renderer::escape(title, false) : "";

    // KLEVERNOTES ADDED THOSE LINES
    // ======
    if (href.startsWith("./"))
        href = m_parser->getNotePath() + href.mid(1);
    if (href.startsWith("~"))
        href = QDir::homePath() + href.mid(1);
    if (!(href.startsWith("http") || href.startsWith("//")))
        href = "file:" + href;
    // ======
    QString out;
    QString cap1 = cap.captured(1);
    if (cap.captured(0).at(0) != "!") {
        QString outputed = output(cap1);
        out = Renderer::link(href, title, outputed);
    } else {
        QString escaped = Renderer::escape(cap1, false);
        out = (useInlineText) ? escaped : Renderer::image(href, title, escaped);
    }

    return out;
}

QString InlineLexer::escapes(QString &text)
{
    QRegularExpression escapesReg("\\\\([!\"#$%&'()*+,\\-.\\/:;<=>?@\\[\\]\\^_`{|}~])");

    return !text.isEmpty() ? text.replace(escapesReg, "\\1") : text;
}
