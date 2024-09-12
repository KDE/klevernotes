/*
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2024 Louis Schul <schul9louis@gmail.com>
*/

#pragma once

// md-editor include.
#define MD4QT_QT_SUPPORT
#include "logic/parser/md4qt/doc.hpp"
#include "logic/parser/md4qt/traits.hpp"

using ItemSharedPointer = std::shared_ptr<MD::Item<MD::QStringTrait>>;
using SharedItems = typename MD::QStringTrait::template Vector<ItemSharedPointer>;
using Items = typename MD::QStringTrait::template Vector<MD::Item<MD::QStringTrait> *>;

namespace posCacheUtils
{
struct DelimsInfo {
    int headingLevel = 0;
    MD::WithPosition opening = {};
    MD::WithPosition closing = {};
};
bool operator==(const DelimsInfo &d1, const DelimsInfo &d2);

void addDelimsFromItems(QList<DelimsInfo> &delims,
                        const Items &items,
                        const MD::WithPosition &pos,
                        const MD::WithPosition &selectStartPos = {},
                        const MD::WithPosition &selectEndPos = {});
}
