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

Klevernotes is currently in [KDE incubator](https://community.kde.org/Incubator) 

You'll need to build the app to test it ! 

## Build

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
