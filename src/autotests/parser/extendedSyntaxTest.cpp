/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "logic/parser/extendedSyntax/extendedSyntaxMaker.hpp"
#include "logic/parser/md4qtDataCleaner.hpp"
#include "logic/parser/md4qtDataGetter.hpp"

// Qt include
#include <QObject>
#include <QTextStream>
#include <QtTest/QTest>
#include <vector>

#define MD4QT_QT_SUPPORT
#include "logic/parser/md4qt/doc.hpp"
#include "logic/parser/md4qt/parser.hpp"
#include "logic/parser/md4qt/traits.hpp"

class ExtendedSyntaxTest : public QObject
{
    Q_OBJECT

private:
    void makeTexts();
    void addExtendedSyntaxs();

private Q_SLOTS:
    void initTestCase();

    void simpleHighlight();
    void originalAndNew();
    void cancellingPrevious();
    void newAndOriginal();
    void withNonText();
    void withNonTextAndOriginal();
    void withNonTextAndCancelling();
    void untouched1();
    void untouched2();
    void untouched3();
    void untouched4();
    void unaffected1();
    void unaffected2();
    void newStyleMix();
    void multiLineMix();
    void cancellingPart();
    void cancellingPartInTitle();
    void checkOpenCloseStylesParity();
    void blankLineText();
    void continuousText();

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
        QStringLiteral("^Unaffected\n^"),
        QStringLiteral("New style mix-===-==-==-=="),
        QStringLiteral("Multi__*line*__ ==*mix*== --of--\n^new^ --====and==-- original"),
        QStringLiteral("==*Cancelling part of== **original style***"),
        QStringLiteral("# ==*Cancelling part of== **original style in title***"),
        QStringLiteral("==*With `non text in the middle` and cancelling==*text==Untouched "
                       "`code`\ntextUntouched==text==Untouched\\==text\\==Untouched==text*==Unaffected*==text^Unaffected^text New style "
                       "mix-===-==-==-== Multi__*line*__ ==*mix*== --of--text^new^ --====and==-- original"),
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
    m_md4qtParser.addTextPlugin(1024, md4qtDataCleaner::dataCleaningFunc, false, {});
    addExtendedSyntaxs();
    makeTexts();
}

/* TEST */

/*
==Simple highlight==
*/
void ExtendedSyntaxTest::simpleHighlight()
{
    QTextStream s(&m_testingLines[0], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("simpleHighlight: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("simpleHighlight: Incorrect items count in the paragraph");
    }

    const auto item = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));

    // Check delims
    const auto &openStyles = item->openStyles();
    QCOMPARE_EQ(openStyles.length(), 1);
    QCOMPARE_EQ(openStyles[0].startColumn(), 0);
    QCOMPARE_EQ(openStyles[0].endColumn(), 1);
    QCOMPARE_EQ(openStyles[0].startLine(), 0);
    QCOMPARE_EQ(openStyles[0].endLine(), 0);
    const auto &closeStyles = item->closeStyles();
    QCOMPARE_EQ(closeStyles.length(), 1);
    QCOMPARE_EQ(closeStyles[0].startColumn(), 18);
    QCOMPARE_EQ(closeStyles[0].endColumn(), 19);
    QCOMPARE_EQ(closeStyles[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles[0].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item->opts(), 8);
    QVERIFY(item->isSpaceBefore());
    QVERIFY(item->isSpaceAfter());
    QCOMPARE(item->text(), QStringLiteral("Simple highlight"));
};

/*
==*Original and new style*==
*/
void ExtendedSyntaxTest::originalAndNew()
{
    QTextStream s(&m_testingLines[1], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("originalAndNew: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("originalAndNew: Incorrect items count in the paragraph");
    }

    const auto item = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));

    // Check delims
    const auto &openStyles = item->openStyles();
    QCOMPARE_EQ(openStyles.length(), 2);
    QCOMPARE_EQ(openStyles[0].startColumn(), 0);
    QCOMPARE_EQ(openStyles[0].endColumn(), 1);
    QCOMPARE_EQ(openStyles[0].startLine(), 0);
    QCOMPARE_EQ(openStyles[0].endLine(), 0);
    QCOMPARE_EQ(openStyles[1].startColumn(), 2);
    QCOMPARE_EQ(openStyles[1].endColumn(), 2);
    QCOMPARE_EQ(openStyles[1].startLine(), 0);
    QCOMPARE_EQ(openStyles[1].endLine(), 0);
    const auto &closeStyles = item->closeStyles();
    QCOMPARE_EQ(closeStyles.length(), 2);
    QCOMPARE_EQ(closeStyles[0].startColumn(), 25);
    QCOMPARE_EQ(closeStyles[0].endColumn(), 25);
    QCOMPARE_EQ(closeStyles[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles[0].endLine(), 0);
    QCOMPARE_EQ(closeStyles[1].startColumn(), 26);
    QCOMPARE_EQ(closeStyles[1].endColumn(), 27);
    QCOMPARE_EQ(closeStyles[1].startLine(), 0);
    QCOMPARE_EQ(closeStyles[1].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item->opts(), 10);
    QVERIFY(item->isSpaceBefore());
    QVERIFY(item->isSpaceAfter());
    QCOMPARE(item->text(), QStringLiteral("Original and new style"));
};

