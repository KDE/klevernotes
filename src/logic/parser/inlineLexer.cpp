/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2023 Louis Schul <schul9louis@gmail.com>
*/

// CREDIT TO ORIGINAL IDEA: https://marked.js.org/
#include "inlineLexer.h"
// #include <QDebug>

#include <QDir>
#include <QMap>
#include <QRandomGenerator>

#include "kleverconfig.h"
#include "logic/plugins/emoji/emojiModel.h"
#include "parser.h"
#include "renderer.h"

InlineLexer::InlineLexer(Parser *parser)
    : m_parser(parser)
{
}

QString InlineLexer::output(QString &src, bool useInlineText)
{
    static const QString emptyStr = QLatin1String();
    QString out = emptyStr, text, href, title, cap0, cap1, cap2, cap3, cap4, outputed;
    QMap<QString, QString> linkInfo;
    QRegularExpressionMatch cap, secondCap;

    static const PluginHelper *pluginHelper = m_parser->getPluginHelper();
    static NoteMapperParserUtils *mapperParserUtils = pluginHelper->getMapperParserUtils();

    while (!src.isEmpty()) {
        cap = inline_escape.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            out += cap.captured(1);
            continue;
        }

        cap = inline_autolink.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            if (cap.captured(2) == QStringLiteral("@")) {
                cap1 = cap.captured(1);
                QString mangled = mangle(cap1);
                text = Renderer::escape(mangled, false);
                href = QStringLiteral("mailto:") + text;
            } else {
                cap1 = cap.captured(1);
                text = Renderer::escape(cap1, false);
                href = text;
            }
            title = emptyStr;

            out += Renderer::link(href, title, text);
            continue;
        }

        // url (gfm)
        cap = inline_url.match(src);
        if (!m_inLink && cap.hasMatch()) {
            cap0 = inline_backPedal.match(cap.captured(0)).captured(0);
            src.replace(src.indexOf(cap0), cap.capturedLength(), emptyStr);

            if (cap.captured(2) == QStringLiteral("@")) {
                text = Renderer::escape(cap0, false);
                href = QStringLiteral("mailto:") + text;
            } else {
                text = Renderer::escape(cap0, false);
                if (cap.captured(1) == QStringLiteral("www.")) {
                    href = QStringLiteral("http://") + text;
                } else {
                    href = text;
                }
            }
            title = emptyStr;

            out += Renderer::link(href, title, text);
            continue;
        }

        // tag
        cap = inline_tag.match(src);
        if (cap.hasMatch()) {
            static const QRegularExpression aTagOpenReg = QRegularExpression(QStringLiteral("^<a "), QRegularExpression::CaseInsensitiveOption);
            const bool hasOpeningLink = aTagOpenReg.match(cap.captured(0)).hasMatch();
            static const QRegularExpression aTagCloseRag = QRegularExpression(QStringLiteral("^<\\/a>"), QRegularExpression::CaseInsensitiveOption);
            const bool hasClosingLink = aTagCloseRag.match(cap.captured(0)).hasMatch();

            if (!m_inLink && hasOpeningLink) {
                m_inLink = true;
            } else if (m_inLink && hasClosingLink) {
                m_inLink = false;
            }
            src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            out += cap.captured(0);
            continue;
        }

        // link
        cap = inline_link.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            m_inLink = true;
            href = cap.captured(2);

            const int end = cap.captured(3).length() - 2;
            title = !cap.captured(3).isEmpty() ? cap.captured(3).mid(1, end) : emptyStr;

            href = href.trimmed();
            static const QRegularExpression tagReg = QRegularExpression(QStringLiteral("^<([\\s\\S]*)>$"));
            const QRegularExpressionMatch tagMatch = tagReg.match(href);
            href = href.replace(tagMatch.capturedStart(), tagMatch.capturedLength(), tagMatch.captured(1));
            linkInfo = {{QStringLiteral("href"), escapes(href)}, {QStringLiteral("title"), escapes(title)}};

            out += outputLink(cap, linkInfo, useInlineText);
            m_inLink = false;
            continue;
        }

        // wikilink
        if (KleverConfig::noteMapEnabled()) {
            const static QRegularExpression inline_wikilink =
                QRegularExpression(QStringLiteral("\\[\\[([^:\\]\\|\\r\\n]*)(:)?([^:\\]\\|\\r\\n]*)(\\|)?([^:\\]\\|\\r\\n]*)\\]\\]"));
            cap = inline_wikilink.match(src);
            if (cap.hasMatch()) {
                src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);
                if (!cap.captured(1).trimmed().isEmpty()) {
                    href = cap.captured(1).trimmed();
                    const QPair<QString, bool> sanitizedHref = mapperParserUtils->sanitizePath(href);

                    cap3 = cap.captured(3).trimmed();

                    const bool hasPipe = !cap.captured(4).isEmpty();

                    const QString potentitalTitle = cap.captured(5).trimmed();
                    title = hasPipe && !potentitalTitle.isEmpty() ? potentitalTitle : sanitizedHref.first.split(QStringLiteral("/")).last();

                    if (sanitizedHref.second) {
                        mapperParserUtils->addToLinkedNoteInfos({sanitizedHref.first, cap3, title});
                        // This hopefuly, is enough to separate the 2 without collinding with user input
                        QString fullLink = sanitizedHref.first + QStringLiteral("@HEADER@") + cap3; // <Note path>@HEADER@<header ref>
                        out += Renderer::wikilink(fullLink, title, title);
                        continue;
                    }
                }
                // Not a note path
                out += Renderer::paragraph(cap.captured(0));
                continue;
            }
        }

        // reflink, nolink
        const QRegularExpressionMatch tempMatch = inline_reflink.match(src);
        if (tempMatch.hasMatch())
            cap = tempMatch;
        else
            cap = inline_nolink.match(src);

        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            cap2 = cap.captured(2);
            static const QRegularExpression whiteSpaceReg = QRegularExpression(QStringLiteral("\\s+"));
            cap1 = cap.captured(1).replace(whiteSpaceReg, QStringLiteral(" "));

            const QString linkId = !cap2.isEmpty() ? cap2 : cap1;

            linkInfo = m_parser->links[linkId.toLower()];
            if (linkInfo.isEmpty() || linkInfo[QStringLiteral("href")].isEmpty()) {
                out += cap.captured(0).at(0);
                src = cap.captured(0).mid(1) + src;
                continue;
            }

            m_inLink = true;
            out += outputLink(cap, linkInfo, useInlineText);
            m_inLink = false;
            continue;
        }

        // strong
        cap = inline_strong.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            cap4 = cap.captured(4);
            cap3 = cap.captured(3);
            cap2 = cap.captured(2);
            cap1 = cap.captured(1);

            QString toOutput;
            if (!cap4.isEmpty())
                toOutput = cap4;
            else if (!cap3.isEmpty())
                toOutput = cap3;
            else if (!cap2.isEmpty())
                toOutput = cap2;
            else
                toOutput = cap1;

            outputed = output(toOutput);

            out += Renderer::strong(outputed);
            continue;
        }

        // em
        cap = inline_em.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            const QString cap6 = cap.captured(6);
            const QString cap5 = cap.captured(5);
            cap4 = cap.captured(4);
            cap3 = cap.captured(3);
            cap2 = cap.captured(2);
            cap1 = cap.captured(1);

            QString toOutput;
            if (!cap6.isEmpty())
                toOutput = cap6;
            else if (!cap5.isEmpty())
                toOutput = cap5;
            else if (!cap4.isEmpty())
                toOutput = cap4;
            else if (!cap3.isEmpty())
                toOutput = cap3;
            else if (!cap2.isEmpty())
                toOutput = cap2;
            else
                toOutput = cap1;

            outputed = output(toOutput);

            out += Renderer::em(outputed);
            continue;
        }

        // code
        cap = inline_code.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            QString capTrimmed = cap.captured(2).trimmed();
            const QString escaped = Renderer::escape(capTrimmed, true);

            out += Renderer::codeSpan(escaped);
            continue;
        }

        // br
        cap = inline_br.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            if (!useInlineText)
                out += Renderer::br();
            continue;
        }

        // del (gfm)
        cap = inline_del.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            cap1 = cap.captured(1);
            outputed = output(cap1);

            out += Renderer::del(outputed);

            continue;
        }

        cap = inline_highlight.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            cap1 = cap.captured(1);
            outputed = output(cap1);

            out += Renderer::mark(outputed);
            continue;
        }

        // emoji
        if (KleverConfig::quickEmojiEnabled()) {
            static const auto emojiModel = &EmojiModel::instance();
            static const QRegularExpression inline_emoji = QRegularExpression(QStringLiteral("^:(?=\\S)([^:]*)(:?)([^:]*):"));
            static const QString defaultToneStr = QStringLiteral("default skin tone");
            static const QSet<QString> possibleTones = {
                QStringLiteral("dark skin tone"),
                QStringLiteral("medium-dark skin tone"),
                QStringLiteral("medium skin tone"),
                QStringLiteral("medium-light skin tone"),
                QStringLiteral("light skin tone"),
                defaultToneStr, // To possibly overwrite the default in config
            };

            cap = inline_emoji.match(src);
            if (cap.hasMatch()) {
                cap0 = cap.captured(0);
                cap1 = cap.captured(1).trimmed();
                cap3 = cap.captured(3).trimmed();

                QStringList variantInfo;

                if (!cap3.isEmpty()) {
                    variantInfo = cap3.split(QStringLiteral(","));
                }

                const QString configTone = KleverConfig::emojiTone();
                QString tone = configTone == QStringLiteral("None") ? QLatin1String() : configTone;
                QString givenVariant;
                bool toneGiven = false;
                bool optionsFound = false;
                if (!variantInfo.isEmpty()) {
                    // Ex:
                    // "woman: dark skin tone, blond hair"
                    // "woman: blond hair"
                    const QString possibleTone = variantInfo[0].trimmed();
                    if (possibleTones.contains(possibleTone)) {
                        tone = possibleTone;
                        toneGiven = true;
                    } else {
                        givenVariant = possibleTone;
                    }
                    if (variantInfo.length() > 1) {
                        givenVariant = variantInfo[1].trimmed();
                    }
                }
                const bool defaultToneGiven = tone == defaultToneStr;

                QString uniEmoji;
                const QString searchTerm = givenVariant.isEmpty() ? cap1 : (cap1 + QStringLiteral(": ") + givenVariant);

                if (!tone.isEmpty() && !defaultToneGiven) {
                    const QVariantList tonedEmoji = emojiModel->tones(cap1);
                    for (auto it = tonedEmoji.begin(); it != tonedEmoji.end(); it++) {
                        const Emoji currentEmoji = it->value<Emoji>();
                        const QString emojiName = currentEmoji.shortName;
                        if (emojiName.contains(tone)) {
                            uniEmoji = currentEmoji.unicode;

                            if (givenVariant.isEmpty()) { // only looking for tone
                                if (toneGiven) {
                                    optionsFound = true;
                                }
                                break;
                            }
                            // looking for tone + variant
                            if (emojiName.endsWith(givenVariant)) {
                                optionsFound = true;
                                break;
                            }
                        }
                    }
                } else {
                    const QVariantList possibleEmojis = emojiModel->filterModelNoCustom(searchTerm);
                    for (auto it = possibleEmojis.begin(); it != possibleEmojis.end(); it++) {
                        const Emoji currentEmoji = it->value<Emoji>();
                        if (currentEmoji.shortName == searchTerm) {
                            uniEmoji = currentEmoji.unicode;
                            if (!givenVariant.isEmpty() || defaultToneGiven) {
                                optionsFound = true;
                            }
                            break;
                        }
                    }
                }

                if (optionsFound) {
                    src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);
                } else {
                    static const QString surroundingStr = QStringLiteral("::");
                    src.replace(cap.capturedStart(), cap1.length() + surroundingStr.length(), emptyStr);
                }

                outputed = uniEmoji.isEmpty() ? output(cap2) : uniEmoji;

                out += Renderer::text(outputed);
                continue;
            }
        }

        // text
        cap = inline_text.match(src);
        if (cap.hasMatch()) {
            src.replace(cap.capturedStart(), cap.capturedLength(), emptyStr);

            cap0 = cap.captured(0);
            const QString escaped = Renderer::escape(cap0, false);

            out += (useInlineText) ? escaped : Renderer::text(escaped);
            continue;
        }

        if (!src.isEmpty()) {
            qFatal("Infinite loop on byte: %d", src[0].unicode());
        }
    }

    return out;
};

