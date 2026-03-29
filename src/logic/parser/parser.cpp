/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>
*/

#include "parser.h"

#include "extendedSyntax/extendedSyntaxMaker.hpp"

#include "kleverconfig.h"

namespace MdEditor
{
Parser::Parser()
{
    connect(this, &Parser::newData, this, &Parser::onParse, Qt::QueuedConnection);
    connectPlugins();
}

// Connections
// ===========
void Parser::connectPlugins()
{
    // Note Linking
    connect(KleverConfig::self(), &KleverConfig::noteMapEnabledChanged, this, qOverload<>(&Parser::noteLinkingEnabledChanged), Qt::DirectConnection);
    connect(this, &Parser::noteLinkingEnabledChangedSignal, this, qOverload<bool>(&Parser::noteLinkingEnabledChanged), Qt::QueuedConnection);
    noteLinkingEnabledChanged();

    // Emoji
    connect(KleverConfig::self(), &KleverConfig::quickEmojiEnabledChanged, this, qOverload<>(&Parser::quickEmojiEnabledChanged), Qt::DirectConnection);
    connect(this, &Parser::quickEmojiEnabledChangedSignal, this, qOverload<bool>(&Parser::quickEmojiEnabledChanged), Qt::QueuedConnection);
    quickEmojiEnabledChanged();
}
// !Connections

// KleverNotes slots
// =================
void Parser::noteLinkingEnabledChanged()
{
    Q_EMIT noteLinkingEnabledChangedSignal(KleverConfig::noteMapEnabled());
}

void Parser::quickEmojiEnabledChanged()
{
    Q_EMIT quickEmojiEnabledChangedSignal(KleverConfig::quickEmojiEnabled());
}

void Parser::noteLinkingEnabledChanged(bool on)
{
    addRemovePlugin<NoteLinkingPlugin::NoteLinkingParser>(on);
}

void Parser::quickEmojiEnabledChanged(bool on)
{
    addRemovePlugin<EmojiPlugin::EmojiParser>(on);
}
// !KleverNotes slots

// markdown-tools editor slots
void Parser::onData(const QString &md, const QString &noteDir, const QString &noteName, unsigned long long int counter)
{
    m_data.clear();
    m_data.push_back(md);
    m_noteDir = noteDir;
    m_noteName = noteName;
    m_counter = counter;

    Q_EMIT newData();
}

void Parser::onParse()
{
    if (!m_data.isEmpty()) {
        QTextStream stream(&m_data.back());

        const auto doc = m_md4qtParser.parse(stream, m_noteDir, m_noteName);

        m_data.clear();

        Q_EMIT done(doc, m_counter);
    }
}
// !markdown-tools editor slots
}
