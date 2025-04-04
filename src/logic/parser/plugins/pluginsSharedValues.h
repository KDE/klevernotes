/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2025 Louis Schul <schul9louis@gmail.com>
*/

namespace PluginsSharedValues
{

constexpr int ExtendedSyntax = 256; // MD::TextPlugin::UserDefinedPluginID + 1

enum PluginsId : int {
    NoteLinkingPlugin = ExtendedSyntax + 64,
    HeaderLinkingPlugin,
    EmojiPlugin,
};

enum CustomType : int {
    Emoji = 256, // MD::ItemType::UserDefined + 1
    HeaderLinking,
};
}
