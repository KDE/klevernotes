/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>
*/

#include "renderer.h"

#include "kleverconfig.h"
#include "md4qt/traits.hpp"

#include <QDir>
#include <QRegularExpression>
#include <QUrl>

Renderer::Renderer()
    : MD::details::HtmlVisitor<MD::QStringTrait>(){};

void Renderer::openStyle(const typename MD::ItemWithOpts<MD::QStringTrait>::Styles &styles)
{
    int tmp = 0;

    for (const auto &s : styles) {
        const long long int opts = s.style();
        switch (opts) {
        case MD::TextOption::BoldText: {
            if (!(tmp & MD::TextOption::BoldText)) {
                html.push_back(QStringLiteral("<strong>"));
                tmp |= MD::TextOption::BoldText;
            }
        } break;

        case MD::TextOption::ItalicText: {
            if (!(tmp & MD::TextOption::ItalicText)) {
                html.push_back(QStringLiteral("<em>"));
                tmp |= MD::TextOption::ItalicText;
            }
        } break;

        case MD::TextOption::StrikethroughText: {
            if (!(tmp & MD::TextOption::StrikethroughText)) {
                html.push_back(QStringLiteral("<del>"));
                tmp |= MD::TextOption::StrikethroughText;
            }
        } break;

        default:
            if (m_extendedSyntaxMap.contains(opts)) {
                if (!(tmp & opts)) {
                    html.push_back(m_extendedSyntaxMap[opts].first);
                    tmp |= opts;
                }
            }
            break;
        }
    }
}

void Renderer::closeStyle(const typename MD::ItemWithOpts<MD::QStringTrait>::Styles &styles)
{
    int tmp = 0;

    for (const auto &s : styles) {
        const long long int opts = s.style();
        switch (opts) {
        case MD::TextOption::BoldText: {
            if (!(tmp & MD::TextOption::BoldText)) {
                html.push_back(QStringLiteral("</strong>"));
                tmp |= MD::TextOption::BoldText;
            }
        } break;

        case MD::TextOption::ItalicText: {
            if (!(tmp & MD::TextOption::ItalicText)) {
                html.push_back(QStringLiteral("</em>"));
                tmp |= MD::TextOption::ItalicText;
            }
        } break;

        case MD::TextOption::StrikethroughText: {
            if (!(tmp & MD::TextOption::StrikethroughText)) {
                html.push_back(QStringLiteral("</del>"));
                tmp |= MD::TextOption::StrikethroughText;
            }
        } break;

        default:
            if (m_extendedSyntaxMap.contains(opts)) {
                if (!(tmp & opts)) {
                    html.push_back(m_extendedSyntaxMap[opts].second);
                    tmp |= opts;
                }
            }
            break;
        }
    }
}

// Overriding default
// =========
void Renderer::onHeading(
    //! Heading.
    MD::Heading<MD::QStringTrait> *h,
    //! Heading tag.
    const typename MD::QStringTrait::String &ht)
{
    if (!justCollectFootnoteRefs) {
        html.push_back(QStringLiteral("<"));
        html.push_back(ht);
        html.push_back(MD::details::headingIdToHtml(h));
        html.push_back(QStringLiteral(">"));
    }

    if (h->text().get())
        onParagraph(h->text().get(), false);

    if (!justCollectFootnoteRefs) {
        html.push_back(QStringLiteral("</"));
        html.push_back(ht);
        html.push_back(QStringLiteral(">"));
    }
}

void Renderer::onLink(MD::Link<MD::QStringTrait> *l)
{
    QString url = l->url();

    if (m_pluginHelper) {
        const QString wikilinkDelim = QStringLiteral("@HEADER@");
        if (url.contains(wikilinkDelim)) {
            auto linkedNoteInfo = url.split(wikilinkDelim);
            linkedNoteInfo.append(l->text());
            m_pluginHelper->mapperParserUtils()->addToLinkedNoteInfos(linkedNoteInfo);
        }
    }

    const auto lit = this->doc->labeledLinks().find(url);

    if (lit != this->doc->labeledLinks().cend())
        url = lit->second->url();

    if (std::find(this->anchors.cbegin(), this->anchors.cend(), url) != this->anchors.cend())
        url = QStringLiteral("#") + url;
    else if (url.startsWith(QStringLiteral("#")) && this->doc->labeledHeadings().find(url) == this->doc->labeledHeadings().cend()) {
        auto path = static_cast<MD::Anchor<MD::QStringTrait> *>(this->doc->items().at(0).get())->label();
        const auto sp = path.lastIndexOf(QStringLiteral("/"));
        path.remove(sp, path.length() - sp);
        const auto p = url.indexOf(path) - 1;
        url.remove(p, url.length() - p);
    }

    if (!justCollectFootnoteRefs) {
        openStyle(l->openStyles());

        html.push_back(QStringLiteral("<a href=\""));
        html.push_back(url);
        html.push_back(QStringLiteral("\">"));
    }

    if (!l->img()->isEmpty()) {
        if (!justCollectFootnoteRefs) {
            onImage(l->img().get());
        }
    } else if (l->p() && !l->p()->isEmpty()) {
        onParagraph(l->p().get(), false);
    } else if (!l->text().isEmpty()) {
        if (!justCollectFootnoteRefs) {
            html.push_back(MD::details::prepareTextForHtml<MD::QStringTrait>(l->text()));
        }
    } else {
        if (!justCollectFootnoteRefs) {
            html.push_back(MD::details::prepareTextForHtml<MD::QStringTrait>(l->url()));
        }
    }

    if (!justCollectFootnoteRefs) {
        html.push_back(QStringLiteral("</a>"));

        closeStyle(l->closeStyles());
    }
}

