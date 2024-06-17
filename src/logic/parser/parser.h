/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#pragma once

#include <QObject>
#include <QSet>

#include "../plugins/pluginHelper.h"
#include "renderer.h"

#define MD4QT_QT_SUPPORT
#include "logic/md4qt/parser.hpp"

namespace MdEditor
{

class EditorHandler;
class Parser : public QObject
{
    /* Q_OBJECT */
    /* Q_PROPERTY(QString notePath WRITE setNotePath) */
    /* // NoteMapper */
    /* Q_PROPERTY(QStringList headerInfo WRITE setHeaderInfo) */
    /* Q_PROPERTY(QString headerLevel READ headerLevel CONSTANT) */
public:
    explicit Parser(EditorHandler *editorHandler = nullptr);

    EditorHandler *editorHandler() const;

    QString parse(QString src);

    QString getNotePath() const;
    void setNotePath(const QString &notePath);
    QVector<QVariantMap> tokens;
    QMap<QString, QMap<QString, QString>> links;

    PluginHelper *getPluginHelper() const;

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
    void addParsePlugins();

    QString renderHtml();

private:
    enum ExtensionID : int {
        /* Extended syntax
         * ===============*/
        ExtendedSyntax = MD::TextPlugin::UserDefinedPluginID + 1,
        /* Plugins
         * ===============*/
        KleverPlugins = ExtendedSyntax + 64, // Keep some margin for other Extended syntax
    };

    EditorHandler *m_editorHandler = nullptr;

    PluginHelper *m_pluginHelper = nullptr;

    QString m_notePath;

    MD::Parser<MD::QStringTrait> m_md4qtParser;

    Renderer *m_renderer = nullptr;
};
}
