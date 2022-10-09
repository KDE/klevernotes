// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

#include "about.h"

KAboutData AboutType::aboutData() const
{
    return KAboutData::applicationData();
}
