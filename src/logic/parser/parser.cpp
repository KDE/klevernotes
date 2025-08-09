/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>
*/

#include "parser.h"

#include "extendedSyntax/extendedSyntaxMaker.hpp"
#include "md4qtDataCleaner.hpp"

#include "kleverconfig.h"

namespace MdEditor
{
Parser::Parser()
{
    connect(this, &Parser::newData, this, &Parser::onParse, Qt::QueuedConnection);
    connectPlugins();
    m_md4qtParser.addTextPlugin(md4qtDataCleaner::dataCleanerId, md4qtDataCleaner::dataCleaningFunc, false, {});
}

// Connections
// ===========
void Parser::connectPlugins()
{
    // Note Linking
    connect(KleverConfig::self(), &KleverConfig::noteMapEnabledChanged, this, &Parser::noteLinkindEnabledChanged);
    noteLinkindEnabledChanged();

    // Emoji
    connect(KleverConfig::self(), &KleverConfig::quickEmojiEnabledChanged, this, &Parser::quickEmojiEnabledChanged);
    quickEmojiEnabledChanged();
}
// !Connections

// Setters
// =======
void Parser::addExtendedSyntax(const QStringList &details)
{
    m_md4qtParser.addTextPlugin(static_cast<MD::TextPlugin>(details.last().toInt()), ExtendedSyntaxMaker::extendedSyntaxHelperFunc, true, details);
}

void Parser::addRemovePlugin(const int pluginId, const bool add)
{
    if (add) {
        m_md4qtParser.addTextPlugin(static_cast<MD::TextPlugin>(pluginId), m_kleverPlugins.at(pluginId), true, {});
    } else {
        m_md4qtParser.removeTextPlugin(static_cast<MD::TextPlugin>(pluginId));
    }
}
// !Setters

// KleverNotes slots
// =================
void Parser::noteLinkindEnabledChanged()
{
    addRemovePlugin(PluginsId::NoteLinkingPlugin, KleverConfig::noteMapEnabled());
}

void Parser::quickEmojiEnabledChanged()
{
    addRemovePlugin(PluginsId::EmojiPlugin, KleverConfig::quickEmojiEnabled());
}
// !KleverNotes slots

// markdown-tools editor slots
void Parser::onData(const QString &md, const QString &notePath, unsigned long long int counter)
{
    m_data.clear();
    m_data.push_back(md);
    m_notePath = notePath;
    m_counter = counter;

    Q_EMIT newData();
}

void Parser::onParse()
{
    if (!m_data.isEmpty()) {
        QTextStream stream(&m_data.back());

        const auto doc = m_md4qtParser.parse(stream, m_notePath, QStringLiteral("note.md"), false);

        m_data.clear();

        Q_EMIT done(doc, m_counter);
    }
}
// !markdown-tools editor slots
}
