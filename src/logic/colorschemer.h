// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QQmlEngine>
#include <QVariantMap>

class QAbstractItemModel;
class KColorSchemeManager;

class ColorSchemer : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QAbstractItemModel *model READ model CONSTANT)
public:
    explicit ColorSchemer(QObject *parent = nullptr);

    /**
     * @brief Get a model with all the available color schemes.
     *
     * @return QAbstractItemModel containing color schemes.
     */
    QAbstractItemModel *model() const;

    /**
     * @brief Apply a color scheme based on its index.
     *
     * @param idx The index of the wanted color scheme in the model.
     */
    Q_INVOKABLE void apply(int idx);

    /**
     * @brief Apply a color scheme based on its name.
     *
     * @param name The name of the color scheme.
     */
    Q_INVOKABLE void apply(const QString &name);

    /**
     * @brief Get the model index of a color scheme based on its name.
     *
     * @param name The name of the color scheme.
     * @return The index of the color scheme inside the model.
     */
    Q_INVOKABLE int indexForScheme(const QString &name) const;

    /**
     * @brief Get the name of a color scheme based on its index.
     *
     * @param index The index of the color scheme inside the model.
     * @return The name of the color color scheme.
     */
    Q_INVOKABLE QString nameForIndex(int index) const;

    /**
     * @brief Get all the color scheme colors used by KleverNotes.
     *
     * @param index The index of the color scheme in the model.
     */
    Q_INVOKABLE QStringList getUsefullColors(int index) const;

private:
    KColorSchemeManager *c;
};
