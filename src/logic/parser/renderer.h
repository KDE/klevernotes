/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

#include "plugins/pluginHelper.h"
#include <utility>

#define MD4QT_QT_SUPPORT
#include "md4qt/html.hpp"
#include "md4qt/traits.hpp"

class Renderer : public MD::details::HtmlVisitor<MD::QStringTrait>
{
public:
    Renderer();
    ~Renderer() override = default;

    // General info
    void setNotePath(const QString &notePath);
    void addPluginHelper(PluginHelper *pluginHelper);
    void addExtendedSyntax(const long long int opts, const QString &openingHTML, const QString &closingHTML);

    // md4qt
    void openStyle(const typename MD::ItemWithOpts<MD::QStringTrait>::Styles &styles) override;
    void closeStyle(const typename MD::ItemWithOpts<MD::QStringTrait>::Styles &styles) override;
    void onImage(MD::Image<MD::QStringTrait> *i) override;
    void onListItem(MD::ListItem<MD::QStringTrait> *i, bool first) override;
    void onCode(MD::Code<MD::QStringTrait> *c) override;
    void onHeading(MD::Heading<MD::QStringTrait> *h, const typename MD::QStringTrait::String &ht) override;
    void onLink(MD::Link<MD::QStringTrait> *l) override;

    static QString code(QString &code);
    static QString openListItem(const bool hasTask = false, const bool isChecked = false, const int startNumber = -1);
    static QString closeListItem(const bool hasTask = false);
    static QString checkbox(bool checked);
    static QString wikilink(const QString &href, const QString &title, const QString &text);
    static QString image(const QString &href, const QString &text);
    static QString text(const QString &text);
    static QString escape(QString &html, bool encode);
    static QString unescape(const QString &html);

protected:
    QString m_notePath;

    // Plugins
    PluginHelper *m_pluginHelper;

    QMap<long long int, std::pair<QString, QString>> m_extendedSyntaxMap;
};
