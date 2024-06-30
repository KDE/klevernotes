/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "../../logic/extendedSyntax/extendedSyntaxMaker.hpp"
#include "logic/md4qt/doc.hpp"
#include "logic/md4qt/traits.hpp"

// Qt include
#include <QObject>
#include <QTextStream>
#include <QtTest/QTest>

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
    void blankLineText();
    // void continuousText();

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

/*
==Simple highlight==
*/
void ExtendedSyntaxTest::simpleHighlight()
{
    QTextStream s(&m_testingLines[0], QIODeviceBase::ReadOnly);
    const std::shared_ptr<MD::Document<MD::QStringTrait>> doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));

    if (doc->items().length() != 2) {
        QFAIL("simpleHighlight: Incorrect items count in the doc");
    }

    MD::Paragraph<MD::QStringTrait> *paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    ;
    if (paragraph->items().length() != 1) {
        QFAIL("simpleHighlight: Incorrect items count in the paragraph");
    }

    const auto item = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));

    QCOMPARE_EQ(item->openStyles().length(), 1);
    QCOMPARE_EQ(item->closeStyles().length(), 1);
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

    MD::Paragraph<MD::QStringTrait> *paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    ;
    if (paragraph->items().length() != 1) {
        QFAIL("originalAndNew: Incorrect items count in the paragraph");
    }

    const auto item = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));

    QCOMPARE_EQ(item->openStyles().length(), 2);
    QCOMPARE_EQ(item->closeStyles().length(), 2);
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

    MD::Paragraph<MD::QStringTrait> *paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("cancellingPrevious: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 1);
    QCOMPARE_EQ(item1->closeStyles().length(), 1);
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

    MD::Paragraph<MD::QStringTrait> *paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    ;
    if (paragraph->items().length() != 1) {
        QFAIL("newAndOriginal: Incorrect items count in the paragraph");
    }

    const auto item = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));

    QCOMPARE_EQ(item->openStyles().length(), 2);
    QCOMPARE_EQ(item->closeStyles().length(), 2);
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

    MD::Paragraph<MD::QStringTrait> *paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("withNonText: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 1);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
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
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 1);
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

    MD::Paragraph<MD::QStringTrait> *paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("withNonTextAndOriginal: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 2);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
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
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 2);
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

    MD::Paragraph<MD::QStringTrait> *paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 4) {
        QFAIL("withNonTextAndCancelling: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 1);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
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
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 1);
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

    MD::Paragraph<MD::QStringTrait> *paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
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
        QFAIL("untouched1: Incorrect items count in the doc");
    }

    MD::Paragraph<MD::QStringTrait> *paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("untouched1: Incorrect items count in the paragraph");
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
        QFAIL("untouched1: Incorrect items count in the doc");
    }

    MD::Paragraph<MD::QStringTrait> *paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("untouched1: Incorrect items count in the paragraph");
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
        QFAIL("untouched1: Incorrect items count in the doc");
    }

    MD::Paragraph<MD::QStringTrait> *paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("untouched1: Incorrect items count in the paragraph");
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
        QFAIL("untouched1: Incorrect items count in the doc");
    }

    MD::Paragraph<MD::QStringTrait> *paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("untouched1: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 1);
    QCOMPARE_EQ(item1->closeStyles().length(), 1);
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
};

/*
New style mix-===-==-==-==
*/
void ExtendedSyntaxTest::newStyleMix()
{
    QTextStream s(&m_testingLines[13], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
};

/*
Multi__*line*__ ==*mix*== --of--\n^new^ --====and==-- original
*/
void ExtendedSyntaxTest::multiLineMix()
{
    QTextStream s(&m_testingLines[14], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
};

/*
==*Cancelling part of== **original style***
*/
void ExtendedSyntaxTest::cancellingPart()
{
    QTextStream s(&m_testingLines[15], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
};

void ExtendedSyntaxTest::blankLineText()
{
    QTextStream s(&m_blankLineText, QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));
};

/* WARNING: CRASH ??????? */
/*
void ExtendedSyntaxTest::continuousText()
{
    QTextStream s(&m_continuousText, QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, {}, QStringLiteral("note.md"));

    // QCOMPARE_EQ(doc->items().length(), 17);
};
*/

QTEST_MAIN(ExtendedSyntaxTest)
#include "extendedSyntaxTest.moc"
