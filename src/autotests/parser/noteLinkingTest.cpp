/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "logic/parser/md4qtDataCleaner.hpp"
#include "logic/parser/plugins/noteMapper/noteLinkingPlugin.hpp"

// Qt include
#include <QObject>
#include <QTextStream>
#include <QtTest/QTest>

#define MD4QT_QT_SUPPORT
#include "logic/parser/md4qt/doc.hpp"
#include "logic/parser/md4qt/parser.hpp"
#include "logic/parser/md4qt/traits.hpp"

class NoteLinkingTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void linkBeginningWithOpenClose();
    void linkBeginningWithOpen();
    void linkMiddleWithClose();
    void linkMiddleWithOpen();
    void linkMiddle();
    void linkNoHeader();
    void linkNoSpecificTitle();
    void badUrl();
    void illFormed1();
    void illFormed2();
    void inTitle();

private:
    // md4qt
    MD::Parser<MD::QStringTrait> m_md4qtParser;
    const QString dummyPath = QStringLiteral("/home/dummy/");

    // Data
    QStringList m_testingLines = {
        QStringLiteral("**[[/my/link:header | My link with opening and closing]]* text after with closing*"),
        QStringLiteral("*[[/my/link:header | My link with opening]] text after with closing*"),
        QStringLiteral("**Text before [[/my/link:header | my link with closing]]* text outside with closing*"),
        QStringLiteral("Text before *[[/my/link:header | my link ]] text after with closing*"),
        QStringLiteral("*Text before with opening [[/my/link:header | my link]] and text after with closing* text outside"),
        QStringLiteral("My [[my/Link | link]] with no header in the URL"),
        QStringLiteral("My [[/link]] with no specific text"),
        QStringLiteral("Bad URL [[ | link]]"),
        QStringLiteral("Ill-formed [[URL | link]"),
        QStringLiteral("Ill-formed [URL | link]]"),
        QStringLiteral("# *Text before with opening [[/my/link:header | my link]] and text after with closing* text outside in header"),
    };
};

/* Settings Data */
void NoteLinkingTest::initTestCase()
{
    m_md4qtParser.addTextPlugin(256, NoteLinkingPlugin::noteLinkingHelperFunc, true, {});
    m_md4qtParser.addTextPlugin(1024, md4qtDataCleaner::dataCleaningFunc, false, {});
}

/* TEST */
/*
**[[/my/link:header | My link with opening and closing]]* text after with closing*
*/
void NoteLinkingTest::linkBeginningWithOpenClose()
{
    QTextStream s(&m_testingLines[0], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("linkBeginningWithOpenClose: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("linkBeginningWithOpenClose: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Link<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 2);
    QCOMPARE_EQ(item1->closeStyles().length(), 1);
    QCOMPARE_EQ(item1->opts(), 2);
    QCOMPARE(item1->url(), QStringLiteral("/my/.BaseGroup/link@HEADER@header"));
    QCOMPARE_EQ(item1->urlPos(), MD::WithPosition(4, 0, 18, 0));
    QCOMPARE(item1->text(), QStringLiteral("My link with opening and closing"));
    QCOMPARE_EQ(item1->textPos(), MD::WithPosition(22, 0, 53, 0));

    const auto item2 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 1);
    QCOMPARE_EQ(item2->opts(), 2);
    QVERIFY(item2->isSpaceBefore());
    QVERIFY(item2->isSpaceAfter());
    QCOMPARE(item2->text(), QStringLiteral("text after with closing"));
    QCOMPARE_EQ(item2->startColumn(), 57);
    QCOMPARE_EQ(item2->endColumn(), 80);
}

/*
 *[[/my/link:header | My link with opening]] text after with closing*
 */
