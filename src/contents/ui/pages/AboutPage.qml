// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick 2.15

import org.kde.kirigamiaddons.formcard 1.0 as FormCard

import org.kde.Klever 1.0

FormCard.AboutPage {
    aboutData: About

    onBackRequested: {
        applicationWindow().currentPageName = "Main"
    }
}
