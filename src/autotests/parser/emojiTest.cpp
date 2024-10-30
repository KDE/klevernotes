/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "logic/parser/md4qtDataCleaner.hpp"
#include "logic/parser/plugins/emoji/emojiPlugin.hpp"

// Qt include
#include <QObject>
#include <QTextStream>
#include <QtTest/QTest>

#define MD4QT_QT_SUPPORT
#include "logic/parser/md4qt/doc.h"
#include "logic/parser/md4qt/parser.h"
#include "logic/parser/md4qt/traits.h"

class EmojiTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void simpleEmojiMiddle();
    void simpleEmojiStart();
    void simpleEmojiEnd();
    void simpleEmojiWithStyle();
    void darkSkinEmoji();
    void mediumDarkSkinEmoji();
    void mediumSkinEmoji();
    void mediumLightSkinEmoji();
    void lightSkinEmoji();
    void defaultSkinEmoji();
    void wrongSkinEmoji();
    void variantEmoji();
    void wrongVariantEmoji();
    void toneAndVariantEmoji();
    void wrongToneAndVariantEmoji();
    void toneAndWrongVariantEmoji();
    void wrongToneAndWrongVariantEmoji();
    void badEmoji1();
    void badEmoji2();
    void badEmoji3();
    void badEmoji4();

private:
    // md4qt
    MD::Parser<MD::QStringTrait> m_md4qtParser;
    const QString dummyPath = QStringLiteral("/home/dummy/");
    // Data
    QStringList m_testingLines = {
        QStringLiteral("Simple :woman: emoji in the middle"),
        QStringLiteral(":woman: simple emoji at start"),
        QStringLiteral("Simple emoji at end :woman:"),
        QStringLiteral("Simple *:woman:* emoji with style"),
        QStringLiteral("Dark skin emoji :woman:dark skin tone:"),
        QStringLiteral("Medium dark skin emoji :woman:medium-dark skin tone:"),
        QStringLiteral("Medium skin emoji :woman:medium skin tone:"),
        QStringLiteral("Medium light skin emoji :woman:medium-light skin tone:"),
        QStringLiteral("Light skin emoji :woman:light skin tone:"),
        QStringLiteral("Default skin emoji :woman:default skin tone:"),
        QStringLiteral("Wrong given skin tone emoji :woman:wrong skin tone:"),
        QStringLiteral("Variant emoji :woman:red hair:"),
        QStringLiteral("Wrong variant emoji :woman: wrong hair:"),
        QStringLiteral("Tone + variant emoji :woman:light skin tone, red hair:"),
        QStringLiteral("Wrong tone + variant emoji :woman: wrong skin tone, red hair:"),
        QStringLiteral("Tone + wrong variant emoji :woman: dark skin tone, wrong hair:"),
        QStringLiteral("Wrong tone + wrong variant emoji :woman: wrong skin tone, wrong hair:"),
        QStringLiteral(":bad emoji:"),
        QStringLiteral(":bad emoji: red hair:"),
        QStringLiteral(":bad emoji: light skin tone:"),
        QStringLiteral(":bad emoji: light skin tone, red hair:"),
    };
};

/* Settings Data */
void EmojiTest::initTestCase()
{
    m_md4qtParser.addTextPlugin(256, EmojiPlugin::emojiHelperFunc, true, {});
    m_md4qtParser.addTextPlugin(1024, md4qtDataCleaner::dataCleaningFunc, false, {});
}

