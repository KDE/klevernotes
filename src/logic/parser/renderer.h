/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#pragma once

#include "../plugins/pluginHelper.h"

#define MD4QT_QT_SUPPORT
#include "logic/md4qt/html.hpp"
#include "logic/md4qt/traits.hpp"

class Renderer : public MD::details::HtmlVisitor<MD::QStringTrait>
{
public:
    Renderer(PluginHelper *pluginHelper);
    ~Renderer() override = default;

    void setNotePath(const QString &notePath);

    void onImage(MD::Image<MD::QStringTrait> *i) override;
    void onListItem(MD::ListItem<MD::QStringTrait> *i, bool first) override;
    void onCode(MD::Code<MD::QStringTrait> *c) override;

    static QString code(QString &code, const bool highlight);
    static QString openListItem(const bool hasTask = false, const bool isChecked = false, const int startNumber = -1);
    static QString closeListItem(const bool hasTask = false);
    static QString checkbox(bool checked);
    static QString superscript(const QString &text);
    static QString subscript(const QString &text);
    static QString mark(const QString &text);
    static QString wikilink(const QString &href, const QString &title, const QString &text);
    static QString image(const QString &href, const QString &text);
    static QString text(const QString &text);
    static QString escape(QString &html, bool encode);
    static QString unescape(const QString &html);

protected:
    QString m_notePath;
    PluginHelper *m_pluginHelper;
};
