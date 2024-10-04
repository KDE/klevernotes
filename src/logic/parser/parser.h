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
    Q_OBJECT

public:
    explicit Parser();

    // Connections
    void connectPlugins();

    // Setters
    void addExtendedSyntax(const QStringList &details);
    void addRemovePlugin(const int pluginId, const bool add);

Q_SIGNALS:
    void newData();
    void done(std::shared_ptr<MD::Document<MD::QStringTrait>> mdDoc, unsigned long long int parseCount);

public Q_SLOTS:
    // markdown-tools editor
    void onData(const QString &md, const QString &notePath, unsigned long long int counter);

private Q_SLOTS:
    // Note Linking
    void noteLinkindEnabledChanged();

    // Emoji
    void quickEmojiEnabledChanged();

    // markdown-tools editor
    void onParse();

private:
    enum PluginsId : int {
        NoteLinkingPlugin = 320, // EditorHandler::ExtensionID::KleverPlugins
        EmojiPlugin,
    };

    const std::map<int, MD::TextPluginFunc<MD::QStringTrait>> m_kleverPlugins = {
        {PluginsId::NoteLinkingPlugin, NoteLinkingPlugin::noteLinkingHelperFunc},
        {PluginsId::EmojiPlugin, EmojiPlugin::emojiHelperFunc},
    };

    // markdown-tools editor
    QStringList m_data; // Using a QStringList enable us to make the difference between no data and empty data !!
    QString m_notePath;
    unsigned long long int m_counter;
    MD::Parser<MD::QStringTrait> m_md4qtParser;
};
}
