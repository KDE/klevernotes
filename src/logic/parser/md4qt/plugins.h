/*
    SPDX-FileCopyrightText: 2022-2025 Igor Mironchik <igor.mironchik@gmail.com>
    SPDX-License-Identifier: MIT
*/

#ifndef MD4QT_MD_PLUGINS_H_INCLUDED
#define MD4QT_MD_PLUGINS_H_INCLUDED

// md4qt include.
#include "parser.h"
#include "utils.h"

// C++ include.
#include <vector>

namespace MD
{

/*!
 * \namespace MD::EmphasisPlugin
 * \inmodule md4qt
 * \inheaderfile md4qt/plugins.h
 *
 * \brief Namespace for emphasises plugin template.
 *
 * Namespace for emphasises plugin template.
 */
namespace EmphasisPlugin
{

namespace details
{

struct OpenerInfo {
    long long int m_index = -1;
    long long int m_startColumn = -1;
    long long int m_endColumn = -1;
    long long int m_line = -1;
    bool m_bothFlanking = false;
};

inline bool operator==(const OpenerInfo &o1, const OpenerInfo &o2)
{
    return (o1.m_line == o2.m_line && o1.m_startColumn == o2.m_startColumn && o1.m_endColumn == o2.m_endColumn);
}

using OpenersStack = std::vector<OpenerInfo>;

template<class Trait>
inline long long int findRawTextIndex(std::shared_ptr<Paragraph<Trait>> p, long long int index)
{
    long long int res = -1;

    for (auto it = p->items().cbegin(), last = p->items().cend(); it != last; ++it) {
        --index;

        if ((*it)->type() == ItemType::Text) {
            ++res;
        }

        if (index < 0) {
            break;
        }
    }

    return (res >= 0 ? res : 0);
}

template<class Trait>
inline long long int
findKnownOpener(std::shared_ptr<Paragraph<Trait>> p, TextParsingOpts<Trait> &po, OpenersStack &knownOpeners, OpenersStack &openers, long long int openerIndex)
{
    const auto &o = openers[openerIndex];
    const auto virginLine = po.m_fr.m_data[o.m_line].second.m_lineNumber;
    const auto virginPos = po.m_fr.m_data[o.m_line].first.virginPos(0) + po.m_rawTextData[findRawTextIndex(p, o.m_index)].m_pos + o.m_endColumn;

    for (long long int i = 0; i < static_cast<long long int>(knownOpeners.size()); ++i) {
        if ((knownOpeners[i].m_line == virginLine && knownOpeners[i].m_startColumn > virginPos) || knownOpeners[i].m_line > virginLine) {
            return i;
        }
    }

    return -1;
}

template<class Trait>
inline void clearStyleValue(std::shared_ptr<Paragraph<Trait>> p, int styleValue, long long int startIndex, long long int endIndex)
{
    long long int counter = 0;
    bool first = true;

    const auto collectOpenDelims = [&counter, &first, &styleValue](ItemWithOpts<Trait> *item) {
        auto it = item->openStyles().cbegin();

        while (it != item->openStyles().cend()) {
            it = std::find_if(it, item->openStyles().cend(), [&styleValue](const auto &s) -> bool {
                return s.style() == styleValue;
            });

            if (it != item->openStyles().cend()) {
                if (!first) {
                    ++counter;
                } else {
                    first = false;
                }

                it = std::next(it);
            }
        }
    };

    const auto collectCloseDelims = [&counter, &styleValue](ItemWithOpts<Trait> *item) {
        auto it = item->closeStyles().cbegin();

        while (it != item->closeStyles().cend()) {
            it = std::find_if(it, item->closeStyles().cend(), [&styleValue](const auto &s) -> bool {
                return s.style() == styleValue;
            });

            if (it != item->closeStyles().cend()) {
                --counter;

                it = std::next(it);
            }
        }
    };

    for (long long int i = startIndex; i < endIndex; ++i) {
        auto itemWithOpts = static_cast<ItemWithOpts<Trait> *>(p->items().at(i).get());

        collectOpenDelims(itemWithOpts);

        if (counter == 0) {
            itemWithOpts->setOpts(itemWithOpts->opts() & ~styleValue);
        }

        collectCloseDelims(itemWithOpts);
    }
}

template<class Trait>
inline void setStyleValue(std::shared_ptr<Paragraph<Trait>> p, int styleValue, long long int startIndex, long long int endIndex)
{
    for (long long int i = startIndex; i < endIndex; ++i) {
        auto itemWithOpts = static_cast<ItemWithOpts<Trait> *>(p->items().at(i).get());
        itemWithOpts->setOpts(itemWithOpts->opts() | styleValue);
    }
}

template<class Trait>
inline void dropCloser(std::shared_ptr<Paragraph<Trait>> p, TextParsingOpts<Trait> &po, const OpenerInfo &info)
{
    const auto addTextItem = [&po, &p](const StyleDelim &info, const typename Trait::String &style, long long int idx) -> std::shared_ptr<Text<Trait>> {
        auto text = std::make_shared<Text<Trait>>();
        text->setStartColumn(info.startColumn());
        text->setStartLine(info.startLine());
        text->setEndColumn(info.endColumn());
        text->setEndLine(info.endLine());
        text->setText(style);

        p->insertItem(idx, text);
        const auto pos = localPosFromVirgin(po.m_fr, info.startColumn(), info.startLine());
        po.m_rawTextData.insert(po.m_rawTextData.begin() + findRawTextIndex(p, idx), {style, pos.first, pos.second});

        return text;
    };

    const auto skipSameDelims =
        [&po](typename ItemWithOpts<Trait>::Styles &styles, const typename Trait::String &style, StyleDelim &info, long long int startIdx) {
            bool skip = true;

            if (!styles.empty()) {
                for (long long int i = startIdx; i < static_cast<long long int>(styles.size()); ++i) {
                    const auto text = virginSubstr(po.m_fr, styles[i]);

                    if (style[0] != text[0]) {
                        skip = false;

                        break;
                    }
                }

                if (skip) {
                    for (long long int i = startIdx; i < static_cast<long long int>(styles.size()); ++i) {
                        styles[i].setStartColumn(styles[i].startColumn() - style.length());
                        styles[i].setEndColumn(styles[i].endColumn() - style.length());
                    }

                    info.setStartColumn(styles.back().endColumn() + 1);
                    info.setEndColumn(info.startColumn() + style.length() - 1);
                }
            }

            return skip;
        };

    long long int textCount = 0;
    long long int openersCount = 0;

    for (long long int i = info.m_index; i < p->items().size(); ++i) {
        auto itemWithOpts = static_cast<ItemWithOpts<Trait> *>(p->items().at(i).get());

        if (itemWithOpts->type() == ItemType::Text) {
            ++textCount;
        }

        if (i != info.m_index) {
            openersCount += itemWithOpts->openStyles().size();
        }

        if (!itemWithOpts->closeStyles().empty() && itemWithOpts->closeStyles().size() > openersCount) {
            auto closeInfo = itemWithOpts->closeStyles()[openersCount];
            const auto style = virginSubstr(po.m_fr, closeInfo);
            const auto styleValue = closeInfo.style();

            itemWithOpts->closeStyles().erase(itemWithOpts->closeStyles().begin() + openersCount);

            const auto skipped = skipSameDelims(itemWithOpts->closeStyles(), style, closeInfo, openersCount);

            if (itemWithOpts->type() == ItemType::Text) {
                auto text = static_cast<Text<Trait> *>(itemWithOpts);

                if (!skipped && openersCount == 0) {
                    text->setEndColumn(closeInfo.endColumn());
                    text->setText(text->text() + style);
                    po.m_rawTextData[findRawTextIndex(p, i)].m_str.append(style);
                } else {
                    addTextItem(closeInfo, style, i + 1);
                }
            }

            clearStyleValue(p, styleValue, info.m_index, i + 1);

            return;
        } else {
            openersCount -= itemWithOpts->closeStyles().size();
        }
    }
}

template<class Trait>
inline void dropKnownOpener(std::shared_ptr<Paragraph<Trait>> p,
                            TextParsingOpts<Trait> &po,
                            const OpenerInfo &info,
                            long long int &index,
                            long long int &rawTextIndex,
                            long long int &closingDelimPos)
{
    dropCloser(p, po, info);

    auto itemWithOpts = static_cast<ItemWithOpts<Trait> *>(p->items().at(info.m_index).get());
    const auto openInfo = itemWithOpts->openStyles().back();
    const auto style = virginSubstr(po.m_fr, openInfo);

    if (itemWithOpts->type() == ItemType::Text) {
        auto text = static_cast<Text<Trait> *>(itemWithOpts);

        text->openStyles().pop_back();
        text->setStartColumn(info.m_startColumn);
        text->setText(style + text->text());
        const auto idx = findRawTextIndex(p, info.m_index);
        po.m_rawTextData[idx].m_str = style + po.m_rawTextData[idx].m_str;
        po.m_rawTextData[idx].m_pos -= style.length();

        if (info.m_index == index) {
            closingDelimPos += style.length();
        }
    } else {
        auto text = std::make_shared<Text<Trait>>();
        text->setStartColumn(openInfo.startColumn());
        text->setStartLine(openInfo.startLine());
        text->setEndColumn(openInfo.endColumn());
        text->setEndLine(openInfo.endLine());
        text->setText(style);
        auto openStyles = itemWithOpts->openStyles();
        openStyles.pop_back();
        text->openStyles() = openStyles;
        itemWithOpts->openStyles() = {};
        p->insertItem(info.m_index, text);
        const auto pos = localPosFromVirgin(po.m_fr, openInfo.startColumn(), openInfo.startLine());
        po.m_rawTextData.insert(po.m_rawTextData.begin() + findRawTextIndex(p, info.m_index), {style, pos.first, pos.second});
        ++index;
        ++rawTextIndex;
    }

    clearStyleValue(p, openInfo.style(), info.m_index, info.m_index + 1);
}

template<class Trait>
inline void dropKnownOpeners(std::shared_ptr<Paragraph<Trait>> p,
                             TextParsingOpts<Trait> &po,
                             long long int &index,
                             OpenersStack &knownOpeners,
                             long long int &rawTextIndex,
                             long long int knownOpenerIndex,
                             long long int &closingDelimPos)
{
    for (long long int i = knownOpeners.size() - 1; i >= knownOpenerIndex; --i) {
        dropKnownOpener(p, po, knownOpeners[i], index, rawTextIndex, closingDelimPos);
    }

    knownOpeners.erase(knownOpeners.begin() + knownOpenerIndex, knownOpeners.end());
}

template<class Trait>
inline long long int addOpenStyleDelimiter(std::shared_ptr<Paragraph<Trait>> p,
                                           TextParsingOpts<Trait> &po,
                                           long long int &index,
                                           OpenersStack &openers,
                                           int style,
                                           long long int &rawTextIndex,
                                           long long int count,
                                           long long int openerIndex,
                                           long long int &closingDelimPos)
{
    const auto addOpenDelimiters = [&count, &style](ItemWithOpts<Trait> *was, ItemWithOpts<Trait> *where, long long int virginPos, bool append) {
        for (long long int i = 0; i < count; ++i) {
            if (append) {
                where->openStyles().push_back({style, virginPos, was->startLine(), virginPos, was->startLine()});
            } else {
                where->openStyles().insert(where->openStyles().begin() + i, {style, virginPos, was->startLine(), virginPos, was->startLine()});
            }
            ++virginPos;
        }
    };

    const auto &open = openers[openerIndex];

    const auto width = open.m_endColumn - open.m_startColumn + 1;
    const auto pos = (width > count ? open.m_startColumn + width - count : open.m_startColumn);
    const auto rawIdx = findRawTextIndex(p, open.m_index);
    const auto rawText = po.m_rawTextData[rawIdx];

    auto oldTextItem = static_cast<Text<Trait> *>(p->items().at(open.m_index).get());

    auto retIndex = open.m_index;

    typename ItemWithOpts<Trait>::Styles closeStyles;

    if (pos > 0) {
        po.m_rawTextData[rawIdx].m_str = rawText.m_str.sliced(0, pos);
        oldTextItem->setText(removeBackslashes<Trait>(replaceEntity<Trait>(po.m_rawTextData[rawIdx].m_str)));
        oldTextItem->setEndColumn(oldTextItem->startColumn() + pos - 1);

        if (pos + count < rawText.m_str.length()) {
            std::swap(closeStyles, oldTextItem->closeStyles());
        }

        if (index == open.m_index) {
            closingDelimPos -= pos;
        }
    }

    if (pos + count < rawText.m_str.length()) {
        if (index == open.m_index) {
            closingDelimPos -= count;
        }

        if (pos > 0) {
            auto text = std::make_shared<Text<Trait>>();
            auto virginPos = oldTextItem->startColumn() + pos;
            text->setStartColumn(virginPos + count);
            text->setStartLine(oldTextItem->startLine());
            const auto tmp = rawText.m_str.sliced(pos + count);
            text->setEndColumn(virginPos + count + tmp.length() - 1);
            text->setEndLine(oldTextItem->endLine());
            text->setText(removeBackslashes<Trait>(replaceEntity<Trait>(tmp)));
            text->setOpts(oldTextItem->opts());
            text->closeStyles() = closeStyles;

            addOpenDelimiters(oldTextItem, text.get(), virginPos, true);

            ++index;
            ++rawTextIndex;
            ++retIndex;

            p->insertItem(open.m_index + 1, text);
            po.m_rawTextData.insert(po.m_rawTextData.begin() + rawIdx + 1, {tmp, rawText.m_pos + pos + count, rawText.m_line});
        } else {
            po.m_rawTextData[rawIdx].m_str = rawText.m_str.sliced(count);
            po.m_rawTextData[rawIdx].m_pos += count;
            oldTextItem->setText(removeBackslashes<Trait>(replaceEntity<Trait>(po.m_rawTextData[rawIdx].m_str)));

            addOpenDelimiters(oldTextItem, oldTextItem, oldTextItem->startColumn(), true);

            oldTextItem->setStartColumn(oldTextItem->startColumn() + count);
        }
    } else {
        addOpenDelimiters(oldTextItem, static_cast<ItemWithOpts<Trait> *>(p->items().at(open.m_index + 1).get()), oldTextItem->startColumn(), false);

        retIndex = open.m_index + (pos == 0 ? 0 : 1);

        if (pos == 0) {
            p->removeItemAt(open.m_index);
            po.m_rawTextData.erase(po.m_rawTextData.begin() + rawIdx);

            --index;
            --rawTextIndex;
        }
    }

    return retIndex;
}

template<class Trait>
inline long long int addCloseStyleDelimiter(std::shared_ptr<Paragraph<Trait>> p,
                                            TextParsingOpts<Trait> &po,
                                            long long int &index,
                                            int style,
                                            long long int &rawTextIndex,
                                            long long int start,
                                            long long int count)
{
    const auto addCloseDelims = [&start, &count, &style](ItemWithOpts<Trait> *was, ItemWithOpts<Trait> *become, bool prepend) {
        auto virginPos = was->startColumn() + start;

        for (long long int i = 0; i < count; ++i) {
            if (prepend) {
                become->closeStyles().insert(become->closeStyles().begin() + i, {style, virginPos, was->startLine(), virginPos, was->startLine()});
            } else {
                become->closeStyles().push_back({style, virginPos, was->startLine(), virginPos, was->startLine()});
            }
            ++virginPos;
        }
    };

    const auto rawText = po.m_rawTextData[rawTextIndex];

    auto oldTextItem = static_cast<Text<Trait> *>(p->items().at(index).get());

    long long int retIndex = index;

    typename ItemWithOpts<Trait>::Styles closeStyles;

    if (start > 0) {
        po.m_rawTextData[rawTextIndex].m_str = rawText.m_str.sliced(0, start);
        oldTextItem->setText(removeBackslashes<Trait>(replaceEntity<Trait>(po.m_rawTextData[rawTextIndex].m_str)));
        oldTextItem->setEndColumn(oldTextItem->startColumn() + start - 1);

        if (start + count < rawText.m_str.length()) {
            std::swap(closeStyles, oldTextItem->closeStyles());
        }

        addCloseDelims(oldTextItem, oldTextItem, true);
    } else {
        addCloseDelims(oldTextItem, static_cast<ItemWithOpts<Trait> *>(p->items().at(index - 1).get()), false);

        retIndex = index - 1;
    }

    if (start + count < rawText.m_str.length()) {
        if (start > 0) {
            auto text = std::make_shared<Text<Trait>>();
            auto virginPos = oldTextItem->startColumn() + start + count;
            text->setStartColumn(virginPos);
            text->setStartLine(oldTextItem->startLine());
            const auto tmp = rawText.m_str.sliced(start + count);
            text->setEndColumn(virginPos + tmp.length() - 1);
            text->setEndLine(oldTextItem->endLine());
            text->setText(removeBackslashes<Trait>(replaceEntity<Trait>(tmp)));
            text->setOpts(oldTextItem->opts());
            text->closeStyles() = closeStyles;

            p->insertItem(index + 1, text);
            po.m_rawTextData.insert(po.m_rawTextData.begin() + rawTextIndex + 1, {tmp, rawText.m_pos + start + count, rawText.m_line});
        } else {
            po.m_rawTextData[rawTextIndex].m_str = rawText.m_str.sliced(count);
            po.m_rawTextData[rawTextIndex].m_pos += count;
            oldTextItem->setText(removeBackslashes<Trait>(replaceEntity<Trait>(po.m_rawTextData[rawTextIndex].m_str)));
            oldTextItem->setStartColumn(oldTextItem->startColumn() + count);
        }
    } else if (start == 0) {
        const auto itemWithOpts = static_cast<ItemWithOpts<Trait> *>(p->items().at(index).get());
        auto prevItemWithOpts = static_cast<ItemWithOpts<Trait> *>(p->items().at(index - 1).get());

        if (!itemWithOpts->closeStyles().empty()) {
            std::copy(itemWithOpts->closeStyles().cbegin(), itemWithOpts->closeStyles().cend(), std::back_inserter(prevItemWithOpts->closeStyles()));
        }

        p->removeItemAt(index);
        po.m_rawTextData.erase(po.m_rawTextData.begin() + rawTextIndex);

        --index;
        --rawTextIndex;
    }

    return retIndex;
}

template<class Trait>
inline void closeStyle(std::shared_ptr<Paragraph<Trait>> p,
                       TextParsingOpts<Trait> &po,
                       long long int &index,
                       OpenersStack &knownOpeners,
                       OpenersStack &openers,
                       int style,
                       long long int &rawTextIndex,
                       long long int start,
                       long long int count,
                       long long int openerIndex)
{
    const auto knownOpenerIndex = findKnownOpener(p, po, knownOpeners, openers, openerIndex);

    if (knownOpenerIndex != -1) {
        dropKnownOpeners(p, po, index, knownOpeners, rawTextIndex, knownOpenerIndex, start);
    }

    const auto startIdx = addOpenStyleDelimiter(p, po, index, openers, style, rawTextIndex, count, openerIndex, start);

    const auto endIdx = addCloseStyleDelimiter(p, po, index, style, rawTextIndex, start, count);

    setStyleValue(p, style, startIdx, endIdx + 1);

    openers.erase(openers.cbegin() + openerIndex);
}

template<class Trait>
inline bool tryCloseStyle(std::shared_ptr<Paragraph<Trait>> p,
                          TextParsingOpts<Trait> &po,
                          long long int &index,
                          OpenersStack &knownOpeners,
                          OpenersStack &openers,
                          int style,
                          long long int &rawTextIndex,
                          long long int start,
                          long long int count,
                          long long int &used,
                          bool isBoth)
{
    for (long long int i = openers.size() - 1; i >= 0; --i) {
        const auto openerSize = openers[i].m_endColumn - openers[i].m_startColumn + 1;

        if (!((isBoth || openers[i].m_bothFlanking) && isMult3(openerSize, count))) {
            used = std::min(openerSize, count);

            closeStyle(p, po, index, knownOpeners, openers, style, rawTextIndex, start, used, i);

            return true;
        }
    }

    return false;
}

template<class Trait>
inline long long int processEmphasisInText(std::shared_ptr<Paragraph<Trait>> p,
                                           TextParsingOpts<Trait> &po,
                                           long long int index,
                                           OpenersStack &knownOpeners,
                                           OpenersStack &openers,
                                           const typename Trait::Char &delimiter,
                                           int style,
                                           long long int &rawTextIndex,
                                           bool &finished)
{
    ReverseSolidusHandler<Trait> reverseSolidus;

    const auto &text = po.m_rawTextData[rawTextIndex];
    const auto length = text.m_str.length();

    for (long long int i = 0; i < text.m_str.length(); ++i) {
        if (reverseSolidus.process(text.m_str[i])) {
        } else if (text.m_str[i] == delimiter && !reverseSolidus.isPrevReverseSolidus()) {
            const bool punctBefore = (text.m_pos + i > 0 ? po.m_fr.m_data[text.m_line].first[text.m_pos + i - 1].isPunct()
                                              || po.m_fr.m_data[text.m_line].first[text.m_pos + i - 1].isSymbol()
                                                         : true);
            const bool uWhitespaceBefore = (text.m_pos + i > 0 ? Trait::isUnicodeWhitespace(po.m_fr.m_data[text.m_line].first[text.m_pos + i - 1]) : true);
            const bool uWhitespaceOrPunctBefore = uWhitespaceBefore || punctBefore;

            const auto start = i;
            const auto count = skipIf(i,
                                      text.m_str,
                                      [&delimiter](const typename Trait::Char &c) {
                                          return (delimiter == c);
                                      })
                - i;

            i += count;

            const bool punctAfter = (text.m_pos + i < po.m_fr.m_data[text.m_line].first.length() ? po.m_fr.m_data[text.m_line].first[text.m_pos + i].isPunct()
                                             || po.m_fr.m_data[text.m_line].first[text.m_pos + i].isSymbol()
                                                                                                 : true);
            const bool uWhitespaceAfter =
                (text.m_pos + i < po.m_fr.m_data[text.m_line].first.length() ? Trait::isUnicodeWhitespace(po.m_fr.m_data[text.m_line].first[text.m_pos + i])
                                                                             : true);

            const bool leftFlanking = !uWhitespaceAfter && (!punctAfter || (punctAfter && uWhitespaceOrPunctBefore));
            const bool rightFlanking = !uWhitespaceBefore && (!punctBefore || (punctBefore && (uWhitespaceAfter || punctAfter)));

            const auto isBoth = rightFlanking && leftFlanking;

            if (rightFlanking) {
                long long int used = -1;

                if (tryCloseStyle(p, po, index, knownOpeners, openers, style, rawTextIndex, start, count, used, isBoth)) {
                    if (i == length && used == count) {
                        finished = true;
                    }

                    return index;
                }
            }

            if (leftFlanking) {
                openers.push_back({index, start, start + count - 1, text.m_line, isBoth});
            }
        }

        reverseSolidus.next();
    }

    finished = true;

    return index;
}

template<class Trait>
inline long long int processEmphasis(std::shared_ptr<Paragraph<Trait>> p,
                                     TextParsingOpts<Trait> &po,
                                     long long int index,
                                     OpenersStack &knownOpeners,
                                     OpenersStack &openers,
                                     const typename Trait::Char &delimiter,
                                     int style,
                                     long long int &rawTextIndex)
{
    const auto populateOpeners = [&index, &knownOpeners, &rawTextIndex](const ItemWithOpts<Trait> *item) {
        for (const auto &o : item->openStyles()) {
            knownOpeners.push_back({index, o.startColumn(), o.endColumn(), o.startLine()});
        }
    };

    const auto closeOpeners = [&knownOpeners, &openers, &po, &p](const typename ItemWithOpts<Trait>::Styles &styles, const OpenerInfo &firstOpener) {
        if (!styles.empty()) {
            if (firstOpener.m_line != -1) {
                long long int openerIdx = -1;

                for (long long int i = 0; i < static_cast<long long int>(openers.size()); ++i) {
                    const auto &o = openers[i];
                    const auto virginPos = po.m_fr.m_data[o.m_line].first.virginPos(po.m_rawTextData[findRawTextIndex(p, o.m_index)].m_pos + o.m_startColumn);
                    const auto virginLine = po.m_fr.m_data[o.m_line].second.m_lineNumber;

                    if (virginLine > firstOpener.m_line || (virginLine == firstOpener.m_line && virginPos > firstOpener.m_endColumn)) {
                        openerIdx = i;

                        break;
                    }
                }

                if (openerIdx != -1) {
                    openers.erase(openers.begin() + openerIdx, openers.end());
                }
            }

            const auto it = std::find(knownOpeners.cbegin(), knownOpeners.cend(), firstOpener);

            if (it != knownOpeners.cend()) {
                knownOpeners.erase(it, knownOpeners.end());
            }
        }
    };

    const auto itemWithOpts = static_cast<ItemWithOpts<Trait> *>(p->items().at(index).get());

    switch (p->items().at(index)->type()) {
    case ItemType::Text: {
        ++rawTextIndex;

        populateOpeners(itemWithOpts);

        bool finished = false;
        const auto closeStyles = itemWithOpts->closeStyles();
        OpenerInfo firstOpener;

        if (!knownOpeners.empty() && !closeStyles.empty()) {
            firstOpener = knownOpeners[knownOpeners.size() - closeStyles.size()];
        }

        const auto idx = processEmphasisInText(p, po, index, knownOpeners, openers, delimiter, style, rawTextIndex, finished);

        if (finished) {
            closeOpeners(closeStyles, firstOpener);
        }

        return idx;
    } break;

    default: {
        populateOpeners(itemWithOpts);

        if (!knownOpeners.empty() && !itemWithOpts->closeStyles().empty()) {
            const auto firstOpener = knownOpeners[knownOpeners.size() - itemWithOpts->closeStyles().size()];
            closeOpeners(itemWithOpts->closeStyles(), firstOpener);
        }
    } break;
    }

    return index;
}

} /* namespace details */

/*!
 * \inheaderfile md4qt/plugins.h
 *
 * Template plugin for emphasises. Emphasises defined with this plugin template  behave like
 * "*" and "_" in Markdown. First string in \a opts is used to set a character that will be
 * used for new emphasis, note that only first character from first string in \a opts is
 * considered. Also keep in mind rules for left and right flanking for emphasises, to work
 * properly character for delimiter should meet the criteria defined in
 * \l {https://spec.commonmark.org/0.31.2/#emphasis-and-strong-emphasis} {CommonMark Spec}.
 *
 * \a p Paragraph.
 *
 * \a po Text parsing options.
 *
 * \a opts Options for plugin. A first string is a delimiter, note that only first character will be used.
 *         A second string is a style number, it should be power of 2,
 *         starting at MD::TextOption::StrikethroughText * 2.
 *
 * \sa MD::Parser::addTextPlugin, MD::Parser::removeTextPlugin, MD::TextPlugin
 */
template<class Trait>
inline void emphasisTemplatePlugin(std::shared_ptr<Paragraph<Trait>> p, TextParsingOpts<Trait> &po, const typename Trait::StringList &opts)
{
    if (opts.size() == 2 && !opts.front().isEmpty() && !opts.back().isEmpty()) {
        if (!po.m_collectRefLinks) {
            const auto delimiter = opts.front()[0];
            const int style = opts.back().toInt();

            long long int i = 0;

            details::OpenersStack knownOpeners;
            details::OpenersStack openers;
            long long int rawTextIndex = -1;

            for (; i < static_cast<long long int>(p->items().size()); ++i) {
                i = details::processEmphasis(p, po, i, knownOpeners, openers, delimiter, style, rawTextIndex);
            }
        }
    }
}

} /* namespace EmphasisPlugin */

//
// YAMLHeader
//

/*!
 * \class MD::YAMLHeader
 * \inmodule md4qt
 * \inheaderfile md4qt/plugins.h
 *
 * \brief YAML header item in the document.
 */
template<class Trait>
class YAMLHeader : public Item<Trait>
{
public:
    YAMLHeader() = default;
    ~YAMLHeader() override = default;