void NoteLinkingTest::linkBeginningWithOpen()
{
    QTextStream s(&m_testingLines[1], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("linkBeginningWithOpen: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("linkBeginningWithOpen: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Link<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 1);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 2);
    QCOMPARE(item1->url(), QStringLiteral("/my/.BaseGroup/link@HEADER@header"));
    QCOMPARE_EQ(item1->urlPos(), MD::WithPosition(3, 0, 17, 0));
    QCOMPARE(item1->text(), QStringLiteral("My link with opening"));
    QCOMPARE_EQ(item1->textPos(), MD::WithPosition(21, 0, 40, 0));

    const auto item2 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 1);
    QCOMPARE_EQ(item2->opts(), 2);
    QVERIFY(item2->isSpaceBefore());
    QVERIFY(item2->isSpaceAfter());
    QCOMPARE(item2->text(), QStringLiteral("text after with closing"));
    QCOMPARE_EQ(item2->startColumn(), 43);
    QCOMPARE_EQ(item2->endColumn(), 66);
}

/*
**Text before [[/my/link:header | my link with closing]]* text outside with closing*
*/
void NoteLinkingTest::linkMiddleWithClose()
{
    QTextStream s(&m_testingLines[2], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("linkMiddleWithClose: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("linkMiddleWithClose: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 2);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 2);
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
    QCOMPARE(item1->text(), QStringLiteral("Text before"));
    QCOMPARE_EQ(item1->startColumn(), 2);
    QCOMPARE_EQ(item1->endColumn(), 13);

    const auto item2 = std::static_pointer_cast<MD::Link<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 1);
    QCOMPARE_EQ(item2->opts(), 2);
    QCOMPARE(item2->url(), QStringLiteral("/my/.BaseGroup/link@HEADER@header"));
    QCOMPARE_EQ(item2->urlPos(), MD::WithPosition(16, 0, 30, 0));
    QCOMPARE(item2->text(), QStringLiteral("my link with closing"));
    QCOMPARE_EQ(item2->textPos(), MD::WithPosition(34, 0, 53, 0));

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 1);
    QCOMPARE_EQ(item3->opts(), 2);
    QVERIFY(item3->isSpaceBefore());
    QVERIFY(item3->isSpaceAfter());
    QCOMPARE(item3->text(), QStringLiteral("text outside with closing"));
    QCOMPARE_EQ(item3->startColumn(), 57);
    QCOMPARE_EQ(item3->endColumn(), 82);
}

/*
Text before *[[/my/link:header | my link ]] text after with closing*
*/
void NoteLinkingTest::linkMiddleWithOpen()
{
    QTextStream s(&m_testingLines[3], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("linkMiddleWithOpen: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("linkMiddleWithOpen: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
    QCOMPARE(item1->text(), QStringLiteral("Text before"));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 11);

    const auto item2 = std::static_pointer_cast<MD::Link<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 1);
    QCOMPARE_EQ(item2->closeStyles().length(), 0);
    QCOMPARE_EQ(item2->opts(), 2);
    QCOMPARE(item2->url(), QStringLiteral("/my/.BaseGroup/link@HEADER@header"));
    QCOMPARE_EQ(item2->urlPos(), MD::WithPosition(15, 0, 29, 0));
    QCOMPARE(item2->text(), QStringLiteral("my link"));
    QCOMPARE_EQ(item2->textPos(), MD::WithPosition(33, 0, 39, 0));

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 1);
    QCOMPARE_EQ(item3->opts(), 2);
    QVERIFY(item3->isSpaceBefore());
    QVERIFY(item3->isSpaceAfter());
    QCOMPARE(item3->text(), QStringLiteral("text after with closing"));
    QCOMPARE_EQ(item3->startColumn(), 43);
    QCOMPARE_EQ(item3->endColumn(), 66);
}

/*
 *Text before with opening [[/my/link:header | my link]] and text after with closing* text outside
 */
