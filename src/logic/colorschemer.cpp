// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <KColorSchemeManager>
#include <KSharedConfig>
#include <QAbstractItemModel>
#include <KColorScheme>

#include "colorschemer.h"
#include <QDebug>

ColorSchemer::ColorSchemer(QObject *parent)
    : QObject(parent)
    , c(new KColorSchemeManager(this))
{
}

QAbstractItemModel *ColorSchemer::model() const
{
    return c->model();
}

void ColorSchemer::apply(int idx)
{
    c->activateScheme(c->model()->index(idx, 0));
}

void ColorSchemer::apply(const QString &name)
{
    c->activateScheme(c->indexForScheme(name));
}

int ColorSchemer::indexForScheme(const QString &name) const
{
    auto index = c->indexForScheme(name).row();
    if (index == -1) {
        index = 0;
    }
    return index;
}

QString ColorSchemer::nameForIndex(int index) const
{
    return c->model()->data(c->model()->index(index, 0), Qt::DisplayRole).toString();
}

QVariantMap ColorSchemer::getUsefullColors(int index) const
{
    QString schemePath = c->model()->data(c->model()->index(index, 0), Qt::UserRole).toString();

    KSharedConfigPtr schemeConfig = KSharedConfig::openConfig(schemePath, KConfig::SimpleConfig);

    KColorScheme activeView(QPalette::Active, KColorScheme::View, schemeConfig);

    QString bodyColor = activeView.background(activeView.NormalBackground).color().name(QColor::HexRgb);
    QString textColor = activeView.foreground(activeView.NormalText).color().name(QColor::HexRgb);
    QString titleColor = activeView.foreground(activeView.InactiveText).color().name(QColor::HexRgb);
    QString linkColor = activeView.foreground(activeView.LinkText).color().name(QColor::HexRgb);
    QString visitedLinkColor = activeView.foreground(activeView.VisitedText).color().name(QColor::HexRgb);
    QString codeColor = activeView.background(activeView.AlternateBackground).color().name(QColor::HexRgb);

    QVariantMap res;
    res.insert("--bodyColor", bodyColor);
    res.insert("--textColor", textColor);
    res.insert("--titleColor", titleColor);
    res.insert("--linkColor", linkColor);
    res.insert("--visitedLinkColor", visitedLinkColor);
    res.insert("--codeColor", codeColor);

    return res;
}

