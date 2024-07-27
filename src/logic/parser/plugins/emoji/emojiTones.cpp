// SPDX-FileCopyrightText: None
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "emojiTones.h"
#include "emojiModel.h"

QMultiHash<QString, QVariant> EmojiTones::_tones = {
#include "emojiTones_data.h"
};
