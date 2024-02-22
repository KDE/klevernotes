// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
#pragma once

#include <QObject>

class MDHandler : public QObject
{
    Q_OBJECT

public:
    explicit MDHandler(QObject *parent = nullptr);

    enum Instructions {
        Apply = Qt::UserRole + 1,
        Remove,
        None,
    };

    Q_INVOKABLE QList<int> getBlockLimits(const int selectionStart, const int selectionEnd, const QString &text) const;
    Q_INVOKABLE QString getNewText(const QString &baseText, const QStringList &charsList, bool multiPlaceApply, bool applyIncrement, bool checkByBlock) const;

private:
    Q_INVOKABLE QList<int>
    getPerLineInstructions(const QStringList &lines, const QStringList &charsList, const bool checkLineEnd, const bool applyIncrement) const;
    int getBlockInstruction(const QString &selectedText, const QStringList &charsList) const;
};
