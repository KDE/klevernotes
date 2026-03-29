/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "emojiPlugin.hpp"

#include "emojiModel.h"
#include "kleverconfig.h"

// md4qt include.
#include <md4qt/src/inline_context.h>
#include <md4qt/src/text_stream.h>

namespace EmojiPlugin
{

// EmojiItem
MD::ItemType EmojiItem::type() const
{
    return MD::ItemType{static_cast<int>(MD::ItemType::UserDefined) + 1};
}

void EmojiItem::applyEmojiBase(const EmojiItem &other)
{
    if (this != &other) {
        MD::ItemWithOpts::applyItemWithOpts(other);
        setEmoji(other.emoji());
        setEmojiNamePos(other.emojiNamePos());
        setOptionsPos(other.optionsPos());
    }
}

QSharedPointer<MD::Item> EmojiItem::clone(MD::Document *doc) const
{
    Q_UNUSED(doc)

    auto t = QSharedPointer<EmojiItem>::create();
    t->applyEmojiBase(*this);

    return t;
}

void EmojiItem::setEmoji(const QString &emoji)
{
    m_emoji = emoji;
}

const QString &EmojiItem::emoji() const
{
    return m_emoji;
}

const MD::WithPosition &EmojiItem::emojiNamePos() const
{
    return m_emojiNamePos;
}

void EmojiItem::setEmojiNamePos(const MD::WithPosition &pos)
{
    m_emojiNamePos = pos;
}

const MD::WithPosition &EmojiItem::optionsPos() const
{
    return m_optionsPos;
}

void EmojiItem::setOptionsPos(const MD::WithPosition &pos)
{
    m_optionsPos = pos;
}

const MD::WithPosition &EmojiItem::startDelim() const
{
    return m_startDelim;
}

void EmojiItem::setStartDelim(const MD::WithPosition &pos)
{
    m_startDelim = pos;
}

const MD::WithPosition &EmojiItem::endDelim() const
{
    return m_endDelim;
}

void EmojiItem::setEndDelim(const MD::WithPosition &pos)
{
    m_endDelim = pos;
}
// !EmojiItem

static const QChar s_colon = QLatin1Char(':');

bool EmojiParser::check(MD::Line &line,
                        MD::ParagraphStream &,
                        MD::InlineContext &ctx,
                        QSharedPointer<MD::Document>,
                        const QString &,
                        const QString &,
                        QStringList &,
                        MD::Parser &,
                        const MD::ReverseSolidusHandler &)
{
    const auto st = line.currentState();
    MD::Line::State toneStartState;
    line.nextChar();
    QString face;
    QString tone;
    auto faceFound = false;
    auto toneFound = false;

    if (!line.currentChar().isSpace()) {
        while (line.position() < line.length()) {
            if (line.currentChar() != s_colon) {
                face.append(line.currentChar());
                line.nextChar();
            } else {
                faceFound = true;

                line.nextChar();

                toneStartState = line.currentState();

                while (line.position() < line.length()) {
                    if (line.currentChar() != s_colon) {
                        tone.append(line.currentChar());
                        line.nextChar();
                    } else {
                        toneFound = true;
                        line.nextChar();
                        break;
                    }
                }

                break;
            }
        }

        if (faceFound && !face.isEmpty()) {
            QStringList optionsInfo;
            if (toneFound && !tone.isEmpty()) {
                optionsInfo = tone.split(QStringLiteral(","));
            }

            if (!toneFound) {
                line.restoreState(&toneStartState);
            }

            static const auto emojiModel = &EmojiModel::instance();
            static const QString defaultToneStr = QStringLiteral("default skin tone");
            static const QSet<QString> tonesOptions = {
                QStringLiteral("dark skin tone"),
                QStringLiteral("medium-dark skin tone"),
                QStringLiteral("medium skin tone"),
                QStringLiteral("medium-light skin tone"),
                QStringLiteral("light skin tone"),
                defaultToneStr, // To possibly overwrite the default in config
            };

            const QString configTone = KleverConfig::emojiTone();
            QString tone = configTone == QStringLiteral("None") ? defaultToneStr : configTone;
            QString givenVariant;
            bool toneGiven = false;

            if (!optionsInfo.isEmpty()) {
                // e.g:
                // "woman: dark skin tone, blond hair"
                // "woman: blond hair"
                const QString possibleTone = optionsInfo[0].trimmed().toLower();
                if (tonesOptions.contains(possibleTone)) {
                    tone = possibleTone;
                    toneGiven = true;
                } else {
                    givenVariant = possibleTone;
                }
                if (1 < optionsInfo.length()) {
                    givenVariant = optionsInfo[1].trimmed();
                }
            }

            const bool defaultToneGiven = tone == defaultToneStr;

            const QString searchTerm = givenVariant.isEmpty() ? face : (face + QStringLiteral(": ") + givenVariant);

            QString uniEmoji;
            bool variantFound = false;

            if (!defaultToneGiven) { // Check for tones, but will also gives tones + variant
                const QVariantList tonedEmojis = emojiModel->tones(face);
                for (auto it = tonedEmojis.begin(); it != tonedEmojis.end(); ++it) {
                    const Emoji currentEmoji = it->value<Emoji>();
                    const QString tonedEmojiName = currentEmoji.shortName;
                    if (tonedEmojiName.contains(QStringLiteral(" ") + tone)) {
                        // The first result are the "closest" to the search term
                        // This ensure a "sain" default if the perfect match is not found
                        if (uniEmoji.isEmpty()) {
                            uniEmoji = currentEmoji.unicode;
                        }

                        // looking for tone + variant
                        if (!givenVariant.isEmpty() && tonedEmojiName.endsWith(givenVariant)) {
                            uniEmoji = currentEmoji.unicode;
                            variantFound = true;
                            break;
                        }
                        // A tone can also come from config
                        if (toneGiven) {
                            uniEmoji = currentEmoji.unicode;
                            variantFound = true;
                            break;
                        }
                    }
                }
            } else { // Only check for variant, e.g: "blond hair", "red hair", ...
                const QVariantList possibleEmojis = emojiModel->filterModelNoCustom(searchTerm);
                for (auto it = possibleEmojis.begin(); it != possibleEmojis.end(); it++) {
                    const Emoji currentEmoji = it->value<Emoji>();
                    if (currentEmoji.shortName == searchTerm) {
                        uniEmoji = currentEmoji.unicode;
                        variantFound = !givenVariant.isEmpty() || (defaultToneGiven && toneGiven);
                        break;
                    }
                }
            }

            if (uniEmoji.isEmpty()) { // Last try to find one
                const QVariantList possibleEmojis = emojiModel->filterModelNoCustom(face);
                for (auto it = possibleEmojis.begin(); it != possibleEmojis.end(); it++) {
                    const Emoji currentEmoji = it->value<Emoji>();
                    if (currentEmoji.shortName == face) {
                        uniEmoji = currentEmoji.unicode;
                        break;
                    }
                }
            }

            if (!uniEmoji.isEmpty()) {
                if (!variantFound) {
                    line.restoreState(&toneStartState);
                }

                auto emojiItem = QSharedPointer<EmojiItem>::create();
                const auto endPos = line.position() - 1;
                emojiItem->applyPositions({st.m_pos, line.lineNumber(), endPos, line.lineNumber()});
                emojiItem->setStartDelim({st.m_pos, line.lineNumber(), st.m_pos, line.lineNumber()});
                emojiItem->setEndDelim({endPos, line.lineNumber(), endPos, line.lineNumber()});
                emojiItem->setEmoji(uniEmoji);

                const long long int nameStart = st.m_pos + 1;
                const long long int nameEnd = toneStartState.m_pos - 2;
                emojiItem->setEmojiNamePos({nameStart, emojiItem->startLine(), nameEnd, emojiItem->endLine()});

                const long long int optionsStart = variantFound || (toneGiven && defaultToneGiven) ? toneStartState.m_pos - 1 : nameStart;
                const long long int optionsEnd = variantFound || (toneGiven && defaultToneGiven) ? line.position() - 2 : nameEnd;
                emojiItem->setOptionsPos({optionsStart, emojiItem->startLine(), optionsEnd, emojiItem->endLine()});

                ctx.inlines().append(emojiItem);

                return true;
            }
        }
    }

    line.restoreState(&st);

    return false;
}

QString EmojiParser::startDelimiterSymbols() const
{
    return QStringLiteral(":");
}
}
