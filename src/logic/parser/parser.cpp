/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

#include "logic/parser/parser.h"
#include "logic/extendedSyntax/textHighlightExtension.hpp"
#include "logic/plugins/noteMapper/noteLinkingExtension.hpp"

Parser::Parser(QObject *parent)
    : QObject(parent)
{
    m_pluginHelper = new PluginHelper(this);
    m_renderer = new Renderer(m_pluginHelper);

    addParsePlugins();
}

void Parser::addParsePlugins()
{
    static const auto extendedSyntaxsList = {
        TextHighlightFunc::textHighlightExtension,
    };

    static const auto kleverPluginsList = {
        NoteMapperFunc::noteLinkingExtension,
    };

    int extendedCount = 0;
    for (auto &func : extendedSyntaxsList) {
        m_md4qtParser.addTextPlugin(ExtensionID::ExtendedSyntax + extendedCount, func, true);
        extendedCount++;
    }

    /* int pluginCount = 0; */
    /* for (auto &func : kleverPluginsList) { */
    /*     m_md4qtParser.addTextPlugin(ExtensionID::KleverPlugins + pluginCount, func, false); */
    /*     pluginCount++; */
    /* } */
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
    m_pluginHelper->clearPluginsPreviousInfo();

    m_pluginHelper->getMapperParserUtils()->setNotePath(notePath);
}

QString Parser::getNotePath() const
{
    return m_notePath;
}

QString Parser::parse(QString src)
{
    m_pluginHelper->clearPluginsInfo();

    QTextStream s(&src, QIODeviceBase::ReadOnly);

    const auto doc = m_md4qtParser.parse(s, m_notePath, QStringLiteral("note.md"));
    const auto html = m_renderer->toHtml(doc, m_notePath);

    m_pluginHelper->postTokChanges();

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
