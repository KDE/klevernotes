<!--
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
-->

# Klevernotes 

KleverNotes is a note taking and management application for your mobile and desktop devices. It uses markdown and allow you to preview your content. 

Different Markdown flavors are supported thanks to [Marked.js](https://marked.js.org/#specifications) (more features will come). 

**Get involved and join our Matrix channel: [klevernotes:kde.org](https://matrix.to/#/#klevernotes:kde.org)**

___

## Screenshots

### Adaptative
![Screenshot of Klevernotes main page on desktop](https://cdn.kde.org/screenshots/klevernotes/main_note_desktop.png)
![Screenshot of Klevernotes main page on mobile](https://cdn.kde.org/screenshots/klevernotes/main_note_mobile.png)
### Todos for each note
![Screenshot of Klevernotes todo](https://cdn.kde.org/screenshots/klevernotes/todo_mobile.png)
### Draw your thoughts ! 
![Screenshot of  Klevernotes drawing](https://cdn.kde.org/screenshots/klevernotes/painting.png)
### Print your note ! 
![Screenshot of  Klevernotes print page](https://cdn.kde.org/screenshots/klevernotes/printing.png)

___

## Plugins
> A feature that goes beyond the normal scope of Markdown. 
> It is considered a 'plugin' and can be enabled/disabled at any time in the settings. 
> A plugin is disabled by default.

### Syntax highlighting
Highlight your blocks of code !

#### Supported highlighter:
- [KSyntaxHighlighter](https://invent.kde.org/frameworks/syntax-highlighting) 
- [Chroma](https://github.com/alecthomas/chroma)
- [Pygments](https://pygments.org/)

___

## Get it

Klevernotes is currently in [KDE incubator](https://community.kde.org/Incubator) 

You'll need to build the app to test it ! 

## Build

**[kdesrc-build](https://invent.kde.org/sdk/kdesrc-build)** would be the best option to build klevernotes and its dependencies easily.

Arch Linux dependencies:
```
base-devel extra-cmake-modules cmake qt5-base qt5-declarative qt5-quickcontrols2 kirigami2 ki18n kcoreaddons breeze kio qt5-webengine extra-cmake-modules
```
