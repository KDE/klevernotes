/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

// md4qt include.
#include <md4qt/src/emphasis_parser.h>

namespace ExtendedSyntaxMaker
{

class SubEmphasisParser : public MD::EmphasisParser
{
public:
    SubEmphasisParser();
    ~SubEmphasisParser() override = default;

    const QChar &symbol() const override;
    bool isEmphasis(int length) const override;
    bool isLengthCorrespond() const override;

    MD::ItemWithOpts::Styles openStyles(qsizetype startPos, qsizetype lineNumber, qsizetype length) const override;
    MD::ItemWithOpts::Styles closeStyles(qsizetype startPos, qsizetype lineNumber, qsizetype length) const override;

private:
    const QChar m_symbol;
}; // class SubEmphasisParser

class SupEmphasisParser : public MD::EmphasisParser
{
public:
    SupEmphasisParser();
    ~SupEmphasisParser() override = default;

    const QChar &symbol() const override;
    bool isEmphasis(int length) const override;
    bool isLengthCorrespond() const override;

    MD::ItemWithOpts::Styles openStyles(qsizetype startPos, qsizetype lineNumber, qsizetype length) const override;
    MD::ItemWithOpts::Styles closeStyles(qsizetype startPos, qsizetype lineNumber, qsizetype length) const override;

private:
    const QChar m_symbol;
}; // class SupEmphasisParser

class HighlightEmphasisParser : public MD::EmphasisParser
{
public:
    HighlightEmphasisParser();
    ~HighlightEmphasisParser() override = default;

    const QChar &symbol() const override;
    bool isEmphasis(int length) const override;
    bool isLengthCorrespond() const override;

    MD::ItemWithOpts::Styles openStyles(qsizetype startPos, qsizetype lineNumber, qsizetype length) const override;
    MD::ItemWithOpts::Styles closeStyles(qsizetype startPos, qsizetype lineNumber, qsizetype length) const override;

private:
    const QChar m_symbol;
}; // class HighlightEmphasisParser

} // ExtendedSyntaxMaker
