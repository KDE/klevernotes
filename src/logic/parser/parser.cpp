/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023-2024 Louis Schul <schul9louis@gmail.com>
*/

#include "parser.h"

#include "extendedSyntax/extendedSyntaxMaker.hpp"
#include "md4qtDataCleaner.hpp"

namespace MdEditor
{
Parser::Parser(QObject *parent)
    : QObject(parent)
{
    m_md4qtParser.addTextPlugin(1024, md4qtDataCleaner::dataCleaningFunc, false, {});
}

std::shared_ptr<MD::Document<MD::QStringTrait>> Parser::parse(QString src)
{
    QTextStream s(&src, QIODeviceBase::ReadOnly);

    return m_md4qtParser.parse(s, m_notePath, QStringLiteral("note.md"));
}

// Getters
// =======
QString Parser::getNotePath() const
{
    return m_notePath;
}
// !Getters

// Setters
// =======
void Parser::setNotePath(const QString &notePath)
{
    if (notePath.isEmpty() || m_notePath == notePath) {
        return;
    }
    m_notePath = notePath;
}

void Parser::addExtendedSyntax(const QStringList &details)
{
    m_md4qtParser.addTextPlugin(details.last().toInt(), ExtendedSyntaxMaker::extendedSyntaxHelperFunc, true, details);
}
// !Setters
}
