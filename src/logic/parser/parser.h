/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

// Qt include
#include <QObject>
#include <QSet>

// KleverNotes include
#include "extendedSyntax/extendedSyntaxMaker.hpp"
#include "plugins/emoji/emojiPlugin.hpp"
#include "plugins/noteMapper/noteLinkingPlugin.hpp"
#include "plugins_helper.h"

// md4qt include
#include <md4qt/src/doc.h>
#include <md4qt/src/parser.h>

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

private:
    // Connections
    /**
     * @brief Connects the default KleverNotes plugins to the md4qt parser.
     */
    void connectPlugins();

    // Setters
    /**
     * @brief Add/remove a specific plugin.
     *
     * @param add Whether to add the plugin, remove otherwise.
     */
    template<class Plugin>
    void addRemovePlugin(const bool add)
    {
        auto inlineParsers =
            setInlineParsers<ExtendedSyntaxMaker::SupEmphasisParser, ExtendedSyntaxMaker::SubEmphasisParser, ExtendedSyntaxMaker::HighlightEmphasisParser>();

        if (add) {
            if (std::is_same<Plugin, EmojiPlugin::EmojiParser>::value) {
                m_plugins.insert(PluginID::EmojiPlugin);
            } else if (std::is_same<Plugin, NoteLinkingPlugin::NoteLinkingParser>::value) {
                m_plugins.insert(PluginID::NoteLinkingPlugin);
            }
        } else {
            if (std::is_same<Plugin, EmojiPlugin::EmojiParser>::value) {
                m_plugins.remove(PluginID::EmojiPlugin);
            } else if (std::is_same<Plugin, NoteLinkingPlugin::NoteLinkingParser>::value) {
                m_plugins.remove(PluginID::NoteLinkingPlugin);
            }
        }

        for (const auto &id : std::as_const(m_plugins)) {
            switch (id) {
            case PluginID::EmojiPlugin: {
                addInlinePlugins<EmojiPlugin::EmojiParser>(inlineParsers);
            } break;

            case PluginID::NoteLinkingPlugin: {
                addInlinePlugins<NoteLinkingPlugin::NoteLinkingParser>(inlineParsers);
            } break;

            default:
                break;
            }
        }

        m_md4qtParser.setInlineParsers(inlineParsers);
    }

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
    void done(QSharedPointer<MD::Document> mdDoc, unsigned long long int parseCount);

    /**
     * @brief Note linking option changed.
     *
     * @param on Is on?
     */
    void noteLinkingEnabledChangedSignal(bool on);

    /**
     * @brief Emoji option changed.
     *
     * @param on Is on?
     */
    void quickEmojiEnabledChangedSignal(bool on);

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
     *
     * Invokes on the main thread.
     */
    void noteLinkingEnabledChanged();

    // Emoji
    /**
     * @brief Connection to KleverNotes config for the Quick Emoji Plugin.
     *
     * Invokes on the main thread.
     */
    void quickEmojiEnabledChanged();

    /**
     * @brief Connection to KleverNotes config for the Note Linking Plugin.
     *
     * Invokes on the parsing thread.
     *
     * @param on Is on?
     */
    void noteLinkingEnabledChanged(bool on);

    // Emoji
    /**
     * @brief Connection to KleverNotes config for the Quick Emoji Plugin.
     *
     * Invokes on the parsing thread.
     *
     * @param on Is on?
     */
    void quickEmojiEnabledChanged(bool on);

    // markdown-tools editor
    /**
     * @brief Receive the request to perform parsing.
     */
    void onParse();

private:
    enum class PluginID {
        EmojiPlugin,
        NoteLinkingPlugin
    };

    QSet<PluginID> m_plugins;

    // markdown-tools editor
    QStringList m_data; // Using a QStringList enable us to make the difference between no data and empty data !!
    QString m_noteDir;
    QString m_noteName;
    unsigned long long int m_counter;
    MD::Parser m_md4qtParser;
};
}
