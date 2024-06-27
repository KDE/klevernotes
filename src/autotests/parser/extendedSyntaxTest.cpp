/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "../../logic/extendedSyntax/extendedSyntaxMaker.hpp"

// Qt include
#include <QObject>
#include <QtTest/QTest>
#include <qlogging.h>

#define MD4QT_QT_SUPPORT
#include "../../logic/md4qt/parser.hpp"

class ExtendedSyntaxTest : public QObject
{
    Q_OBJECT

private:
    void makeTexts();
    void addExtendedSyntaxs();

private Q_SLOTS:
    void initTestCase();

    void test();

private:
    // md4qt
    MD::Parser<MD::QStringTrait> m_md4qtParser;
    int m_extendedSyntaxCount = 0;
    const int m_id = MD::TextPlugin::UserDefinedPluginID + 1;

    // Data
    QStringList m_testingLines = {
        QStringLiteral("==Simple highlight=="),
        QStringLiteral("==*Original and new style*=="),
        QStringLiteral("==*Cancelling previous style==*"),
        QStringLiteral("*==New and original style==*"),
        QStringLiteral("==With `non text in the` middle=="),
        QStringLiteral("==*With `non text in the middle` and original style*=="),
        QStringLiteral("==*With `non text in the middle` and cancelling==*"),
        QStringLiteral("==Untouched `code`"),
        QStringLiteral("Untouched=="),
        QStringLiteral("==Untouched\\=="),
        QStringLiteral("\\==Untouched=="),
        QStringLiteral("*==Unaffected*=="),
        QStringLiteral("New style mix-===-==-==-=="),
        QStringLiteral("Multi__*line*__ ==*mix*== --of--\n^new^ --====and==-- original"),
        QStringLiteral("==*Cancelling part of== **original style***"),
    };
    QString m_blankLineText;
    QString m_continuousText;
};

/* Settings Data */
void ExtendedSyntaxTest::makeTexts()
{
    static const QChar newLine = QChar::fromLatin1('\n');
    m_continuousText = m_testingLines.join(newLine);
    m_blankLineText = m_testingLines.join(newLine + newLine);
}

void ExtendedSyntaxTest::addExtendedSyntaxs()
{
    static const QList<QStringList> extendedSyntaxsList = {
        {QStringLiteral("=="), QStringLiteral("<mark>"), QStringLiteral("</mark>")}, // Highlight
        {QStringLiteral("-"), QStringLiteral("<sub>"), QStringLiteral("</sub>")}, // Subscript
        {QStringLiteral("^"), QStringLiteral("<sup>"), QStringLiteral("</sup>")}, // Superscript
    };

    for (const auto &details : extendedSyntaxsList) {
        const long long int opts = MD::TextOption::StrikethroughText << (m_extendedSyntaxCount + 1);

        const QStringList options = {details[0], QString::number(opts)};
        m_md4qtParser.addTextPlugin(m_id + m_extendedSyntaxCount, ExtendedSyntaxMaker::extendedSyntaxHelperFunc, true, options);
        ++m_extendedSyntaxCount;
    }
}

void ExtendedSyntaxTest::initTestCase()
{
    addExtendedSyntaxs();
    makeTexts();
}

/* TEST */
void ExtendedSyntaxTest::test()
{
    qWarning() << m_testingLines << "\n";
    qWarning() << m_blankLineText << "\n";
    qWarning() << m_continuousText << "\n";
}

QTEST_MAIN(ExtendedSyntaxTest)
#include "extendedSyntaxTest.moc"
