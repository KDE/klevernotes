<!--
    SPDX-License-Identifier: GPL-2.0-or-later
    SPDX-FileCopyrightText: 2022 Louis Schul <schul9louis@gmail.com>
-->

# KleverNotes


A convergent markdown note taking application.


## Dependencies
* Kirigami
* Qt Quick Controls

## Installing
1. Install the required packages, which you can find [on this page](https://develop.kde.org/docs/getting-started/kirigami/introduction-getting_started/)
2. Download or clone the repo
3. Inside the directory run :

```sh
mkdir build
cd build
/usr/bin/cmake  -Wno-dev -G Ninja ../
/usr/bin/ninja
```

`/usr/bin/cmake` is the default path to cmake, replace it by yours if it's different.

Same goes for `/usr/bin/ninja`.