/*
==*Cancelling previous style==*
*/
void ExtendedSyntaxTest::cancellingPrevious()
{
    QTextStream s(&m_testingLines[2], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("cancellingPrevious: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("cancellingPrevious: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    // Check delims
    const auto &openStyles = item1->openStyles();
    QCOMPARE_EQ(openStyles.length(), 1);
    QCOMPARE_EQ(openStyles[0].startColumn(), 0);
    QCOMPARE_EQ(openStyles[0].endColumn(), 1);
    QCOMPARE_EQ(openStyles[0].startLine(), 0);
    QCOMPARE_EQ(openStyles[0].endLine(), 0);
    const auto &closeStyles = item1->closeStyles();
    QCOMPARE_EQ(closeStyles.length(), 1);
    QCOMPARE_EQ(closeStyles[0].startColumn(), 28);
    QCOMPARE_EQ(closeStyles[0].endColumn(), 29);
    QCOMPARE_EQ(closeStyles[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles[0].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item1->opts(), 8);
    QCOMPARE(item1->text(), QStringLiteral("*Cancelling previous style"));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(!item1->isSpaceAfter());

    const auto item2 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 0);
    QCOMPARE_EQ(item2->opts(), 0);
    QCOMPARE(item2->text(), QStringLiteral("*"));
    QVERIFY(!item2->isSpaceBefore());
    QVERIFY(item2->isSpaceAfter());
};

/*
 *==New and original style==*
 */
void ExtendedSyntaxTest::newAndOriginal()
{
    QTextStream s(&m_testingLines[3], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("newAndOriginal: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("newAndOriginal: Incorrect items count in the paragraph");
    }

    const auto item = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    // Check delims
    const auto &openStyles = item->openStyles();
    QCOMPARE_EQ(openStyles.length(), 2);
    QCOMPARE_EQ(openStyles[0].startColumn(), 0);
    QCOMPARE_EQ(openStyles[0].endColumn(), 0);
    QCOMPARE_EQ(openStyles[0].startLine(), 0);
    QCOMPARE_EQ(openStyles[0].endLine(), 0);
    QCOMPARE_EQ(openStyles[1].startColumn(), 1);
    QCOMPARE_EQ(openStyles[1].endColumn(), 2);
    QCOMPARE_EQ(openStyles[1].startLine(), 0);
    QCOMPARE_EQ(openStyles[1].endLine(), 0);
    const auto &closeStyles = item->closeStyles();
    QCOMPARE_EQ(closeStyles.length(), 2);
    QCOMPARE_EQ(closeStyles[0].startColumn(), 25);
    QCOMPARE_EQ(closeStyles[0].endColumn(), 26);
    QCOMPARE_EQ(closeStyles[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles[0].endLine(), 0);
    QCOMPARE_EQ(closeStyles[1].startColumn(), 27);
    QCOMPARE_EQ(closeStyles[1].endColumn(), 27);
    QCOMPARE_EQ(closeStyles[1].startLine(), 0);
    QCOMPARE_EQ(closeStyles[1].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item->opts(), 10);
    QVERIFY(item->isSpaceBefore());
    QVERIFY(item->isSpaceAfter());
    QCOMPARE(item->text(), QStringLiteral("New and original style"));
};

/*
==With `non text in the` middle==
*/
void ExtendedSyntaxTest::withNonText()
{
    QTextStream s(&m_testingLines[4], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("withNonText: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("withNonText: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    // Check delims
    const auto &openStyles = item1->openStyles();
    QCOMPARE_EQ(openStyles.length(), 1);
    QCOMPARE_EQ(openStyles[0].startColumn(), 0);
    QCOMPARE_EQ(openStyles[0].endColumn(), 1);
    QCOMPARE_EQ(openStyles[0].startLine(), 0);
    QCOMPARE_EQ(openStyles[0].endLine(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    // !Check delims
    QCOMPARE_EQ(item1->opts(), 8);
    QCOMPARE(item1->text(), QStringLiteral("With"));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());

    const auto item2 = std::static_pointer_cast<MD::ItemWithOpts<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 0);
    QCOMPARE_EQ(item2->opts(), 8);
    QCOMPARE_EQ(item2->type(), MD::ItemType::Code);

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    // Check delims
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    const auto &closeStyles = item3->closeStyles();
    QCOMPARE_EQ(closeStyles.length(), 1);
    QCOMPARE_EQ(closeStyles[0].startColumn(), 31);
    QCOMPARE_EQ(closeStyles[0].endColumn(), 32);
    QCOMPARE_EQ(closeStyles[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles[0].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item3->opts(), 8);
    QCOMPARE(item3->text(), QStringLiteral("middle"));
    QVERIFY(item3->isSpaceBefore());
    QVERIFY(item3->isSpaceAfter());
};

/*
==*With `non text in the middle` and original style*==
*/
void ExtendedSyntaxTest::withNonTextAndOriginal()
{
    QTextStream s(&m_testingLines[5], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("withNonTextAndOriginal: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("withNonTextAndOriginal: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    // Check delims
    const auto &openStyles = item1->openStyles();
    QCOMPARE_EQ(openStyles.length(), 2);
    QCOMPARE_EQ(openStyles[0].startColumn(), 0);
    QCOMPARE_EQ(openStyles[0].endColumn(), 1);
    QCOMPARE_EQ(openStyles[0].startLine(), 0);
    QCOMPARE_EQ(openStyles[0].endLine(), 0);
    QCOMPARE_EQ(openStyles[1].startColumn(), 2);
    QCOMPARE_EQ(openStyles[1].endColumn(), 2);
    QCOMPARE_EQ(openStyles[1].startLine(), 0);
    QCOMPARE_EQ(openStyles[1].endLine(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    // !Check delims
    QCOMPARE_EQ(item1->opts(), 10);
    QCOMPARE(item1->text(), QStringLiteral("With"));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());

    const auto item2 = std::static_pointer_cast<MD::ItemWithOpts<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 0);
    QCOMPARE_EQ(item2->opts(), 10);
    QCOMPARE_EQ(item2->type(), MD::ItemType::Code);

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    // Check delims
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    const auto &closeStyles = item3->closeStyles();
    QCOMPARE_EQ(closeStyles.length(), 2);
    QCOMPARE_EQ(closeStyles[0].startColumn(), 51);
    QCOMPARE_EQ(closeStyles[0].endColumn(), 51);
    QCOMPARE_EQ(closeStyles[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles[0].endLine(), 0);
    QCOMPARE_EQ(closeStyles[1].startColumn(), 52);
    QCOMPARE_EQ(closeStyles[1].endColumn(), 53);
    QCOMPARE_EQ(closeStyles[1].startLine(), 0);
    QCOMPARE_EQ(closeStyles[1].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item3->opts(), 10);
    QCOMPARE(item3->text(), QStringLiteral("and original style"));
    QVERIFY(item3->isSpaceBefore());
    QVERIFY(item3->isSpaceAfter());
};

/*
==*With `non text in the middle` and cancelling==*
*/
void ExtendedSyntaxTest::withNonTextAndCancelling()
{
    QTextStream s(&m_testingLines[6], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("withNonTextAndCancelling: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 4) {
        QFAIL("withNonTextAndCancelling: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    // Check delims
    const auto &openStyles = item1->openStyles();
    QCOMPARE_EQ(openStyles.length(), 1);
    QCOMPARE_EQ(openStyles[0].startColumn(), 0);
    QCOMPARE_EQ(openStyles[0].endColumn(), 1);
    QCOMPARE_EQ(openStyles[0].startLine(), 0);
    QCOMPARE_EQ(openStyles[0].endLine(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    // !Check delims
    QCOMPARE_EQ(item1->opts(), 8);
    QCOMPARE(item1->text(), QStringLiteral("*With"));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());

    const auto item2 = std::static_pointer_cast<MD::ItemWithOpts<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 0);
    QCOMPARE_EQ(item2->opts(), 8);
    QCOMPARE_EQ(item2->type(), MD::ItemType::Code);

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    // Check delims
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    const auto &closeStyles = item3->closeStyles();
    QCOMPARE_EQ(closeStyles.length(), 1);
    QCOMPARE_EQ(closeStyles[0].startColumn(), 47);
    QCOMPARE_EQ(closeStyles[0].endColumn(), 48);
    QCOMPARE_EQ(closeStyles[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles[0].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item3->opts(), 8);
    QCOMPARE(item3->text(), QStringLiteral("and cancelling"));
    QVERIFY(item3->isSpaceBefore());
    QVERIFY(!item3->isSpaceAfter());

    const auto item4 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(3));
    QCOMPARE_EQ(item4->openStyles().length(), 0);
    QCOMPARE_EQ(item4->closeStyles().length(), 0);
    QCOMPARE_EQ(item4->opts(), 0);
    QCOMPARE(item4->text(), QStringLiteral("*"));
    QVERIFY(!item4->isSpaceBefore());
    QVERIFY(item4->isSpaceAfter());
};

/*
==Untouched `code`
*/
void ExtendedSyntaxTest::untouched1()
{
    QTextStream s(&m_testingLines[7], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("untouched1: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("untouched1: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QCOMPARE(item1->text(), QStringLiteral("==Untouched"));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
};

/*
Untouched==
*/
void ExtendedSyntaxTest::untouched2()
{
    QTextStream s(&m_testingLines[8], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("untouched2: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("untouched2: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Untouched=="));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
};

/*
==Untouched\==
*/
void ExtendedSyntaxTest::untouched3()
{
    QTextStream s(&m_testingLines[9], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("untouched3: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("untouched3: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QCOMPARE(item1->text(), QStringLiteral("==Untouched=="));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
};

/*
\==Untouched==
*/
void ExtendedSyntaxTest::untouched4()
{
    QTextStream s(&m_testingLines[10], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("untouched4: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("untouched4: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QCOMPARE(item1->text(), QStringLiteral("==Untouched=="));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
};

/*
 *==Unaffected*==
 */
void ExtendedSyntaxTest::unaffected1()
{
    QTextStream s(&m_testingLines[11], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("unaffected1: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("unaffected1: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 1);
    QCOMPARE_EQ(item1->closeStyles().length(), 1);
    // Check delims
    const auto &openStyles = item1->openStyles();
    QCOMPARE_EQ(openStyles.length(), 1);
    QCOMPARE_EQ(openStyles[0].startColumn(), 0);
    QCOMPARE_EQ(openStyles[0].endColumn(), 0);
    QCOMPARE_EQ(openStyles[0].startLine(), 0);
    QCOMPARE_EQ(openStyles[0].endLine(), 0);
    const auto &closeStyles = item1->closeStyles();
    QCOMPARE_EQ(closeStyles.length(), 1);
    QCOMPARE_EQ(closeStyles[0].startColumn(), 13);
    QCOMPARE_EQ(closeStyles[0].endColumn(), 13);
    QCOMPARE_EQ(closeStyles[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles[0].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item1->opts(), 2);
    QCOMPARE(item1->text(), QStringLiteral("==Unaffected"));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(!item1->isSpaceAfter());

    const auto item2 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 0);
    QCOMPARE_EQ(item2->opts(), 0);
    QCOMPARE(item2->text(), QStringLiteral("=="));
    QVERIFY(!item2->isSpaceBefore());
    QVERIFY(item2->isSpaceAfter());
};

/*
^Unaffected
^
*/
void ExtendedSyntaxTest::unaffected2()
{
    QTextStream s(&m_testingLines[12], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("unaffected2: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("unaffected2: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QCOMPARE(item1->text(), QStringLiteral("^Unaffected"));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());

    const auto item2 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 0);
    QCOMPARE_EQ(item2->opts(), 0);
    QCOMPARE(item2->text(), QStringLiteral("^"));
    QVERIFY(item2->isSpaceBefore());
    QVERIFY(item2->isSpaceAfter());
};

/*
New style mix-===-==-==-==
*/
void ExtendedSyntaxTest::newStyleMix()
{
    QTextStream s(&m_testingLines[13], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("newStyleMix: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 5) {
        QFAIL("newStyleMix: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QCOMPARE(item1->text(), QStringLiteral("New style mix"));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(!item1->isSpaceAfter());

    const auto item2 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(1));
    // Check delims
    const auto &openStyles2 = item2->openStyles();
    QCOMPARE_EQ(openStyles2.length(), 1);
    QCOMPARE_EQ(openStyles2[0].startColumn(), 13);
    QCOMPARE_EQ(openStyles2[0].endColumn(), 13);
    QCOMPARE_EQ(openStyles2[0].startLine(), 0);
    QCOMPARE_EQ(openStyles2[0].endLine(), 0);
    const auto &closeStyles2 = item2->closeStyles();
    QCOMPARE_EQ(closeStyles2.length(), 1);
    QCOMPARE_EQ(closeStyles2[0].startColumn(), 17);
    QCOMPARE_EQ(closeStyles2[0].endColumn(), 17);
    QCOMPARE_EQ(closeStyles2[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles2[0].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item2->opts(), 16);
    QCOMPARE(item2->text(), QStringLiteral("==="));
    QVERIFY(!item2->isSpaceBefore());
    QVERIFY(!item2->isSpaceAfter());

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 0);
    QCOMPARE_EQ(item3->opts(), 0);
    QCOMPARE(item3->text(), QStringLiteral("=="));
    QVERIFY(!item3->isSpaceBefore());
    QVERIFY(!item3->isSpaceAfter());

    const auto item4 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(3));
    // Check delims
    const auto &openStyles4 = item4->openStyles();
    QCOMPARE_EQ(openStyles4.length(), 1);
    QCOMPARE_EQ(openStyles4[0].startColumn(), 20);
    QCOMPARE_EQ(openStyles4[0].endColumn(), 20);
    QCOMPARE_EQ(openStyles4[0].startLine(), 0);
    QCOMPARE_EQ(openStyles4[0].endLine(), 0);
    const auto &closeStyles4 = item4->closeStyles();
    QCOMPARE_EQ(closeStyles4.length(), 1);
    QCOMPARE_EQ(closeStyles4[0].startColumn(), 23);
    QCOMPARE_EQ(closeStyles4[0].endColumn(), 23);
    QCOMPARE_EQ(closeStyles4[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles4[0].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item4->opts(), 16);
    QCOMPARE(item4->text(), QStringLiteral("=="));
    QVERIFY(!item4->isSpaceBefore());
    QVERIFY(!item4->isSpaceAfter());

    const auto item5 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(4));
    QCOMPARE_EQ(item5->openStyles().length(), 0);
    QCOMPARE_EQ(item5->closeStyles().length(), 0);
    QCOMPARE_EQ(item5->opts(), 0);
    QCOMPARE(item5->text(), QStringLiteral("=="));
    QVERIFY(!item5->isSpaceBefore());
    QVERIFY(item5->isSpaceAfter());
};

/*
Multi__*line*__ ==*mix*== --of--
^new^ --====and==-- original
*/
void ExtendedSyntaxTest::multiLineMix()
{
    QTextStream s(&m_testingLines[14], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("multiLineMix: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 9) {
        QFAIL("multiLineMix: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Multi__"));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(!item1->isSpaceAfter());

    const auto item2 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(1));
    // Check delims
    const auto &openStyles2 = item2->openStyles();
    QCOMPARE_EQ(openStyles2.length(), 1);
    QCOMPARE_EQ(openStyles2[0].startColumn(), 7);
    QCOMPARE_EQ(openStyles2[0].endColumn(), 7);
    QCOMPARE_EQ(openStyles2[0].startLine(), 0);
    QCOMPARE_EQ(openStyles2[0].endLine(), 0);
    const auto &closeStyles2 = item2->closeStyles();
    QCOMPARE_EQ(closeStyles2.length(), 1);
    QCOMPARE_EQ(closeStyles2[0].startColumn(), 12);
    QCOMPARE_EQ(closeStyles2[0].endColumn(), 12);
    QCOMPARE_EQ(closeStyles2[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles2[0].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item2->opts(), 2);
    QCOMPARE(item2->text(), QStringLiteral("line"));
    QVERIFY(!item2->isSpaceBefore());
    QVERIFY(!item2->isSpaceAfter());

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 0);
    QCOMPARE_EQ(item3->opts(), 0);
    QCOMPARE(item3->text(), QStringLiteral("__"));
    QVERIFY(!item3->isSpaceBefore());
    QVERIFY(item3->isSpaceAfter());

    const auto item4 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(3));
    // Check delims
    const auto &openStyles4 = item4->openStyles();
    QCOMPARE_EQ(openStyles4.length(), 2);
    QCOMPARE_EQ(openStyles4[0].startColumn(), 16);
    QCOMPARE_EQ(openStyles4[0].endColumn(), 17);
    QCOMPARE_EQ(openStyles4[0].startLine(), 0);
    QCOMPARE_EQ(openStyles4[0].endLine(), 0);
    QCOMPARE_EQ(openStyles4[1].startColumn(), 18);
    QCOMPARE_EQ(openStyles4[1].endColumn(), 18);
    QCOMPARE_EQ(openStyles4[1].startLine(), 0);
    QCOMPARE_EQ(openStyles4[1].endLine(), 0);
    const auto &closeStyles4 = item4->closeStyles();
    QCOMPARE_EQ(closeStyles4.length(), 2);
    QCOMPARE_EQ(closeStyles4[0].startColumn(), 22);
    QCOMPARE_EQ(closeStyles4[0].endColumn(), 22);
    QCOMPARE_EQ(closeStyles4[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles4[0].endLine(), 0);
    QCOMPARE_EQ(closeStyles4[1].startColumn(), 23);
    QCOMPARE_EQ(closeStyles4[1].endColumn(), 24);
    QCOMPARE_EQ(closeStyles4[1].startLine(), 0);
    QCOMPARE_EQ(closeStyles4[1].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item4->opts(), 10);
    QCOMPARE(item4->text(), QStringLiteral("mix"));
    QVERIFY(item4->isSpaceBefore());
    QVERIFY(item4->isSpaceAfter());

    const auto item5 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(4));
    // Check delims
    const auto &openStyles5 = item5->openStyles();
    QCOMPARE_EQ(openStyles5.length(), 2);
    QCOMPARE_EQ(openStyles5[0].startColumn(), 26);
    QCOMPARE_EQ(openStyles5[0].endColumn(), 26);
    QCOMPARE_EQ(openStyles5[0].startLine(), 0);
    QCOMPARE_EQ(openStyles5[0].endLine(), 0);
    QCOMPARE_EQ(openStyles5[1].startColumn(), 27);
    QCOMPARE_EQ(openStyles5[1].endColumn(), 27);
    QCOMPARE_EQ(openStyles5[1].startLine(), 0);
    QCOMPARE_EQ(openStyles5[1].endLine(), 0);
    const auto &closeStyles5 = item5->closeStyles();
    QCOMPARE_EQ(closeStyles5.length(), 2);
    QCOMPARE_EQ(closeStyles5[0].startColumn(), 30);
    QCOMPARE_EQ(closeStyles5[0].endColumn(), 30);
    QCOMPARE_EQ(closeStyles5[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles5[0].endLine(), 0);
    QCOMPARE_EQ(closeStyles5[1].startColumn(), 31);
    QCOMPARE_EQ(closeStyles5[1].endColumn(), 31);
    QCOMPARE_EQ(closeStyles5[1].startLine(), 0);
    QCOMPARE_EQ(closeStyles5[1].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item5->opts(), 16);
    QCOMPARE(item5->text(), QStringLiteral("of"));
    QVERIFY(item5->isSpaceBefore());
    QVERIFY(item5->isSpaceAfter());

    const auto item6 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(5));
    // Check delims
    const auto &openStyles6 = item6->openStyles();
    QCOMPARE_EQ(openStyles6.length(), 1);
    QCOMPARE_EQ(openStyles6[0].startColumn(), 0);
    QCOMPARE_EQ(openStyles6[0].endColumn(), 0);
    QCOMPARE_EQ(openStyles6[0].startLine(), 1);
    QCOMPARE_EQ(openStyles6[0].endLine(), 1);
    const auto &closeStyles6 = item6->closeStyles();
    QCOMPARE_EQ(closeStyles6.length(), 1);
    QCOMPARE_EQ(closeStyles6[0].startColumn(), 4);
    QCOMPARE_EQ(closeStyles6[0].endColumn(), 4);
    QCOMPARE_EQ(closeStyles6[0].startLine(), 1);
    QCOMPARE_EQ(closeStyles6[0].endLine(), 1);
    // !Check delims
    QCOMPARE_EQ(item6->opts(), 32);
    QCOMPARE(item6->text(), QStringLiteral("new"));
    QVERIFY(item6->isSpaceBefore());
    QVERIFY(item6->isSpaceAfter());

    const auto item7 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(6));
    // Check delims
    const auto &openStyles7 = item7->openStyles();
    QCOMPARE_EQ(openStyles7.length(), 2);
    QCOMPARE_EQ(openStyles7[0].startColumn(), 6);
    QCOMPARE_EQ(openStyles7[0].endColumn(), 6);
    QCOMPARE_EQ(openStyles7[0].startLine(), 1);
    QCOMPARE_EQ(openStyles7[0].endLine(), 1);
    QCOMPARE_EQ(openStyles7[1].startColumn(), 7);
    QCOMPARE_EQ(openStyles7[1].endColumn(), 7);
    QCOMPARE_EQ(openStyles7[1].startLine(), 1);
    QCOMPARE_EQ(openStyles7[1].endLine(), 1);
    QCOMPARE_EQ(item7->closeStyles().length(), 0);
    // !Check delims
    QCOMPARE_EQ(item7->opts(), 16);
    QCOMPARE(item7->text(), QStringLiteral("=="));
    QVERIFY(item7->isSpaceBefore());
    QVERIFY(!item7->isSpaceAfter());

    const auto item8 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(7));
    // Check delims
    const auto &openStyles8 = item8->openStyles();
    QCOMPARE_EQ(openStyles8.length(), 1);
    QCOMPARE_EQ(openStyles8[0].startColumn(), 10);
    QCOMPARE_EQ(openStyles8[0].endColumn(), 11);
    QCOMPARE_EQ(openStyles8[0].startLine(), 1);
    QCOMPARE_EQ(openStyles8[0].endLine(), 1);
    const auto &closeStyles8 = item8->closeStyles();
    QCOMPARE_EQ(closeStyles8.length(), 3);
    QCOMPARE_EQ(closeStyles8[0].startColumn(), 15);
    QCOMPARE_EQ(closeStyles8[0].endColumn(), 16);
    QCOMPARE_EQ(closeStyles8[0].startLine(), 1);
    QCOMPARE_EQ(closeStyles8[0].endLine(), 1);
    QCOMPARE_EQ(closeStyles8[1].startColumn(), 17);
    QCOMPARE_EQ(closeStyles8[1].endColumn(), 17);
    QCOMPARE_EQ(closeStyles8[1].startLine(), 1);
    QCOMPARE_EQ(closeStyles8[1].endLine(), 1);
    QCOMPARE_EQ(closeStyles8[2].startColumn(), 18);
    QCOMPARE_EQ(closeStyles8[2].endColumn(), 18);
    QCOMPARE_EQ(closeStyles8[2].startLine(), 1);
    QCOMPARE_EQ(closeStyles8[2].endLine(), 1);
    // !Check delims
    QCOMPARE_EQ(item8->opts(), 24);
    QCOMPARE(item8->text(), QStringLiteral("and"));
    QVERIFY(!item8->isSpaceBefore());
    QVERIFY(item8->isSpaceAfter());

    const auto item9 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(8));
    QCOMPARE_EQ(item9->openStyles().length(), 0);
    QCOMPARE_EQ(item9->closeStyles().length(), 0);
    QCOMPARE_EQ(item9->opts(), 0);
    QCOMPARE(item9->text(), QStringLiteral("original"));
    QVERIFY(item9->isSpaceBefore());
    QVERIFY(item9->isSpaceAfter());
};

/*
==*Cancelling part of== **original style***
*/
void ExtendedSyntaxTest::cancellingPart()
{
    QTextStream s(&m_testingLines[15], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("cancellingPart: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("cancellingPart: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    // Check delims
    const auto &openStyles1 = item1->openStyles();
    QCOMPARE_EQ(openStyles1.length(), 1);
    QCOMPARE_EQ(openStyles1[0].startColumn(), 0);
    QCOMPARE_EQ(openStyles1[0].endColumn(), 1);
    QCOMPARE_EQ(openStyles1[0].startLine(), 0);
    QCOMPARE_EQ(openStyles1[0].endLine(), 0);
    const auto &closeStyles1 = item1->closeStyles();
    QCOMPARE_EQ(closeStyles1.length(), 1);
    QCOMPARE_EQ(closeStyles1[0].startColumn(), 21);
    QCOMPARE_EQ(closeStyles1[0].endColumn(), 22);
    QCOMPARE_EQ(closeStyles1[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles1[0].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item1->opts(), 8);
    QCOMPARE(item1->text(), QStringLiteral("*Cancelling part of"));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());

    const auto item2 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(1));
    // Check delims
    const auto &openStyles2 = item2->openStyles();
    QCOMPARE_EQ(openStyles2.length(), 1);
    QCOMPARE_EQ(openStyles2[0].startColumn(), 24);
    QCOMPARE_EQ(openStyles2[0].endColumn(), 25);
    QCOMPARE_EQ(openStyles2[0].startLine(), 0);
    QCOMPARE_EQ(openStyles2[0].endLine(), 0);
    const auto &closeStyles2 = item2->closeStyles();
    QCOMPARE_EQ(closeStyles2.length(), 1);
    QCOMPARE_EQ(closeStyles2[0].startColumn(), 40);
    QCOMPARE_EQ(closeStyles2[0].endColumn(), 41);
    QCOMPARE_EQ(closeStyles2[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles2[0].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item2->opts(), 1);
    QCOMPARE(item2->text(), QStringLiteral("original style"));
    QVERIFY(item2->isSpaceBefore());
    QVERIFY(!item2->isSpaceAfter());

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 0);
    QCOMPARE_EQ(item3->opts(), 0);
    QCOMPARE(item3->text(), QStringLiteral("*"));
    QVERIFY(!item3->isSpaceBefore());
    QVERIFY(item3->isSpaceAfter());
};

/*
# ==*Cancelling part of== **original style in title***
*/
void ExtendedSyntaxTest::cancellingPartInTitle()
{
    QTextStream s(&m_testingLines[16], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("cancellingPart: Incorrect items count in the doc");
    }

    const auto heading = static_cast<MD::Heading<MD::QStringTrait> *>(doc->items().at(1).get());
    const auto paragraph = heading->text();
    if (paragraph->items().length() != 3) {
        QFAIL("cancellingPart: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    // Check delims
    const auto &openStyles1 = item1->openStyles();
    QCOMPARE_EQ(openStyles1.length(), 1);
    QCOMPARE_EQ(openStyles1[0].startColumn(), 2);
    QCOMPARE_EQ(openStyles1[0].endColumn(), 3);
    QCOMPARE_EQ(openStyles1[0].startLine(), 0);
    QCOMPARE_EQ(openStyles1[0].endLine(), 0);
    const auto &closeStyles1 = item1->closeStyles();
    QCOMPARE_EQ(closeStyles1.length(), 1);
    QCOMPARE_EQ(closeStyles1[0].startColumn(), 23);
    QCOMPARE_EQ(closeStyles1[0].endColumn(), 24);
    QCOMPARE_EQ(closeStyles1[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles1[0].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item1->opts(), 8);
    QCOMPARE(item1->text(), QStringLiteral("*Cancelling part of"));
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());

    const auto item2 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(1));
    // Check delims
    const auto &openStyles2 = item2->openStyles();
    QCOMPARE_EQ(openStyles2.length(), 1);
    QCOMPARE_EQ(openStyles2[0].startColumn(), 26);
    QCOMPARE_EQ(openStyles2[0].endColumn(), 27);
    QCOMPARE_EQ(openStyles2[0].startLine(), 0);
    QCOMPARE_EQ(openStyles2[0].endLine(), 0);
    const auto &closeStyles2 = item2->closeStyles();
    QCOMPARE_EQ(closeStyles2.length(), 1);
    QCOMPARE_EQ(closeStyles2[0].startColumn(), 51);
    QCOMPARE_EQ(closeStyles2[0].endColumn(), 52);
    QCOMPARE_EQ(closeStyles2[0].startLine(), 0);
    QCOMPARE_EQ(closeStyles2[0].endLine(), 0);
    // !Check delims
    QCOMPARE_EQ(item2->opts(), 1);
    QCOMPARE(item2->text(), QStringLiteral("original style in title"));
    QVERIFY(item2->isSpaceBefore());
    QVERIFY(!item2->isSpaceAfter());

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 0);
    QCOMPARE_EQ(item3->opts(), 0);
    QCOMPARE(item3->text(), QStringLiteral("*"));
    QVERIFY(!item3->isSpaceBefore());
    QVERIFY(item3->isSpaceAfter());
};

/*
==*With `non text in the middle` and cancelling==*text==Untouched `code`
textUntouched==text==Untouched\==text\==Untouched==text*==Unaffected*==text^Unaffected^text New style mix-===-==-==-== Multi__*line*__ ==*mix*== --of--text^new^
--====and==-- original
*/
void ExtendedSyntaxTest::checkOpenCloseStylesParity()
{
    QTextStream s(&m_testingLines[17], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("cancellingPart: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 27) {
        QFAIL("cancellingPart: Incorrect items count in the paragraph");
    }

    QList<MD::StyleDelim> openStyles;
    QList<MD::StyleDelim> closeStyles;
    for (const auto &item : paragraph->items()) {
        const auto itemWithOpts = md4qtHelperFunc::getSharedItemWithOpts(item);

        openStyles << itemWithOpts->openStyles();
        closeStyles << itemWithOpts->closeStyles();
    }

    QCOMPARE_EQ(openStyles.length(), closeStyles.length());
}

// If those don't crash that already a good things
void ExtendedSyntaxTest::blankLineText()
{
    QTextStream s(&m_blankLineText, QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
};

void ExtendedSyntaxTest::continuousText()
{
    QTextStream s(&m_continuousText, QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
};

QTEST_MAIN(ExtendedSyntaxTest)
#include "extendedSyntaxTest.moc"
