// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>

#include "toolbarUtils.hpp"

// KleverNotes includes
#include "logic/editor/editorHandler.hpp"

// Qt includes
#include <QTextBlock>

// C++ includes
#include <vector>

// Credit to: https://stackoverflow.com/a/8216059
QString rstrip(const QString &str)
{
    int n = str.size() - 1;
    for (; n >= 0; --n) {
        if (!str.at(n).isSpace()) {
            return str.left(n + 1);
        }
    }
    return {};
}

int getLastInBlockNonEmptyPos(const QTextBlock &block)
{
    return rstrip(block.text()).length();
}

int getFirstInBlockNonEmptyPos(const QTextBlock &block)
{
    return getLastInBlockNonEmptyPos(block) - block.text().trimmed().length();
}

bool isEmptyBlock(const QTextBlock &block)
{
    // This way we also avoid line that only contains white space
    return getLastInBlockNonEmptyPos(block) == 0;
}

bool nextNonEmptyBlock(QTextBlock &block, const int finalPos)
{
    block = block.next();
    while (block.isValid() && isEmptyBlock(block) && !block.contains(finalPos)) {
        block = block.next();
    }
    return block.isValid();
}

std::vector<int> getWrappedPositions(const MdEditor::EditorHandler *editor)
{
    auto cursor = editor->textCursor();
    if (cursor.position() != editor->selectionStart()) {
        cursor.setPosition(editor->selectionStart());
    }

    const int blockStart = cursor.block().position();

    cursor.setPosition(editor->selectionEnd());

    const int blockEnd = cursor.block().position() + cursor.block().length() - 1;

    return {blockStart, blockEnd};
}

std::vector<int> getByBlockPositions(const MdEditor::EditorHandler *editor, const bool isBlockItemDelim)
{
    if (!isBlockItemDelim && editor->selectionStart() == editor->selectionEnd()) {
        return {};
    }

    auto cursor = editor->textCursor();
    if (cursor.position() != editor->selectionStart()) {
        cursor.setPosition(editor->selectionStart());
    }
    auto block = cursor.block();

    std::vector<int> positions;
    QTextBlock lastNonEmptyBlock;
    if (!isEmptyBlock(block)) {
        lastNonEmptyBlock = block;
        positions.push_back(isBlockItemDelim ? block.position() : editor->selectionStart());
    }

    int currentBlockNum = block.blockNumber();
    if (!block.contains(editor->selectionEnd())) {
        while (nextNonEmptyBlock(block, editor->selectionEnd()) && !block.contains(editor->selectionEnd())) {
            if (isBlockItemDelim) {
                positions.push_back(block.position());
            } else if (1 < block.blockNumber() - currentBlockNum) { // Blank line
                // Avoid the white space at the start and the end
                if (lastNonEmptyBlock.isValid()) {
                    positions.push_back(lastNonEmptyBlock.position() + getLastInBlockNonEmptyPos(lastNonEmptyBlock));
                }
                positions.push_back(block.position() + getFirstInBlockNonEmptyPos(block));
            }

            lastNonEmptyBlock = block;
            currentBlockNum = block.blockNumber();
        }
    }

    if ((!block.isValid() || isEmptyBlock(block)) && !lastNonEmptyBlock.isValid()) {
        return {};
    } else if (!block.isValid() && !isBlockItemDelim) {
        block = lastNonEmptyBlock;
    } else if (!block.isValid()) {
        return positions;
    }

    if (!isBlockItemDelim && 1 < block.blockNumber() - currentBlockNum) { // Blank line
        if (lastNonEmptyBlock.isValid()) {
            positions.push_back(lastNonEmptyBlock.position() + lastNonEmptyBlock.length() - 1);
        }
        positions.push_back(block.position());
    }

    positions.push_back(isBlockItemDelim ? block.position() : editor->selectionEnd());
    return positions;
}

std::vector<int> getFuturDelimsPositions(const MdEditor::EditorHandler *editor, const bool wrapSelection = false, const bool isBlockItemDelim = false)
{
    return wrapSelection ? getWrappedPositions(editor) : getByBlockPositions(editor, isBlockItemDelim);
}

