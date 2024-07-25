/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#include "emojiExtension.hpp"

#include "emojiModel.h"
#include "kleverconfig.h"
#include "logic/parser/md4qtDataManip.hpp"

// Qt include
#include <QRegularExpression>
#include <qlogging.h>

namespace EmojiExtension
{

inline long long int processEmoji(MDParagraphPtr p, MDParsingOpts &po, long long int rawIdx)
{
    if (rawIdx < 0 || rawIdx >= static_cast<long long>(po.rawTextData.size())) {
        return rawIdx;
    }

    auto textData = po.rawTextData[rawIdx];
    QString src = textData.str;

    static const auto emojiModel = &EmojiModel::instance();
    static const QRegularExpression inline_emoji = QRegularExpression(QStringLiteral(":(?=\\S)([^:]*)(:?)([^:]*):"));
    static const QString defaultToneStr = QStringLiteral("default skin tone");
    static const QSet<QString> tonesOptions = {
        QStringLiteral("dark skin tone"),
        QStringLiteral("medium-dark skin tone"),
        QStringLiteral("medium skin tone"),
        QStringLiteral("medium-light skin tone"),
        QStringLiteral("light skin tone"),
        defaultToneStr, // To possibly overwrite the default in config
    };

    int offSet = 0;
    while (offSet != src.length()) {
        QRegularExpressionMatch cap = inline_emoji.matchView(src, offSet);
        if (cap.hasMatch()) {
            const QString emojiName = cap.captured(1).trimmed();
            const QString possibleVariants = cap.captured(3).trimmed();

            QStringList variantInfo;
            if (!possibleVariants.isEmpty()) {
                variantInfo = possibleVariants.split(QStringLiteral(","));
            }

            const QString configTone = KleverConfig::emojiTone();
            QString tone = configTone == QStringLiteral("None") ? defaultToneStr : configTone;
            QString givenVariant;
            bool toneGiven = false;
            if (!variantInfo.isEmpty()) {
                // e.g:
                // "woman: dark skin tone, blond hair"
                // "woman: blond hair"
                const QString possibleTone = variantInfo[0].trimmed().toLower();
                if (tonesOptions.contains(possibleTone)) {
                    tone = possibleTone;
                    toneGiven = true;
                } else {
                    givenVariant = possibleTone;
                }
                if (1 < variantInfo.length()) {
                    givenVariant = variantInfo[1].trimmed();
                }
            }
            const bool defaultToneGiven = tone == defaultToneStr;

            const QString searchTerm = givenVariant.isEmpty() ? emojiName : (emojiName + QStringLiteral(": ") + givenVariant);

            QString uniEmoji;
            bool variantFound = false;
            if (!defaultToneGiven) { // Check for tones, but will also gives tones + variant
                const QVariantList tonedEmojis = emojiModel->tones(emojiName);
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
                    const QString tonedEmojiName = currentEmoji.shortName;
                    if (currentEmoji.shortName == searchTerm) {
                        uniEmoji = currentEmoji.unicode;
                        variantFound = !givenVariant.isEmpty() || defaultToneGiven;
                        break;
                    }
                }
            }

            if (uniEmoji.isEmpty()) { // Last try to find one
                const QVariantList possibleEmojis = emojiModel->filterModelNoCustom(emojiName);
                for (auto it = possibleEmojis.begin(); it != possibleEmojis.end(); it++) {
                    const Emoji currentEmoji = it->value<Emoji>();
                    if (currentEmoji.shortName == emojiName) {
                        uniEmoji = currentEmoji.unicode;
                        break;
                    }
                }
            }

            static const QString surroundingStr = QStringLiteral("::");
            const int firstPartCap = cap.capturedStart() + cap.capturedLength(1) + surroundingStr.length();
            if (uniEmoji.isEmpty()) {
                offSet = firstPartCap;
                continue;
            }

            auto lineInfo = po.fr.data.at(textData.line);
            auto paraIdx = textAtIdx(p, rawIdx);
            const auto item = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx));

            const long long int splitLength = variantFound || toneGiven ? cap.capturedLength() : firstPartCap;
            const long long int virginStartPos = item->startColumn() + cap.capturedStart();
            const long long int virginEndPos = item->startColumn() + cap.capturedStart() + splitLength - 1;

            std::shared_ptr<MD::Link<MD::QStringTrait>> emojiItem(new MD::Link<MD::QStringTrait>);
            emojiItem->setStartColumn(virginStartPos);
            emojiItem->setStartLine(item->startLine());
            emojiItem->setEndColumn(virginEndPos);
            emojiItem->setEndLine(item->endLine());
            emojiItem->setOpts(item->opts());
            emojiItem->setText(uniEmoji);
            emojiItem->setTextPos({virginStartPos, emojiItem->startLine(), virginEndPos, emojiItem->endLine()});
            emojiItem->setUrl(QStringLiteral("copy:") + uniEmoji);
            emojiItem->setUrlPos({virginStartPos, emojiItem->startLine(), virginEndPos, emojiItem->endLine()});

            const int addedData = md4qtHelperFunc::splitItem(p, po, paraIdx, rawIdx, virginStartPos, splitLength);
            if (addedData == -1) {
                emojiItem->openStyles() << item->openStyles();
                emojiItem->closeStyles() << item->closeStyles();
                p->removeItemAt(paraIdx);
                po.rawTextData.erase(po.rawTextData.cbegin() + rawIdx);
            } else if (addedData == 1) {
                const auto currentItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx));
                const auto nextItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx + 1));
                md4qtHelperFunc::transferStyle(currentItem, nextItem, true);
            }

            // If capture start at 0, then we stay on the same index
            const long long int emojiParaIdx = cap.capturedStart() ? paraIdx + 1 : paraIdx;
            p->insertItem(emojiParaIdx, emojiItem);

            if (addedData == 0) {
                const auto currentItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx));
                const auto nextItem = md4qtHelperFunc::getSharedItemWithOpts(p->getItemAt(paraIdx + 1));
                if (cap.capturedStart()) {
                    md4qtHelperFunc::transferStyle(currentItem, nextItem, true);
                    md4qtHelperFunc::transferStyle(currentItem, nextItem, false);
                } else {
                    md4qtHelperFunc::transferStyle(nextItem, currentItem, true);
                    md4qtHelperFunc::transferStyle(nextItem, currentItem, false);
                }
            }

            return addedData < 0 ? rawIdx : ++rawIdx;
        }
        break;
    }
    return ++rawIdx;
}

void emojiHelperFunc(MDParagraphPtr p, MDParsingOpts &po, const QStringList &options)
{
    Q_UNUSED(options);
    if (!po.collectRefLinks) {
        long long int i = 0;

        while (0 <= i && i < (long long int)po.rawTextData.size()) {
            i = processEmoji(p, po, i);
        }
    }
}
}
