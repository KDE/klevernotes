/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#include "parser.h"

#include <QJsonArray>

using namespace std;

void KleverNotesHtmlVisitor::onImage(MD::Image<MD::QStringTrait> *i)
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
        html.push_back(QStringLiteral("<img src=\""));
        html.push_back(url);
        html.push_back(QStringLiteral("\" alt=\""));
        html.push_back(MD::details::prepareTextForHtml<MD::QStringTrait>(i->text()));
        html.push_back(QStringLiteral("\" style=\"max-width:100%;\" />"));
    }
}

void KleverNotesHtmlVisitor::onListItem(MD::ListItem<MD::QStringTrait> *i, bool first)
{
    if (!justCollectFootnoteRefs) {
        html.push_back(QStringLiteral("<li"));

        if (i->isTaskList()) {
            html.push_back(
                QStringLiteral(" class=\"task-list-item\"><label class=\"form-control\">"
                               "<input type=\"checkbox\" id=\"\" disabled=\"\" class=\"task-list-item-checkbox\""));

            if (i->isChecked()) {
                html.push_back(QStringLiteral(" checked=\"\""));
            }
        }

        if (i->listType() == MD::ListItem<MD::QStringTrait>::Ordered && first) {
            html.push_back(QStringLiteral(" value=\""));
            html.push_back(QString::number(i->startNumber()));
            html.push_back(QStringLiteral("\""));
        }

        html.push_back(QStringLiteral(">\n"));
    }

    Visitor<MD::QStringTrait>::onListItem(i, first);
    if (i->isTaskList()) {
        html.push_back(QStringLiteral("</label>"));
    }
    if (!justCollectFootnoteRefs)
        html.push_back(QStringLiteral("</li>\n"));
}

Parser::Parser(QObject *parent)
    : QObject(parent)
{
    m_renderer = new KleverNotesHtmlVisitor({});
}

PluginHelper *Parser::getPluginHelper() const
{
    return pluginHelper;
}

void Parser::setHeaderInfo(const QStringList &headerInfo)
{
    pluginHelper->getMapperParserUtils()->setHeaderInfo(headerInfo);
}

QString Parser::headerLevel() const
{
    return pluginHelper->getMapperParserUtils()->headerLevel();
};

void Parser::setNotePath(const QString &notePath)
{
    if (notePath.isEmpty() || m_notePath == notePath) {
        return;
    }

    m_notePath = notePath == QStringLiteral("qrc:") ? notePath : notePath.chopped(1);
    m_renderer->setNotePath(m_notePath);
    if (notePath == QStringLiteral("qrc:")) {
        return;
    }
    // We do this here because we're sure to be in another note
    pluginHelper->clearPluginsInfo();

    pluginHelper->getMapperParserUtils()->setPathsInfo(notePath);
}

QString Parser::getNotePath() const
{
    return m_notePath;
}

QString Parser::parse(QString src)
{
    // pluginHelper->clearPluginsInfo();
    //
    // blockLexer.lex(src);
    //
    // pluginHelper->preTokChanges();
    //
    // std::reverse(tokens.begin(), tokens.end());
    //
    // QString out;
    // while (getNextToken()) {
    //     out += tok();
    // }
    //
    const QString path = m_notePath == QStringLiteral("qrc:") ? m_notePath : m_notePath.chopped(1);

    QTextStream s(&src, QIODeviceBase::ReadOnly);

    const auto doc = m_md4qtParser.parse(s, path, QStringLiteral("local.md"));
    const auto html = m_renderer->toHtml(doc, path);

    pluginHelper->postTokChanges();
    return html;
}

// Syntax highlight
void Parser::newHighlightStyle()
{
    pluginHelper->getHighlightParserUtils()->newHighlightStyle();
}

// PUML
void Parser::pumlDarkChanged()
{
    pluginHelper->getPUMLParserUtils()->pumlDarkChanged();
}
