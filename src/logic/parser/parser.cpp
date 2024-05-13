/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#include "parser.h"
#include "kleverconfig.h"

#include <QJsonArray>
#include <qcontainerfwd.h>

using namespace std;

QString makeImageString(const QString &href, const QString &text)
{
    return QStringLiteral("<img src=\"") + href + QStringLiteral("\" alt=\"") + text + QStringLiteral("\">");
}

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

        html.push_back(makeImageString(url, i->text()));
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
    if (!justCollectFootnoteRefs) {
        html.push_back(QStringLiteral("</li>\n"));
    }
}

void KleverNotesHtmlVisitor::onCode(MD::Code<MD::QStringTrait> *c)
{
    if (!justCollectFootnoteRefs) {
        static const QString pumlStr = QStringLiteral("puml");
        static const QString plantUMLStr = QStringLiteral("plantuml");

        const QString lang = c->syntax();
        const QString _text = c->text();
        QString returnValue;
        if (KleverConfig::pumlEnabled() && (lang.toLower() == pumlStr || lang.toLower() == plantUMLStr)) {
            QPair<QString, QString> imageInfo = m_pluginHelper->getPUMLParserUtils()->renderCode(_text, KleverConfig::pumlDark());

            returnValue = makeImageString(imageInfo.first, imageInfo.second);
        } else {
            returnValue = m_pluginHelper->getHighlightParserUtils()->renderCode(KleverConfig::codeSynthaxHighlightEnabled(), _text, lang);
        }

        html.push_back(returnValue);
    }
}

void KleverNotesHtmlVisitor::setNotePath(const QString &notePath)
{
    m_notePath = notePath;
}

Parser::Parser(QObject *parent)
    : QObject(parent)
{
    m_renderer = new KleverNotesHtmlVisitor(getPluginHelper());
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
    pluginHelper->clearPluginsPreviousInfo();

    QTextStream s(&src, QIODeviceBase::ReadOnly);

    const auto doc = m_md4qtParser.parse(s, m_notePath, QStringLiteral("local.md"));
    const auto html = m_renderer->toHtml(doc, m_notePath);

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
