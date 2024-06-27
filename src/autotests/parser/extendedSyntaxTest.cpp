/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "../../logic/extendedSyntax/extendedSyntaxMaker.hpp"

// Qt include
#include <QObject>
#include <QtTest/QTest>

#define MD4QT_QT_SUPPORT
#include "../../logic/md4qt/parser.hpp"

class ExtendedSyntaxTest : public QObject
{
    Q_OBJECT

private:
    void readFile();
    void makeContinuousText();
    void getData();
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
    QStringList m_testingLines;
    QString m_blankLineText;
    QString m_continuousText;
};

/* Settings Data */
void ExtendedSyntaxTest::readFile()
{
    QFile file(QStringLiteral("../testdata/extendSyntaxData.md"));
    QStringList lines;

    static const QChar newLine = QChar::fromLatin1('\n');
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        bool previousEmpty = true;
        while (!stream.atEnd()) {
            const QString line = stream.readLine();
            m_blankLineText.append(line + newLine);

            if (!line.isEmpty()) {
                if (previousEmpty) {
                    m_testingLines.append(line);
                } else {
                    m_testingLines.last().append(newLine + line);
                }
                previousEmpty = false;
            } else {
                previousEmpty = true;
            }
        }
    }

    file.close();
}

void ExtendedSyntaxTest::makeContinuousText()
{
    static const QChar newLine = QChar::fromLatin1('\n');
    m_continuousText = m_testingLines.join(newLine);
}

void ExtendedSyntaxTest::getData()
{
    readFile();
    Q_ASSERT_X(m_testingLines.length() == 15, "ExtendedSyntaxTest readFile", "Incorrect test case count");
    makeContinuousText();
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
    getData();
}

/* TEST */

void ExtendedSyntaxTest::test()
{
    qDebug() << m_testingLines;

    qDebug() << m_blankLineText << "\n END BLANK";

    qDebug() << m_testingLines;
}

QTEST_MAIN(ExtendedSyntaxTest)
#include "extendedSyntaxTest.moc"
