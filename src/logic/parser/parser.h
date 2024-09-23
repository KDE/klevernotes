/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

// Qt include
#include <QObject>
#include <QSet>

// KleverNotes include
#include "plugins/emoji/emojiPlugin.hpp"
#include "plugins/noteMapper/noteLinkingPlugin.hpp"

// md4qt include
#define MD4QT_QT_SUPPORT
#include "md4qt/doc.h"
#include "md4qt/parser.h"
#include "md4qt/traits.h"

namespace MdEditor
{

class Parser : public QObject
{
public:
    explicit Parser(QObject *parent = nullptr);
    std::shared_ptr<MD::Document<MD::QStringTrait>> parse(QString src);

    // Connections
    void connectPlugins();

    // Getters
    QString getNotePath() const;

    // Setters
    void setNotePath(const QString &notePath);
    void addExtendedSyntax(const QStringList &details);
    void addRemovePlugin(const int pluginId, const bool add);

private Q_SLOTS:
    // Note Linking
    void noteLinkindEnabledChanged();

    // Emoji
    void quickEmojiEnabledChanged();

private:
    // KleverNotes
    QString m_notePath;

    enum PluginsId : int {
        NoteLinkingPlugin = 320, // EditorHandler::ExtensionID::KleverPlugins
        EmojiPlugin,
    };

    const std::map<int, MD::TextPluginFunc<MD::QStringTrait>> m_kleverPlugins = {
        {PluginsId::NoteLinkingPlugin, NoteLinkingPlugin::noteLinkingHelperFunc},
        {PluginsId::EmojiPlugin, EmojiPlugin::emojiHelperFunc},
    };

    // md4qt
    MD::Parser<MD::QStringTrait> m_md4qtParser;
};
}