    /*!
     * Returns type of the item.
     */
    ItemType type() const override
    {
        return static_cast<ItemType>(static_cast<int>(ItemType::UserDefined) + 1);
    }

    /*!
     * Clone this item.
     *
     * \a doc Parent of new item.
     */
    std::shared_ptr<Item<Trait>> clone(Document<Trait> *doc = nullptr) const override
    {
        MD_UNUSED(doc)

        auto h = std::make_shared<YAMLHeader<Trait>>();
        h->applyPositions(*this);
        h->setYaml(yaml());
        h->setStartDelim(startDelim());
        h->setEndDelim(endDelim());

        return h;
    }

    /*!
     * Returns YAML content.
     */
    const typename Trait::String &yaml() const
    {
        return m_yaml;
    }

    /*!
     * Set YAML content.
     *
     * \a y YAML content.
     */
    void setYaml(const typename Trait::String &y)
    {
        m_yaml = y;
    }

    /*!
     * Returns start delimiter position.
     */
    const WithPosition &startDelim() const
    {
        return m_startDelim;
    }

    /*!
     * Set start delimiter position.
     *
     * \a p Position.
     */
    void setStartDelim(const WithPosition &p)
    {
        m_startDelim = p;
    }

    /*!
     * Returns end delimiter position.
     */
    const WithPosition &endDelim() const
    {
        return m_endDelim;
    }