void Renderer::onImage(MD::Image<MD::QStringTrait> *i)
{
    if (!justCollectFootnoteRefs) {
        QString url = i->url();
        if (url.startsWith(QStringLiteral("./"))) {
            url = m_notePath + url.mid(1);
        }
        if (url.startsWith(QStringLiteral("~"))) {
            url = QDir::homePath() + url.mid(1);
        }
        if (!(url.startsWith(QStringLiteral("http")) || url.startsWith(QStringLiteral("//")) || url.startsWith(QStringLiteral("qrc:")))) {
            url = QStringLiteral("file:") + url;
        }

        openStyle(i->openStyles());

        html.push_back(image(url, i->text()));

        closeStyle(i->closeStyles());
    }
}

void Renderer::onListItem(MD::ListItem<MD::QStringTrait> *i, bool first)
{
    const bool hasTask = i->isTaskList();
    const bool isChecked = i->isChecked();
    const int startNum = i->listType() == MD::ListItem<MD::QStringTrait>::Ordered && first ? i->startNumber() : -1;

    if (!justCollectFootnoteRefs) {
        html.push_back(openListItem(hasTask, isChecked, startNum));
    }

    // Add the text
    Visitor<MD::QStringTrait>::onListItem(i, first);

    if (!justCollectFootnoteRefs) {
        html.push_back(closeListItem(hasTask));
    }
}

void Renderer::onCode(MD::Code<MD::QStringTrait> *c)
{
    if (!justCollectFootnoteRefs) {
        static const QString pumlStr = QStringLiteral("puml");
        static const QString plantUMLStr = QStringLiteral("plantuml");

        const QString lang = c->syntax();
        const QString _text = c->text();
        QString code = _text;
        bool highlight = false;

        QString returnValue;
        if (m_pluginHelper && KleverConfig::pumlEnabled() && (lang.toLower() == pumlStr || lang.toLower() == plantUMLStr)) {
            QPair<QString, QString> imageInfo = m_pluginHelper->pumlParserUtils()->renderCode(_text, KleverConfig::pumlDark());

            returnValue = image(imageInfo.first, imageInfo.second);
        } else {
            if (m_pluginHelper) {
                highlight = KleverConfig::codeSynthaxHighlightEnabled();
                code = m_pluginHelper->highlightParserUtils()->getCode(highlight, _text, lang);
            }
            returnValue = Renderer::code(code, highlight);
        }

        html.push_back(returnValue);
    }
}
// !Overriding default

// Internal info
// =============
void Renderer::setNotePath(const QString &notePath)
{
    m_notePath = notePath;
}

void Renderer::addExtendedSyntax(const long long int opts, const QString &openingHTML, const QString &closingHTML)
{
    m_extendedSyntaxMap[opts] = {openingHTML, closingHTML};
}

void Renderer::addPluginHelper(PluginHelper *pluginHelper)
{
    m_pluginHelper = pluginHelper;
}

// !Internal info

// Rendering
// =========
QString Renderer::code(QString &code, const bool highlight)
{
    return QStringLiteral("<pre><code>") + (highlight ? code : escape(code, true)) + QStringLiteral("</code></pre>\n");
}

QString Renderer::openListItem(const bool hasTask, const bool isChecked, const int startNumber)
{
    if (!hasTask) {
        const QString startNum = 0 <= startNumber ? (QStringLiteral(" value=\"") + QString::number(startNumber) + QStringLiteral("\"")) : QStringLiteral();
        return QStringLiteral("<li") + startNum + QStringLiteral(">");
    }

    const QString checkboxStr = checkbox(isChecked);
    return QStringLiteral("<li class=\"hasCheck\"> <label class=\"form-control\">\n") + checkboxStr;
}

QString Renderer::closeListItem(const bool hasTask)
{
    return hasTask ? QStringLiteral("</label></li>\n") : QLatin1String();
}

QString Renderer::checkbox(bool checked)
{
    const QString checkedString = checked ? QStringLiteral("checked=\"\" ") : QLatin1String();

    return QStringLiteral("<input ") + checkedString + QStringLiteral("disabled=\"\" type=\"checkbox\">");
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

QString Renderer::image(const QString &href, const QString &text)
{
    return QStringLiteral("<img src=\"") + href + QStringLiteral("\" alt=\"") + text + QStringLiteral("\">");
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
// !Rendering
