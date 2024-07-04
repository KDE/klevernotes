/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>
*/

#include "logic/parser/parser.h"
#include "logic/editor/editorHandler.hpp"
#include "logic/extendedSyntax/extendedSyntaxMaker.hpp"
#include "logic/plugins/pluginHelper.h"

namespace MdEditor
{
Parser::Parser(EditorHandler *editorHandler)
    : m_editorHandler(editorHandler)
{
    m_renderer = new Renderer();
}

std::shared_ptr<MD::Document<MD::QStringTrait>> Parser::parse(QString src)
{
    if (m_pluginHelper) {
        m_pluginHelper->clearPluginsInfo();
    }

    QTextStream s(&src, QIODeviceBase::ReadOnly);

    return m_md4qtParser.parse(s, m_notePath, QStringLiteral("note.md"));
}

// Getters
// =======
EditorHandler *Parser::editorHandler() const
{
    return m_editorHandler;
}

PluginHelper *Parser::pluginHelper() const
{
    return m_pluginHelper;
}

QString Parser::getNotePath() const
{
    return m_notePath;
}
// !Getters

// Setters
// =======
void Parser::setNotePath(const QString &notePath)
{
    if (notePath.isEmpty() || m_notePath == notePath) {
        return;
    }

    m_notePath = notePath;

    if (notePath == QStringLiteral("qrc:")) {
        return;
    }

    if (m_pluginHelper) {
        // We do this here because we're sure to be in another note
        m_pluginHelper->clearPluginsPreviousInfo();

        m_pluginHelper->mapperParserUtils()->setNotePath(notePath);
    }
}

void Parser::addPluginHelper()
{
    /* if (!m_pluginHelper) { */
    /*     m_pluginHelper = new PluginHelper(this); */
    /**/
    /*     m_renderer->addPluginHelper(m_pluginHelper); */
    /*     m_pluginHelper->mapperParserUtils()->setNotePath(m_notePath); */
    /* } */
}

void Parser::addExtendedSyntax(const QStringList &details)
{
    m_md4qtParser.addTextPlugin(details.last().toInt(), ExtendedSyntaxMaker::extendedSyntaxHelperFunc, true, details);
    ++m_extendedSyntaxCount;
}

void Parser::addPlugin()
{
}

void Parser::addPlugins()
{
    /**/
    /* static const auto kleverPluginsList = { */
    /*     NoteMapperFunc::noteLinkingExtension, */
    /* }; */

    /* int pluginCount = 0; */
    /* for (auto &func : kleverPluginsList) { */
    /*     m_md4qtParser.addTextPlugin(ExtensionID::KleverPlugins + pluginCount, func, false); */
    /*     pluginCount++; */
    /* } */
}
// !Setters

// Plugins
// =======

// NoteMapper
void Parser::setHeaderInfo(const QStringList &headerInfo)
{
    if (m_pluginHelper) {
        m_pluginHelper->mapperParserUtils()->setHeaderInfo(headerInfo);
    }
}

QString Parser::headerLevel() const
{
    if (m_pluginHelper) {
        return m_pluginHelper->mapperParserUtils()->headerLevel();
    }
    return QStringLiteral("0");
};
// !NoteMapper

// Code highlight
void Parser::newHighlightStyle()
{
    if (m_pluginHelper) {
        m_pluginHelper->highlightParserUtils()->newHighlightStyle();
    }
}
// !Code highlight

// PUML
void Parser::pumlDarkChanged()
{
    if (m_pluginHelper) {
        m_pluginHelper->pumlParserUtils()->pumlDarkChanged();
    }
}
// !PUML

// !Plugins
// ========
}