    /*!
     * Set end delimiter position.
     *
     * \a p Position.
     */
    void setEndDelim(const WithPosition &p)
    {
        m_endDelim = p;
    }

private:
    /*!
     * YAML content.
     */
    typename Trait::String m_yaml;
    /*!
     * Start delimiter of YAML block.
     */
    WithPosition m_startDelim;
    /*!
     * End delimitier of YAML block.
     */
    WithPosition m_endDelim;
}; // class YAMLHeader

//
// YAMLBlockPlugin
//

/*!
 * \class MD::YAMLBlockPlugin
 * \inmodule md4qt
 * \inheaderfile md4qt/plugins.h
 *
 * \brief Plugin for YAML header.
 */
template<class Trait>
class YAMLBlockPlugin : public BlockPlugin<Trait>
{
public:
    YAMLBlockPlugin() = default;
    ~YAMLBlockPlugin() override = default;

    /*!
     * Returns whether the given line is a start of a block that this plugin handles.
     *
     * \note Position of \a stream is on next line after \a startLine.
     *
     * \note After checking in \a stream next lines on return from this method \a stream should be in state what it was
     *       before.
     *
     * \a startLine Start line.
     *
     * \a stream Stream.
     *
     * \a emptyLinesBefore Count of empty lines before. May be 0 even if there are empty lines before, but
     *                     in this case you will get access to full stream. In case when not full stream is
     *                     passed to this method you will get correct amount of empty lines before.
     */
    bool isItYou(typename Trait::InternalString &startLine, StringListStream<Trait> &stream, long long int emptyLinesBefore) override
    {
        m_linesCount = -1;

        const auto ns = skipSpaces(0, startLine);

        if (startLine.toString().trimmed() == s_startString && ns == 0 && startLine.virginPos(0) == 0) {
            const auto lineNumber = stream.currentLineNumber() - 1;

            if (lineNumber != 0) {
                const auto linesInStream = stream.currentStreamPos() - 1;

                if (linesInStream + emptyLinesBefore == lineNumber) {
                    for (long long int i = 0; i < linesInStream; ++i) {
                        const auto internal = stream.lineAt(i);

                        if (skipSpaces(0, internal) != internal.length()) {
                            return false;
                        }
                    }
                } else {
                    return false;
                }
            }

            auto i = stream.currentStreamPos();

            for (; i < stream.size(); ++i) {
                const auto internal = stream.lineAt(i);
                const auto str = internal.toString().trimmed();
                const auto ns = skipSpaces(0, internal);

                if ((str == s_startString || str == s_endString) && internal.virginPos(0) == 0 && ns == 0) {
                    break;
                }
            }

            if (i < stream.size()) {
                m_linesCount = i - stream.currentStreamPos() + 2;

                return true;
            }
        }

        return false;
    }

