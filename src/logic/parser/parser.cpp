/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#include "logic/parser/parser.h"

Parser::Parser(QObject *parent)
    : QObject(parent)
{
    m_pluginHelper = new PluginHelper(this);
    m_renderer = new Renderer(m_pluginHelper);
}

PluginHelper *Parser::getPluginHelper() const
{
    return m_pluginHelper;
}

void Parser::setHeaderInfo(const QStringList &headerInfo)
{
    m_pluginHelper->getMapperParserUtils()->setHeaderInfo(headerInfo);
}

QString Parser::headerLevel() const
{
    return m_pluginHelper->getMapperParserUtils()->headerLevel();
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
    m_pluginHelper->clearPluginsInfo();

    m_pluginHelper->getMapperParserUtils()->setPathsInfo(notePath);
}

QString Parser::getNotePath() const
{
    return m_notePath;
}

QString Parser::parse(QString src)
{
    m_pluginHelper->clearPluginsPreviousInfo();

    QTextStream s(&src, QIODeviceBase::ReadOnly);

    const auto doc = m_md4qtParser.parse(s, m_notePath, QStringLiteral("local.md"));
    const auto html = m_renderer->toHtml(doc, m_notePath);

    return html;
}

// Syntax highlight
void Parser::newHighlightStyle()
{
    m_pluginHelper->getHighlightParserUtils()->newHighlightStyle();
}

// PUML
void Parser::pumlDarkChanged()
{
    m_pluginHelper->getPUMLParserUtils()->pumlDarkChanged();
}