namespace toolbarUtils
{
void removeDelims(const MdEditor::EditorHandler *editor, const int delimType)
{
    std::vector<MD::WithPosition> toRemove;
    for (const auto &delimInfo : editor->getSurroundingDelims()) {
        if (delimInfo.delimType == delimType) {
            toRemove.push_back(delimInfo.opening);
            if (delimInfo.closing.startLine() != -1) {
                toRemove.push_back(delimInfo.closing);
            }
        }
    }

    std::sort(toRemove.begin(), toRemove.end(), md4qtHelperFunc::StartColumnOrder{});

    QTextCursor cursor = editor->textCursor();

    cursor.beginEditBlock();
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it) {
        const QTextBlock line = editor->document()->findBlockByNumber(it->startLine());
        const int startPos = line.position() + it->startColumn();
        const int endPos = line.position() + it->endColumn() + 1;

        cursor.setPosition(startPos, QTextCursor::MoveAnchor);
        cursor.setPosition(endPos, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
    cursor.endEditBlock();
}

bool addDelims(const MdEditor::EditorHandler *editor, const int delimType)
{
    bool increment = false;
    QString delimText;
    std::vector<int> delimsPos;
    switch (delimType) {
    case posCacheUtils::BlockDelimTypes::Heading1:
        delimsPos = getFuturDelimsPositions(editor, false, true);
        delimText = QStringLiteral("# ");
        break;
    case posCacheUtils::BlockDelimTypes::Heading2:
        delimsPos = getFuturDelimsPositions(editor, false, true);
        delimText = QStringLiteral("## ");
        break;
    case posCacheUtils::BlockDelimTypes::Heading3:
        delimsPos = getFuturDelimsPositions(editor, false, true);
        delimText = QStringLiteral("### ");
        break;
    case posCacheUtils::BlockDelimTypes::Heading4:
        delimsPos = getFuturDelimsPositions(editor, false, true);
        delimText = QStringLiteral("#### ");
        break;
    case posCacheUtils::BlockDelimTypes::Heading5:
        delimsPos = getFuturDelimsPositions(editor, false, true);
        delimText = QStringLiteral("##### ");
        break;
    case posCacheUtils::BlockDelimTypes::Heading6:
        delimsPos = getFuturDelimsPositions(editor, false, true);
        delimText = QStringLiteral("###### ");
        break;
    case posCacheUtils::BlockDelimTypes::CodeBlock:
        delimText = QStringLiteral("\n```\n");
        delimsPos = getFuturDelimsPositions(editor, true);
        break;
    case posCacheUtils::BlockDelimTypes::BlockQuote:
        delimText = QStringLiteral("> ");
        delimsPos = getFuturDelimsPositions(editor, false, true);
        break;
    case posCacheUtils::BlockDelimTypes::OrderedList:
        increment = true;
        delimText = QStringLiteral(". ");
        delimsPos = getFuturDelimsPositions(editor, false, true);
        break;
    case posCacheUtils::BlockDelimTypes::UnorderedList:
        delimText = QStringLiteral("- ");
        delimsPos = getFuturDelimsPositions(editor, false, true);
        break;
    case MD::TextOption::BoldText:
        delimText = QStringLiteral("**");
        delimsPos = getFuturDelimsPositions(editor);
        break;
    case MD::TextOption::ItalicText:
        delimText = QStringLiteral("*");
        delimsPos = getFuturDelimsPositions(editor);
        break;
    case MD::TextOption::StrikethroughText:
        delimText = QStringLiteral("~~");
        delimsPos = getFuturDelimsPositions(editor);
        break;
    case HighlightDelim:
        delimText = QStringLiteral("==");
        delimsPos = getFuturDelimsPositions(editor);
        break;
    }

    if (delimsPos.empty()) {
        return false;
    }

    auto cursor = editor->textCursor();
    cursor.beginEditBlock();
    int pos = delimsPos.size();
    for (auto it = delimsPos.rbegin(); it != delimsPos.rend(); ++it) {
        cursor.setPosition(*it);
        cursor.insertText(increment ? QString::number(pos) + delimText : delimText);
        --pos;
    }
    cursor.endEditBlock();

    return true;
}
}
