// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#pragma once

#include "kleverconfig.h"

// Qt include
#include <QObject>

class StyleHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool printBackground READ printBackground WRITE setPrintBackground)
    Q_PROPERTY(bool inMain READ inMain WRITE setInMain)

public:
    explicit StyleHandler(QObject *parent = nullptr);

    Q_INVOKABLE void loadStyle();
    Q_INVOKABLE void changeStyles(const QStringList &styleInfo = {});
    Q_INVOKABLE void setDefault(const QStringList &defaultStyle);

    bool printBackground() const;
    void setPrintBackground(const bool printBackground);

    bool inMain() const;
    void setInMain(const bool inMain);

Q_SIGNALS:
    void styleChanged(const QStringList styleInfo);
    void newCss(const QString &css);

private Q_SLOTS:
    void changeStyle();

private:
    void connectStyles();

    void makeCss();

private:
    KleverConfig *m_config;
    QString m_style;

    QStringList m_defaultStyle;
    bool m_defaultSet = false;
    QStringList m_styleInfo;

    bool m_printBackground = true;
    bool m_inMain = true;
};
