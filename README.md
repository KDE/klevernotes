<!--
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
-->

# Klevernotes 

KleverNotes is a note taking and management application. It uses markdown and allow you to preview your content. 

Different versions of Markdown are supported thanks to the [md4qt](https://github.com/igormironchik/md4qt) parser with which the project works closely (many thanks to the author: igormironchik).
This parser allows text plugins to extend Markdown parsing, enabling the creation of KleverNotes "[plugins](https://invent.kde.org/office/klevernotes#plugins)". 

**Get involved and join our Matrix channel: [klevernotes:kde.org](https://matrix.to/#/#klevernotes:kde.org)**

___

## Screenshots

### Adaptative
![Screenshot of Klevernotes main page on desktop](https://cdn.kde.org/screenshots/klevernotes/main_note_desktop.png)
![Screenshot of Klevernotes main page on mobile](https://cdn.kde.org/screenshots/klevernotes/main_note_mobile.png)
### Todos for each note
![Screenshot of Klevernotes todo](https://cdn.kde.org/screenshots/klevernotes//todo_desktop.png)
### Draw your thoughts ! 
![Screenshot of  Klevernotes drawing](https://cdn.kde.org/screenshots/klevernotes//painting.png)

___

## Plugins
> A feature that goes beyond the normal scope of Markdown. 
> It is considered a 'plugin' and can be enabled/disabled at any time in the settings.
> A plugin is disabled by default.

<details><summary>Available plugins</summary>

### Note linking
Link your notes together. You can even link them to a specific header !

Use the following syntax to do so : 

`[[ Category/Group/Note:# header | displayed name ]]`

### Syntax highlighting
Highlight your blocks of code !

Use the following syntax to do so : 
```
    ```<lang>
        code goes here
    ```
```
Replace `<lang>` with the desired language :smile:

#### Supported highlighter:
- [KSyntaxHighlighter](https://invent.kde.org/frameworks/syntax-highlighting) 
- [Chroma](https://github.com/alecthomas/chroma)
- [Pygments](https://pygments.org/)

### Quick emoji
Quickly add emojis to your text !

Use the following syntax to do so :

`:emoji_name:`

You can also choose your preferred tone for the emoji. If a colored variant of the emoji is available, your preferred tone will be used by default.

### PlantUML
Create diagram with PlantUML.

Use the following syntax to do so :
```
    ```puml
    <PlantUML syntax>
    ```
```
or
```
    ```plantuml
    <PlantUML syntax>
    ```
```

[More info about PlantUML](https://plantuml.com/)

</details>

___

## Get it

### Install the flatpak
Klevernotes is in the process to be added on flathub.

In the meantime, a Nightly Flatpak is available through [KDE cdn](https://cdn.kde.org/flatpak/klevernotes-nightly/), just run the following command to get it:
```
flatpak install --user --or-update https://cdn.kde.org/flatpak/klevernotes-nightly/org.kde.klevernotes.flatpakref
```

Command based on [this tutorial](https://userbase.kde.org/Tutorials/Flatpak#Nightly_KDE_apps)
 

### Build

**[kdesrc-build](https://invent.kde.org/sdk/kdesrc-build)** would be the best option to build klevernotes and its dependencies easily.


### ⚠️  Note: This requires building with _QT6_, the default with kdesrc-build. ⚠️

<br>

Arch Linux dependencies:
```
base-devel extra-cmake-modules cmake qt6-base qt6-declarative qt6-quickcontrols2 qt6-webengine
```

<br>

Once kdesrc-build is correctly installed on your system:

1. Let's make sure KleverNotes will look good by running:
```
kdesrc-build kirigami kcoreaddons ki18n breeze plasma-integration qqc2-desktop-style
```

2. Let's build KleverNotes:
```
kdesrc-build klevernotes
```

3. Start it with:
```
kdesrc-run klevernotes
```