/* TEST */
/*
Simple :woman: emoji in the middle
*/
void EmojiTest::simpleEmojiMiddle()
{
    QTextStream s(&m_testingLines[0], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("simpleEmojiMiddle: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("simpleEmojiMiddle: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 6);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Simple "));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 7);
    QCOMPARE_EQ(item2->endColumn(), 13);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👩"));
    MD::WithPosition namePos = {8, 0, 12, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = namePos;
    auto t = item2->optionsPos();
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->startColumn(), 14);
    QCOMPARE_EQ(item3->endColumn(), 33);
    QCOMPARE_EQ(item3->startLine(), 0);
    QCOMPARE_EQ(item3->endLine(), 0);
    QCOMPARE(item3->text(), QStringLiteral(" emoji in the middle"));
}

/*
:woman: simple emoji at start
*/
void EmojiTest::simpleEmojiStart()
{
    QTextStream s(&m_testingLines[1], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("simpleEmojiStart: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("simpleEmojiStart: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 6);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->emoji(), QStringLiteral("👩"));
    MD::WithPosition namePos = {1, 0, 5, 0};
    QCOMPARE_EQ(item1->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = namePos;
    QCOMPARE_EQ(item1->optionsPos(), optionsPos);

    const auto item2 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 7);
    QCOMPARE_EQ(item2->endColumn(), 28);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->text(), QStringLiteral("simple emoji at start"));
}

