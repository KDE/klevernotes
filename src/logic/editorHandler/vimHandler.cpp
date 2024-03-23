// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "vimHandler.h"
#include <QRegularExpression>
// #include <QDebug>

VimHandler::VimHandler(QObject *parent)
    : QObject(parent)
{
}

void VimHandler::handleKeyPress(const QKeyEvent &event)
{
    // TODO
    Q_UNUSED(event);
}

void VimHandler::setVimOn(const bool vimOn)
{
    m_vimOn = vimOn;
}

bool VimHandler::getVimOn()
{
    return m_vimOn;
}

void VimHandler::setTextArea(QObject *textArea)
{
    m_textArea = textArea;
}

QObject *VimHandler::getTextArea()
{
    return m_textArea;
}
