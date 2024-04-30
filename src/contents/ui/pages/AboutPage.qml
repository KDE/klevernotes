// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>

import QtQuick

import org.kde.kirigamiaddons.formcard as FormCard

import org.kde.Klever

FormCard.AboutPage {
    aboutData: About

    onBackRequested: {
        applicationWindow().currentPageName = "Main"
    }
}
