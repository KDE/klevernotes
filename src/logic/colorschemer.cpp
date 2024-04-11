// SPDX-FileCopyrightText: 2021 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <KColorScheme>
#include <KColorSchemeManager>
#include <KSharedConfig>
#include <QAbstractItemModel>

#include "colorschemer.h"
// #include <QDebug>

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
    const QString schemePath = c->model()->data(c->model()->index(index, 0), Qt::UserRole).toString();

    const KSharedConfigPtr schemeConfig = KSharedConfig::openConfig(schemePath, KConfig::SimpleConfig);

    const KColorScheme activeView(QPalette::Active, KColorScheme::Window, schemeConfig);
    const KColorScheme selectionView(QPalette::Active, KColorScheme::Selection, schemeConfig);

    const QString bodyColor = activeView.background(activeView.NormalBackground).color().name(QColor::HexRgb);
    const QString textColor = activeView.foreground(activeView.NormalText).color().name(QColor::HexRgb);
    const QString titleColor = activeView.foreground(activeView.InactiveText).color().name(QColor::HexRgb);
    const QString linkColor = activeView.foreground(activeView.LinkText).color().name(QColor::HexRgb);
    const QString visitedLinkColor = activeView.foreground(activeView.VisitedText).color().name(QColor::HexRgb);
    const QString codeColor = activeView.background(activeView.AlternateBackground).color().name(QColor::HexRgb);
    const QString highlightColor = selectionView.background().color().name(QColor::HexRgb);

    const QVariantMap res = {
        {QStringLiteral("--bodyColor"), bodyColor},
        {QStringLiteral("--textColor"), textColor},
        {QStringLiteral("--titleColor"), titleColor},
        {QStringLiteral("--linkColor"), linkColor},
        {QStringLiteral("--visitedLinkColor"), visitedLinkColor},
        {QStringLiteral("--codeColor"), codeColor},
        {QStringLiteral("--highlightColor"), highlightColor},
    };

    return res;
}

#include "moc_colorschemer.cpp"