void NoteLinkingTest::linkMiddle()
{
    QTextStream s(&m_testingLines[4], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("linkMiddle: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 4) {
        QFAIL("linkMiddle: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 1);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 2);
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
    QCOMPARE(item1->text(), QStringLiteral("Text before with opening"));
    QCOMPARE_EQ(item1->startColumn(), 1);
    QCOMPARE_EQ(item1->endColumn(), 25);

    const auto item2 = std::static_pointer_cast<MD::Link<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 0);
    QCOMPARE_EQ(item2->opts(), 2);
    QCOMPARE(item2->url(), QStringLiteral("/my/.BaseGroup/link@HEADER@header"));
    QCOMPARE_EQ(item2->urlPos(), MD::WithPosition(28, 0, 42, 0));
    QCOMPARE(item2->text(), QStringLiteral("my link"));
    QCOMPARE_EQ(item2->textPos(), MD::WithPosition(46, 0, 52, 0));

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 1);
    QCOMPARE_EQ(item3->opts(), 2);
    QVERIFY(item3->isSpaceBefore());
    QVERIFY(item3->isSpaceAfter());
    QCOMPARE(item3->text(), QStringLiteral("and text after with closing"));
    QCOMPARE_EQ(item3->startColumn(), 55);
    QCOMPARE_EQ(item3->endColumn(), 82);

    const auto item4 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(3));
    QCOMPARE_EQ(item4->openStyles().length(), 0);
    QCOMPARE_EQ(item4->closeStyles().length(), 0);
    QCOMPARE_EQ(item4->opts(), 0);
    QVERIFY(item4->isSpaceBefore());
    QVERIFY(item4->isSpaceAfter());
    QCOMPARE(item4->text(), QStringLiteral("text outside"));
    QCOMPARE_EQ(item4->startColumn(), 84);
    QCOMPARE_EQ(item4->endColumn(), 96);
}

/*
My [[my/Link | link]] with no header in the URL
*/
void NoteLinkingTest::linkNoHeader()
{
    QTextStream s(&m_testingLines[5], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("linkNoHeader: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("linkNoHeader: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
    QCOMPARE(item1->text(), QStringLiteral("My"));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 2);

    const auto item2 = std::static_pointer_cast<MD::Link<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 0);
    QCOMPARE_EQ(item2->opts(), 0);
    QCOMPARE(item2->url(), QStringLiteral("/my/.BaseGroup/Link@HEADER@"));
    QCOMPARE_EQ(item2->urlPos(), MD::WithPosition(5, 0, 11, 0));
    QCOMPARE(item2->text(), QStringLiteral("link"));
    QCOMPARE_EQ(item2->textPos(), MD::WithPosition(15, 0, 18, 0));

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 0);
    QCOMPARE_EQ(item3->opts(), 0);
    QVERIFY(item3->isSpaceBefore());
    QVERIFY(item3->isSpaceAfter());
    QCOMPARE(item3->text(), QStringLiteral("with no header in the URL"));
    QCOMPARE_EQ(item3->startColumn(), 21);
    QCOMPARE_EQ(item3->endColumn(), 46);
}

/*
My [[/link]] with no specific text
*/
void NoteLinkingTest::linkNoSpecificTitle()
{
    QTextStream s(&m_testingLines[6], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("linkNoSpecificTitle: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("linkNoSpecificTitle: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
    QCOMPARE(item1->text(), QStringLiteral("My"));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 2);

    const auto item2 = std::static_pointer_cast<MD::Link<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 0);
    QCOMPARE_EQ(item2->opts(), 0);
    QCOMPARE(item2->url(), QStringLiteral("/home/link@HEADER@"));
    QCOMPARE_EQ(item2->urlPos(), MD::WithPosition(5, 0, 9, 0));
    QCOMPARE(item2->text(), QStringLiteral("link"));
    QCOMPARE_EQ(item2->textPos(), MD::WithPosition(10, 0, 9, 0));

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 0);
    QCOMPARE_EQ(item3->opts(), 0);
    QVERIFY(item3->isSpaceBefore());
    QVERIFY(item3->isSpaceAfter());
    QCOMPARE(item3->text(), QStringLiteral("with no specific text"));
    QCOMPARE_EQ(item3->startColumn(), 12);
    QCOMPARE_EQ(item3->endColumn(), 33);
}

