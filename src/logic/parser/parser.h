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

/**
 * @class Parser
 * @brief Wrapper class for `md4qt` parser, with added methods and slots.
 *
 */
class Parser : public QObject
{
    Q_OBJECT

public:
    explicit Parser();

    // Connections
    /**
     * @brief Connects the default KleverNotes plugins to the md4qt parser.
     */
    void connectPlugins();

    // Setters
    /**
     * @brief Add an extended syntax to the md4qt parser.
     *
     * @param details The details of the extended syntax.
     */
    void addExtendedSyntax(const QStringList &details);

    /**
     * @brief Add/remove a specific plugin.
     *
     * @param pluginId The id of the plugin inside the md4qt parser.
     * @param add Whether to add the plugin, remove otherwise.
     */
    void addRemovePlugin(const int pluginId, const bool add);

Q_SIGNALS:
    /**
     * @brief Tells the parser that new data is available.
     */
    void newData();

    /**
     * @brief The parsing is finished.
     *
     * @param mdDoc The resulting MD::Document.
     * @param parseCount The parsing number related to this MD::Document. Used for multithreading.
     */
    void done(std::shared_ptr<MD::Document<MD::QStringTrait>> mdDoc, unsigned long long int parseCount);

public Q_SLOTS:
    // markdown-tools editor
    /**
     * @brief Receive the newly available data.
     *
     * @param md The markdown text to be parsed.
     * @param noteDir The directory in which the note is located.
     * @param noteName The name of the note.
     * @param counter The number that will be used as the `parseCount` for the `done` signal.
     */
    void onData(const QString &md, const QString &noteDir, const QString &noteName, unsigned long long int counter);

private Q_SLOTS:
    // Note Linking
    /**
     * @brief Connection to KleverNotes config for the Note Linking Plugin.
     */
    void noteLinkindEnabledChanged();

    // Emoji
    /**
     * @brief Connection to KleverNotes config for the Quick Emoji Plugin.
     */
    void quickEmojiEnabledChanged();

    // markdown-tools editor
    /**
     * @brief Receive the request to perform parsing.
     */
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
    QString m_noteDir;
    QString m_noteName;
    unsigned long long int m_counter;
    MD::Parser<MD::QStringTrait> m_md4qtParser;
};
}
