// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QVariantMap>

class QAbstractItemModel;
class KColorSchemeManager;

class ColorSchemer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel *model READ model CONSTANT)
public:
    explicit ColorSchemer(QObject *parent = nullptr);

    QAbstractItemModel *model() const;
    Q_INVOKABLE void apply(int idx);
    Q_INVOKABLE void apply(const QString &name);
    Q_INVOKABLE int indexForScheme(const QString &name) const;
    Q_INVOKABLE QString nameForIndex(int index) const;
    Q_INVOKABLE QStringList getUsefullColors(int index) const;

private:
    KColorSchemeManager *c;
};
