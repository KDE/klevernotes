// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "styleHandler.hpp"

// KleverNotes includes
#include "logic/documentHandler.h"

// Qt includes
#include <QFontInfo>

StyleHandler::StyleHandler(QObject *parent)
    : QObject(parent)
    , m_config(KleverConfig::self())
{
    connectStyles();
}

void StyleHandler::connectStyles()
{
    connect(m_config, &KleverConfig::stylePathChanged, this, &StyleHandler::loadStyle);

    connect(m_config, &KleverConfig::viewBodyColorChanged, this, &StyleHandler::changeStyle);
    connect(m_config, &KleverConfig::viewFontChanged, this, &StyleHandler::changeStyle);
    connect(m_config, &KleverConfig::viewTextColorChanged, this, &StyleHandler::changeStyle);
    connect(m_config, &KleverConfig::viewTitleColorChanged, this, &StyleHandler::changeStyle);
    connect(m_config, &KleverConfig::viewLinkColorChanged, this, &StyleHandler::changeStyle);
    connect(m_config, &KleverConfig::viewVisitedLinkColorChanged, this, &StyleHandler::changeStyle);
    connect(m_config, &KleverConfig::viewCodeColorChanged, this, &StyleHandler::changeStyle);
    connect(m_config, &KleverConfig::viewHighlightColorChanged, this, &StyleHandler::changeStyle);
    connect(m_config, &KleverConfig::codeFontChanged, this, &StyleHandler::changeStyle);
}

// Setter/getter
bool StyleHandler::printBackground() const
{
    return m_printBackground;
}

void StyleHandler::setPrintBackground(const bool printBackground)
{
    if (m_printBackground != printBackground) {
        m_printBackground = printBackground;
        makeCss();
    }
}

bool StyleHandler::inMain() const
{
    return m_inMain;
}

void StyleHandler::setInMain(const bool inMain)
{
    if (m_inMain != inMain) {
        m_inMain = inMain;
    }
}

void StyleHandler::setDefault(const QStringList &defaultStyle)
{
    if (m_defaultStyle != defaultStyle && m_inMain) {
        m_defaultStyle = defaultStyle;
        if (!m_defaultSet) {
            m_defaultSet = true;
        }
        changeStyle();
    }
}

// Utility
void StyleHandler::loadStyle()
{
    m_style = DocumentHandler::getCssStyle(KleverConfig::stylePath());
    changeStyle();
}

void StyleHandler::changeStyles(const QStringList &_styleInfo)
{
    if (!m_defaultSet) {
        return;
    }
    const QFontInfo viewFontInfo(KleverConfig::viewFont());
    const QString viewFontFamily = viewFontInfo.family();
    const QString viewFontSize = QString::number(viewFontInfo.pointSize());

    const QFontInfo codeFontInfo(KleverConfig::codeFont());
    const QString codeFontFamily = codeFontInfo.family();
    const QString codeFontSize = QString::number(codeFontInfo.pointSize());

    m_styleInfo = {
        viewFontFamily,
        viewFontSize,
        codeFontFamily,
        codeFontSize,
    };

    if (_styleInfo.isEmpty()) {
        m_config->save();

        static const QString noneStr = QStringLiteral("None");
        const QString bodyColor = KleverConfig::viewBodyColor();
        const QString textColor = KleverConfig::viewTextColor();
        const QString titleColor = KleverConfig::viewTitleColor();
        const QString linkColor = KleverConfig::viewLinkColor();
        const QString visitedLinkColor = KleverConfig::viewVisitedLinkColor();
        const QString codeColor = KleverConfig::viewCodeColor();
        const QString highlightColor = KleverConfig::viewHighlightColor();

        // For css
        QStringList originalStyleInfo = {
            bodyColor == noneStr ? m_defaultStyle[0] : bodyColor,
            textColor == noneStr ? m_defaultStyle[1] : textColor,
            titleColor == noneStr ? m_defaultStyle[2] : titleColor,
            linkColor == noneStr ? m_defaultStyle[3] : linkColor,
            visitedLinkColor == noneStr ? m_defaultStyle[4] : visitedLinkColor,
            codeColor == noneStr ? m_defaultStyle[5] : codeColor,
            highlightColor == noneStr ? m_defaultStyle[6] : highlightColor,
        };
        m_styleInfo << originalStyleInfo;

        // For editorHighlighter
        originalStyleInfo[0] = m_defaultStyle[0];
        const QColor background = QColor::fromString(m_defaultStyle[0]);
        const QString special = (background.value() < 128 ? background.lighter(300) : background.darker(200)).name();
        originalStyleInfo.append(special);

        Q_EMIT styleChanged(originalStyleInfo);
    } else {
        m_styleInfo << _styleInfo;
    }

    makeCss();
}

void StyleHandler::makeCss()
{
    if (m_styleInfo.isEmpty()) {
        return;
    }
    QString css = m_style;
    static const QStringList keys = {
        QStringLiteral("--font"),
        QStringLiteral("--fontSize"),
        QStringLiteral("--codeFont"),
        QStringLiteral("--codeFontSize"),
        QStringLiteral("--bodyColor"),
        QStringLiteral("--textColor"),
        QStringLiteral("--titleColor"),
        QStringLiteral("--linkColor"),
        QStringLiteral("--visitedLinkColor"),
        QStringLiteral("--codeColor"),
        QStringLiteral("--highlightColor"),
    };

    for (int i = 0; i < m_styleInfo.length(); ++i) {
        const QString &key = keys[i];
        const QString value = (key == keys[4] && !m_printBackground) ? QStringLiteral("undefined") : m_styleInfo[i];

        const int keyIdx = css.indexOf(key);
        if (keyIdx < 0) {
            continue;
        }

        static const QChar semicolon = QChar::fromLatin1(';');
        const int endIdx = css.indexOf(semicolon, keyIdx);
        if (endIdx < 0) {
            return;
        }
        const QString px = (i == 1 || i == 3) ? QStringLiteral("px") : QLatin1String();
        const int len = endIdx - keyIdx;
        const QString final = key + QStringLiteral(": ") + value + px;
        css.replace(keyIdx, len, final);
    }
    Q_EMIT newCss(css);
}

// SLOTS
void StyleHandler::changeStyle()
{
    if (m_inMain) {
        changeStyles();
    }
}
