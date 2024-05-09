/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#pragma once

#include <QObject>
#include <QSet>

#include "../plugins/pluginHelper.h"

#define MD4QT_QT_SUPPORT
#include "logic/md4qt/html.hpp"
#include "logic/md4qt/parser.hpp"
#include "logic/md4qt/traits.hpp"

// TODO: move to own file (like renderer)
class KleverNotesHtmlVisitor : public MD::details::HtmlVisitor<MD::QStringTrait>
{
public:
    KleverNotesHtmlVisitor(PluginHelper *pluginHelper)
        : MD::details::HtmlVisitor<MD::QStringTrait>()
        , m_pluginHelper(pluginHelper){};
    ~KleverNotesHtmlVisitor() override = default;

    void setNotePath(const QString &notePath);

    void onImage(MD::Image<MD::QStringTrait> *i) override;
    void onListItem(MD::ListItem<MD::QStringTrait> *i, bool first) override;
    void onCode(MD::Code<MD::QStringTrait> *c) override;

protected:
    QString m_notePath;
    PluginHelper *m_pluginHelper;
};
// =====

class Parser : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString notePath WRITE setNotePath)
    // NoteMapper
    Q_PROPERTY(QStringList headerInfo WRITE setHeaderInfo)
    Q_PROPERTY(QString headerLevel READ headerLevel CONSTANT)
public:
    explicit Parser(QObject *parent = nullptr);

    Q_INVOKABLE QString parse(QString src);

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
    QString renderHtml();
    PluginHelper *pluginHelper = new PluginHelper(this);

    QString m_notePath;

    MD::Parser<MD::QStringTrait> m_md4qtParser;

    KleverNotesHtmlVisitor *m_renderer = nullptr;
};