/*
Simple emoji at end :woman:
*/
void EmojiTest::simpleEmojiEnd()
{
    QTextStream s(&m_testingLines[2], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("simpleEmojiEnd: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("simpleEmojiEnd: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 19);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Simple emoji at end"));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 20);
    QCOMPARE_EQ(item2->endColumn(), 26);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👩"));
    MD::WithPosition namePos = {21, 0, 25, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = namePos;
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);
}

/*
Simple *:woman:* emoji with style
*/
void EmojiTest::simpleEmojiWithStyle()
{
    QTextStream s(&m_testingLines[3], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("simpleEmojiWithStyle: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("simpleEmojiWithStyle: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 6);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Simple "));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 8);
    QCOMPARE_EQ(item2->endColumn(), 14);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->opts(), 2);
    QCOMPARE_EQ(item2->openStyles().length(), 1);
    QCOMPARE_EQ(item2->closeStyles().length(), 1);
    QCOMPARE(item2->emoji(), QStringLiteral("👩"));
    MD::WithPosition namePos = {9, 0, 13, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = namePos;
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->startColumn(), 16);
    QCOMPARE_EQ(item3->endColumn(), 32);
    QCOMPARE_EQ(item3->startLine(), 0);
    QCOMPARE_EQ(item3->endLine(), 0);
    QCOMPARE(item3->text(), QStringLiteral(" emoji with style"));
}

/*
Dark skin emoji :woman:dark skin tone:
*/
void EmojiTest::darkSkinEmoji()
{
    QTextStream s(&m_testingLines[4], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("darkSkinEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("darkSkinEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 15);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Dark skin emoji"));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 16);
    QCOMPARE_EQ(item2->endColumn(), 37);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👱🏿‍♀️"));
    MD::WithPosition namePos = {17, 0, 21, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = {22, 0, 36, 0};
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);
}

/*
Medium dark skin emoji :woman:medium-dark skin tone:
*/
void EmojiTest::mediumDarkSkinEmoji()
{
    QTextStream s(&m_testingLines[5], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("mediumDarkSkinEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("mediumDarkSkinEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 22);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Medium dark skin emoji"));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 23);
    QCOMPARE_EQ(item2->endColumn(), 51);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👱🏾‍♀️"));
    MD::WithPosition namePos = {24, 0, 28, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = {29, 0, 50, 0};
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);
}

/*
Medium skin emoji :woman:medium skin tone:
*/
void EmojiTest::mediumSkinEmoji()
{
    QTextStream s(&m_testingLines[6], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("mediumSkinEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("mediumSkinEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 17);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Medium skin emoji"));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 18);
    QCOMPARE_EQ(item2->endColumn(), 41);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👱🏽‍♀️"));
    MD::WithPosition namePos = {19, 0, 23, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = {24, 0, 40, 0};
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);
}

/*
Medium light skin emoji :woman:medium-light skin tone:
*/
void EmojiTest::mediumLightSkinEmoji()
{
    QTextStream s(&m_testingLines[7], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("mediumLightSkinEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("mediumLightSkinEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 23);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Medium light skin emoji"));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 24);
    QCOMPARE_EQ(item2->endColumn(), 53);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👱🏼‍♀️"));
    MD::WithPosition namePos = {25, 0, 29, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = {30, 0, 52, 0};
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);
}

/*
Light skin emoji :woman:light skin tone:
*/
void EmojiTest::lightSkinEmoji()
{
    QTextStream s(&m_testingLines[8], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("lightSkinEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("lightSkinEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 16);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Light skin emoji"));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 17);
    QCOMPARE_EQ(item2->endColumn(), 39);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👱🏻‍♀️"));
    MD::WithPosition namePos = {18, 0, 22, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = {23, 0, 38, 0};
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);
}

/*
Default skin emoji :woman:default skin tone:
*/
void EmojiTest::defaultSkinEmoji()
{
    QTextStream s(&m_testingLines[9], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("defaultSkinEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("defaultSkinEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 18);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Default skin emoji"));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 19);
    QCOMPARE_EQ(item2->endColumn(), 43);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👩"));
    MD::WithPosition namePos = {20, 0, 24, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = {25, 0, 42, 0};
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);
}

/*
Wrong given skin tone emoji :woman:wrong skin tone:
*/
void EmojiTest::wrongSkinEmoji()
{
    QTextStream s(&m_testingLines[10], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("wrongSkinEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("wrongSkinEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 27);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Wrong given skin tone emoji "));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 28);
    QCOMPARE_EQ(item2->endColumn(), 34);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👩"));
    MD::WithPosition namePos = {29, 0, 33, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = namePos;
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->startColumn(), 35);
    QCOMPARE_EQ(item3->endColumn(), 50);
    QCOMPARE_EQ(item3->startLine(), 0);
    QCOMPARE_EQ(item3->endLine(), 0);
    QCOMPARE(item3->text(), QStringLiteral("wrong skin tone:"));
}

/*
Variant emoji :woman:red hair:
*/
void EmojiTest::variantEmoji()
{
    QTextStream s(&m_testingLines[11], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("variantEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("variantEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 13);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Variant emoji"));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 14);
    QCOMPARE_EQ(item2->endColumn(), 29);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👩‍🦰"));
    MD::WithPosition namePos = {15, 0, 19, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = {20, 0, 28, 0};
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);
}

/*
Wrong variant emoji :woman: wrong hair:
*/
void EmojiTest::wrongVariantEmoji()
{
    QTextStream s(&m_testingLines[12], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("wrongVariantEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("wrongVariantEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 19);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Wrong variant emoji "));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 20);
    QCOMPARE_EQ(item2->endColumn(), 26);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👩"));
    MD::WithPosition namePos = {21, 0, 25, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = namePos;
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->startColumn(), 27);
    QCOMPARE_EQ(item3->endColumn(), 38);
    QCOMPARE_EQ(item3->startLine(), 0);
    QCOMPARE_EQ(item3->endLine(), 0);
    QCOMPARE(item3->text(), QStringLiteral(" wrong hair:"));
}

/*
Tone + variant emoji :woman:light skin tone, red hair:
*/
void EmojiTest::toneAndVariantEmoji()
{
    QTextStream s(&m_testingLines[13], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("toneAndVariantEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("toneAndVariantEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 20);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Tone + variant emoji"));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 21);
    QCOMPARE_EQ(item2->endColumn(), 53);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👱🏻‍♀️"));
    MD::WithPosition namePos = {22, 0, 26, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = {27, 0, 52, 0};
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);
}

/*
Wrong tone + variant emoji :woman: wrong skin tone, red hair:
*/
void EmojiTest::wrongToneAndVariantEmoji()
{
    QTextStream s(&m_testingLines[14], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("wrongToneAndVariantEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("wrongToneAndVariantEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 26);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Wrong tone + variant emoji"));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 27);
    QCOMPARE_EQ(item2->endColumn(), 60);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👩‍🦰"));
    MD::WithPosition namePos = {28, 0, 32, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = {33, 0, 59, 0};
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);
}

/*
Tone + wrong variant emoji :woman: dark skin tone, wrong hair:
*/
void EmojiTest::toneAndWrongVariantEmoji()
{
    QTextStream s(&m_testingLines[15], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("toneAndWrongVariantEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 2) {
        QFAIL("toneAndWrongVariantEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 26);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Tone + wrong variant emoji"));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 27);
    QCOMPARE_EQ(item2->endColumn(), 61);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👱🏿‍♀️"));
    MD::WithPosition namePos = {28, 0, 32, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = {33, 0, 60, 0};
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);
}

/*
Wrong tone + wrong variant emoji :woman: wrong skin tone, wrong hair:
*/
void EmojiTest::wrongToneAndWrongVariantEmoji()
{
    QTextStream s(&m_testingLines[16], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("wrongToneAndWrongVariantEmoji: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 3) {
        QFAIL("wrongToneAndWrongVariantEmoji: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 32);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral("Wrong tone + wrong variant emoji "));

    const auto item2 = std::static_pointer_cast<EmojiPlugin::EmojiItem>(paragraph->getItemAt(1));
    QCOMPARE_EQ(item2->startColumn(), 33);
    QCOMPARE_EQ(item2->endColumn(), 39);
    QCOMPARE_EQ(item2->startLine(), 0);
    QCOMPARE_EQ(item2->endLine(), 0);
    QCOMPARE(item2->emoji(), QStringLiteral("👩"));
    MD::WithPosition namePos = {34, 0, 38, 0};
    QCOMPARE_EQ(item2->emojiNamePos(), namePos);
    MD::WithPosition optionsPos = namePos;
    QCOMPARE_EQ(item2->optionsPos(), optionsPos);

    const auto item3 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(2));
    QCOMPARE_EQ(item3->startColumn(), 40);
    QCOMPARE_EQ(item3->endColumn(), 68);
    QCOMPARE_EQ(item3->startLine(), 0);
    QCOMPARE_EQ(item3->endLine(), 0);
    QCOMPARE(item3->text(), QStringLiteral(" wrong skin tone, wrong hair:"));
}

/*
:bad emoji:
*/
void EmojiTest::badEmoji1()
{
    QTextStream s(&m_testingLines[17], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("badEmoji1: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("badEmoji1: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 10);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral(":bad emoji:"));
}

/*
:bad emoji: red hair:
*/
void EmojiTest::badEmoji2()
{
    QTextStream s(&m_testingLines[18], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("badEmoji2: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("badEmoji2: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 20);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral(":bad emoji: red hair:"));
}

/*
:bad emoji: light skin tone:
*/
void EmojiTest::badEmoji3()
{
    QTextStream s(&m_testingLines[19], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("badEmoji3: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("badEmoji3: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 27);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral(":bad emoji: light skin tone:"));
}

/*
:bad emoji: light skin tone, red hair:
*/
void EmojiTest::badEmoji4()
{
    QTextStream s(&m_testingLines[20], QIODeviceBase::ReadOnly);
    const auto doc = m_md4qtParser.parse(s, dummyPath, QStringLiteral("note.md"));
    if (doc->items().length() != 2) {
        QFAIL("badEmoji4: Incorrect items count in the doc");
    }

    const auto paragraph = static_cast<MD::Paragraph<MD::QStringTrait> *>(doc->items().at(1).get());
    if (paragraph->items().length() != 1) {
        QFAIL("badEmoji4: Incorrect items count in the paragraph");
    }

    const auto item1 = std::static_pointer_cast<MD::Text<MD::QStringTrait>>(paragraph->getItemAt(0));
    QCOMPARE_EQ(item1->startColumn(), 0);
    QCOMPARE_EQ(item1->endColumn(), 37);
    QCOMPARE_EQ(item1->startLine(), 0);
    QCOMPARE_EQ(item1->endLine(), 0);
    QCOMPARE(item1->text(), QStringLiteral(":bad emoji: light skin tone, red hair:"));
}
QTEST_MAIN(EmojiTest)
#include "emojiTest.moc"
