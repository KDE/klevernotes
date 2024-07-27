/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/

#pragma once

// Qt include
#include <QObject>
#include <QSet>

// md4qt include
#define MD4QT_QT_SUPPORT
#include "md4qt/doc.hpp"
#include "md4qt/parser.hpp"
#include "md4qt/traits.hpp"

namespace MdEditor
{

class Parser : public QObject
{
public:
    explicit Parser(QObject *parent = nullptr);
    std::shared_ptr<MD::Document<MD::QStringTrait>> parse(QString src);

    // Getters
    QString getNotePath() const;

    // Setters
    void setNotePath(const QString &notePath);
    void addExtendedSyntax(const QStringList &details);
    void addRemovePlugin(const int pluginId, const bool add);

private:
    // KleverNotes
    QString m_notePath;

    // md4qt
    MD::Parser<MD::QStringTrait> m_md4qtParser;
};
}
