/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#pragma once

#include <QObject>
#include <QSet>

#include "../plugins/pluginHelper.h"
#include "renderer.h"

#define MD4QT_QT_SUPPORT
#include "logic/md4qt/doc.hpp"
#include "logic/md4qt/parser.hpp"
#include "logic/md4qt/traits.hpp"

namespace MdEditor
{

class EditorHandler;
class Parser : public QObject
{
public:
    explicit Parser(EditorHandler *editorHandler = nullptr);

    std::shared_ptr<MD::Document<MD::QStringTrait>> parse(QString src);

    // Getters
    EditorHandler *editorHandler() const;
    QString getNotePath() const;
    PluginHelper *pluginHelper() const;

    // Setters
    void setNotePath(const QString &notePath);
    void addPluginHelper();
    void addExtendedSyntax(const QStringList &details);
    void addPlugin();
    void addPlugins();

    // NoteMapper
    void setHeaderInfo(const QStringList &headerInfo);
    QString headerLevel() const;

Q_SIGNALS:
    // NoteMapper
    void newLinkedNotesInfos(const QSet<QStringList> &linkedNotesInfos);
    void noteHeadersSent(const QString &notePath, const QStringList &noteHeaders);

public Q_SLOTS:
    // Syntax highlight
    void newHighlightStyle();
    // PUML
    void pumlDarkChanged();

private:
    enum ExtensionID : int {
        /* Extended syntax
         * ===============*/
        ExtendedSyntax = MD::TextPlugin::UserDefinedPluginID + 1,
        /* Plugins
         * ===============*/
        KleverPlugins = ExtendedSyntax + 64, // Keep some margin for other Extended syntax
    };

    // KleverNotes
    QString m_notePath;
    Renderer *m_renderer = nullptr;
    PluginHelper *m_pluginHelper = nullptr;

    // md4qt
    MD::Parser<MD::QStringTrait> m_md4qtParser;
    int m_extendedSyntaxCount = 0;
    int m_pluginCount = 0;

    EditorHandler *m_editorHandler = nullptr;
};
}
