/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023-2025 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "plugins/emoji/emojiPlugin.hpp"
#include "plugins/pluginHelper.h"
#include <utility>

#define MD4QT_QT_SUPPORT
#include "md4qt/html.h"
#include "md4qt/traits.h"

/**
 * @class Renderer
 * @brief MD::HtmlVisitor with custom rendering.
 */
class Renderer : public MD::details::HtmlVisitor<MD::QStringTrait>
{
public:
    Renderer();
    ~Renderer() override = default;

    // General info
    /**
     * @brief Get the current note directory.
     *
     * @return The current note directory.
     */
    QString getNoteDir() const;

    /**
     * @brief Set the current note directory.
     *
     * @param notePath The current note path to the directory.
     */
    void setNoteDir(const QString &notePath);

    /**
     * @brief Add the PluginHelper.
     *
     * @param pluginHelper The PluginHelper.
     */
    void addPluginHelper(PluginHelper *pluginHelper);

    /**
     * @brief Add an extended syntax in the renderer.
     *
     * @param opts The unique opts value of the extended syntax.
     * @param openingHTML The opening HTML tag(s) for the extended syntax.
     * @param closingHTML The closing HTML tag(s) for the extended syntax.
     */
    void addExtendedSyntax(const long long int opts, const QString &openingHTML, const QString &closingHTML);

    // Plugins
    /**
     * @brief Set whether the PUML plugin is enable or not.
     *
     * @param enable Whether the plugin is enable.
     */
    void setPUMLenable(const bool enable);

    /**
     * @brief Set whether the PUML plugin is should use dark background.
     *
     * @param enable Whether the plugin uses dark background.
     */
    void setPUMLdark(const bool dark);

    /**
     * @brief Set whether the code highlighting plugin is enable or not.
     *
     * @param enable Whether the plugin is enable.
     */
    void setCodeHighlightEnable(const bool enable);

    using Base = MD::details::HtmlVisitor<MD::QStringTrait>;
    // md4qt
    void openStyle(const typename MD::ItemWithOpts<MD::QStringTrait>::Styles &styles) override;
    using Base::openStyle;

    void closeStyle(const typename MD::ItemWithOpts<MD::QStringTrait>::Styles &styles) override;
    using Base::closeStyle;

    void onImage(MD::Image<MD::QStringTrait> *i) override;
    using Base::onImage;

    void onListItem(MD::ListItem<MD::QStringTrait> *i, bool first, bool skipOpeningWrap) override;
    using Base::onListItem;

    void onCode(MD::Code<MD::QStringTrait> *c) override;
    using Base::onCode;

    void onHeading(MD::Heading<MD::QStringTrait> *h, const typename MD::QStringTrait::String &ht) override;
    using Base::onHeading;

    void onLink(MD::Link<MD::QStringTrait> *l) override;
    using Base::onLink;

    void onUserDefined(MD::Item<MD::QStringTrait> *item) override;
    using Base::onUserDefined;

    // Custom
    /**
     * @brief Helper function to handle the EmojiItem (custom items).
     * This helper function is used by `onUserDefined`.
     *
     * @param e The EmojiItem
     */
    void onEmoji(EmojiPlugin::EmojiItem *e);

    /**
     * @brief Helper function to `onCode` method, to make custom block of code HTML tags.
     *
     * @param code The block of code content.
     * @return The custom HTML block of code.
     */
    static QString code(QString &code);

    /**
     * @brief Helper function to `onListItem` to create custom opening HTML tag for list item.
     *
     * @param hasTask Whether the item is a task (checkbox).
     * @param isChecked Whether the checkbox should be checked.
     * @param startNumber Which number should be added if it is an ordered list. -1 will result in an unordered list item.
     * @return The custom HTML opening tag for a list item.
     */
    static QString openListItem(const bool hasTask = false, const bool isChecked = false, const int startNumber = -1);

    /**
     * @brief Helper function to `onListItem` to create custom closing HTML tag for list item.
     *
     * @param hasTask Whether the item is a task (checkbox).
     * @return The custom HTML closing tag for a list item.
     */
    static QString closeListItem(const bool hasTask = false);

    /**
     * @brief Create a checkbox HTML tag.
     *
     * @param checked Whether the checkbox should be checked.
     * @return A HTML checkbox.
     */
    static QString checkbox(bool checked);

    /**
     * @brief Create a wikilink (custom HTML link).
     *
     * @param href The href of the link.
     * @param title The title of the link.
     * @param text The text of the link.
     * @return A HTML link.
     */
    static QString wikilink(const QString &href, const QString &title, const QString &text);

    /**
     * @brief Create HTML image.
     *
     * @param href The href of the image.
     * @param text The alt text for the image.
     * @return A HTML image.
     */
    static QString image(const QString &href, const QString &text);

    // TODO: NOT IMPLEMENTED, REMOVE THIS
    static QString text(const QString &text);

    /**
     * @brief Escape the special caracters in the given HTML.
     *
     * @param html The HTML to escape.
     * @param encode Whether some specific escape should be encoded.
     * @return The escaped HTML.
     */
    static QString escape(QString &html, bool encode);

    /**
     * @brief Unescape the given HTML.
     *
     * @param html The HTML to unescape.
     * @return The unescaped HTML.
     */
    static QString unescape(const QString &html);

protected:
    QString m_noteDir;

    QMap<long long int, std::pair<QString, QString>> m_extendedSyntaxMap;

    // Plugins
    PluginHelper *m_pluginHelper;

    bool m_pumlEnable = false;
    bool m_pumlDark = false;

    bool m_codeHighlight = false;
};
