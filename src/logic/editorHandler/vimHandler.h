// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include <QKeyEvent>
#include <QObject>
#include <QString>

class VimHandler : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *textArea READ getTextArea WRITE setTextArea)
    Q_PROPERTY(bool vimOn READ getVimOn WRITE setVimOn)

public:
    explicit VimHandler(QObject *parent = nullptr);

    Q_INVOKABLE void handleKeyPress(const QKeyEvent &event);

private:
    bool getVimOn();
    void setVimOn(const bool vimOn);
    bool m_vimOn = false;

    QObject *getTextArea();
    void setTextArea(QObject *textArea);
    QObject *m_textArea = nullptr;
};