QString InlineLexer::mangle(const QString &text) const
{
    QString out = QLatin1String();
    const int l = text.length();

    for (int i = 0; i < l; i++) {
        const QChar ch = text.at(i);
        if (QRandomGenerator::global()->generate() % 2 == 0) {
            out += QStringLiteral("x") + QString::number(ch.unicode(), 16);
        } else {
            out += QStringLiteral("&#") + QString::number(ch.unicode()) + QStringLiteral(";");
        }
    }

    return out;
}

QString InlineLexer::outputLink(QRegularExpressionMatch &cap, QMap<QString, QString> linkInfo, bool useInlineText)
{
    QString href = linkInfo[QStringLiteral("href")];
    QString title = linkInfo[QStringLiteral("title")];
    title = !title.isEmpty() ? Renderer::escape(title, false) : QLatin1String();

    // KLEVERNOTES ADDED THOSE LINES
    // ======
    if (href.startsWith(QStringLiteral("./")))
        href = m_parser->getNotePath() + href.mid(1);
    if (href.startsWith(QStringLiteral("~")))
        href = QDir::homePath() + href.mid(1);
    if (!(href.startsWith(QStringLiteral("http")) || href.startsWith(QStringLiteral("//")) || href.startsWith(QStringLiteral("qrc:"))))
        href = QStringLiteral("file:") + href;
    // ======
    QString out;
    QString cap1 = cap.captured(1);
    if (cap.captured(0).at(0) != QChar::fromLatin1('!')) {
        const QString outputed = output(cap1);
        out = Renderer::link(href, title, outputed);
    } else {
        const QString escaped = Renderer::escape(cap1, false);
        out = (useInlineText) ? escaped : Renderer::image(href, title, escaped);
    }

    return out;
}

QString InlineLexer::escapes(QString &text) const
{
    const static QRegularExpression escapesReg(QStringLiteral("\\\\([!\"#$%&'()*+,\\-.\\/:;<=>?@\\[\\]\\^_`{|}~])"));

    return !text.isEmpty() ? text.replace(escapesReg, QStringLiteral("\\1")) : text;
}