/*
Bad URL [[ | link]]
*/
void NoteLinkingTest::badUrl()
{
    QTextStream s(&m_testingLines[7], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("badUrl: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("badUrl: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
    QCOMPARE(item1->text(), QStringLiteral("Bad URL [[ | link]]"));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 18);
}

/*
Ill-formed [[URL | link]
*/
void NoteLinkingTest::illFormed1()
{
    QTextStream s(&m_testingLines[8], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("illFormed1: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("illFormed1: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
    QCOMPARE(item1->text(), QStringLiteral("Ill-formed [[URL | link]"));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 23);
}

/*
Ill-formed [URL | link]]
*/
void NoteLinkingTest::illFormed2()
{
    QTextStream s(&m_testingLines[9], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("illFormed2: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("illFormed2: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 0);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 0);
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
    QCOMPARE(item1->text(), QStringLiteral("Ill-formed [URL | link]]"));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 23);
}

/*
# *Text before with opening [[/my/link:header | my link]] and text after with closing* text outside in header
*/
void NoteLinkingTest::inTitle()
{
    QTextStream s(&m_testingLines[10], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("inTitle: Incorrect items count in the doc");
    }

    const auto heading = static_cast<MD::Heading<MD::QStringTrait> *>(doc->items().at(1).get());
    const auto paragraph = heading->text();
    if (paragraph->items().length() != 4) {
        QFAIL("inTitle: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->openStyles().length(), 1);
    QCOMPARE_EQ(item1->closeStyles().length(), 0);
    QCOMPARE_EQ(item1->opts(), 2);
    QVERIFY(item1->isSpaceBefore());
    QVERIFY(item1->isSpaceAfter());
    QCOMPARE(item1->text(), QStringLiteral("Text before with opening"));
    QCOMPARE_EQ(item1->startColumn(), 3);
    QCOMPARE_EQ(item1->endColumn(), 27);

    const auto item2 = std::static_pointer_cast<MD::Link<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->openStyles().length(), 0);
    QCOMPARE_EQ(item2->closeStyles().length(), 0);
    QCOMPARE_EQ(item2->opts(), 2);
    QCOMPARE(item2->url(), QStringLiteral("/my/.BaseGroup/link@HEADER@header"));
    QCOMPARE_EQ(item2->urlPos(), MD::WithPosition(30, 0, 44, 0));
    QCOMPARE(item2->text(), QStringLiteral("my link"));
    QCOMPARE_EQ(item2->textPos(), MD::WithPosition(48, 0, 54, 0));

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->openStyles().length(), 0);
    QCOMPARE_EQ(item3->closeStyles().length(), 1);
    QCOMPARE_EQ(item3->opts(), 2);
    QVERIFY(item3->isSpaceBefore());
    QVERIFY(item3->isSpaceAfter());
    QCOMPARE(item3->text(), QStringLiteral("and text after with closing"));
    QCOMPARE_EQ(item3->startColumn(), 57);
    QCOMPARE_EQ(item3->endColumn(), 84);

    const auto item4 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(3));
    QCOMPARE_EQ(item4->openStyles().length(), 0);
    QCOMPARE_EQ(item4->closeStyles().length(), 0);
    QCOMPARE_EQ(item4->opts(), 0);
    QVERIFY(item4->isSpaceBefore());
    QVERIFY(item4->isSpaceAfter());
    QCOMPARE(item4->text(), QStringLiteral("text outside in header"));
    QCOMPARE_EQ(item4->startColumn(), 86);
    QCOMPARE_EQ(item4->endColumn(), 108);
}

QTEST_MAIN(NoteLinkingTest)
#include "noteLinkingTest.moc"