    /*!
     * Returns ID of a block.
     */
    BlockType id() const override
    {
        return static_cast<BlockType>(static_cast<int>(BlockType::UserDefined) + 1);
    }

    /*!
     * Returns count of lines in the last detected block of this type.
     */
    long long int linesCountOfLastBlock() const override
    {
        return m_linesCount;
    }

    /*!
     * Process a block.
     *
     * \a fr Fragment of a document.
     *
     * \a parent Parent where new block should be added.
     *
     * \a doc Document.
     *
     * \a collectRefLinks Indicates that this is first go through the document and we just collecting reference links.
     *                    Most of block plugins may just do nothing when this flag is true.
     */
    void process(MdBlock<Trait> &fr, std::shared_ptr<Block<Trait>> parent, std::shared_ptr<Document<Trait>> doc, bool collectRefLinks) override
    {
        MD_UNUSED(doc)

        if (!collectRefLinks) {
            typename Trait::String yaml;

            for (long long int i = 1; i < fr.m_data.size() - 1; ++i) {
                yaml.append(fr.m_data[i].first.toString());
                yaml.append(s_newLineString<Trait>);
            }

            auto h = std::make_shared<YAMLHeader<Trait>>();
            const auto startPos = fr.m_data.front().first.virginPos(0);
            h->setStartColumn(startPos);
            h->setStartLine(fr.m_data.front().second.m_lineNumber);
            const auto endPos = fr.m_data.back().first.virginPos(fr.m_data.back().first.length() - 1);
            h->setEndColumn(endPos);
            h->setEndLine(fr.m_data.back().second.m_lineNumber);

            h->setYaml(yaml);

            h->setStartDelim({startPos,
                              fr.m_data.front().second.m_lineNumber,
                              fr.m_data.front().first.virginPos(fr.m_data.front().first.length() - 1),
                              fr.m_data.front().second.m_lineNumber});
            h->setEndDelim({fr.m_data.back().first.virginPos(0), fr.m_data.back().second.m_lineNumber, endPos, fr.m_data.back().second.m_lineNumber});

            parent->appendItem(h);
        }
    }

private:
    /*!
     * Start sequence for YAML block.
     */
    static const typename Trait::String s_startString;
    /*!
     * Alternative sequnce that may be used at end of YAML block.
     */
    static const typename Trait::String s_endString;
    /*!
     * Count of lines in last detected YAML block.
     */
    long long int m_linesCount = -1;
}; // class YAMLBlockPlugin

template<class Trait>
const typename Trait::String YAMLBlockPlugin<Trait>::s_startString = Trait::latin1ToString("---");
template<class Trait>
const typename Trait::String YAMLBlockPlugin<Trait>::s_endString = Trait::latin1ToString("...");

} /* namespace MD */

#endif // MD4QT_MD_PLUGINS_H_INCLUDED
