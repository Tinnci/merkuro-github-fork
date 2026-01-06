<!--
SPDX-License-Identifier: GPL-3.0-only
-->

# Personal Calendar

A personal calendar application built on the Merkuro codebase. This is an independent fork utilizing the original Merkuro code as a foundation for a personal calendar management project.

**This is a personal project** - not affiliated with the original Merkuro/Kalendar project.

## Screenshots

![Screenshot of Kalendar's month view](https://cdn.kde.org/screenshots/kalendar/month_view.png)
![Screenshot of Kalendar's task view](https://cdn.kde.org/screenshots/kalendar/task_view.png)
![Screenshot of Kalendar's week view](https://cdn.kde.org/screenshots/kalendar/week_view.png)
![Screenshot of Kalendar's schedule view](https://cdn.kde.org/screenshots/kalendar/schedule_view.png)
![Screenshot of Kalendar's schedule view on mobile](https://cdn.kde.org/screenshots/kalendar/mobile_view.png)

## Get it

Kalendar is available in the Arch AUR and with openSUSE tumbleweed using
the following two OBS repositories: https://build.opensuse.org/project/show/home:KaratekHD:kirigami
and https://build.opensuse.org/project/show/home:andresbs:plasma-mobile

Please note that this is pre-release software and that you may encounter bugs, crashes, or errors.

## Build

**Kalendar requires KFrameworks 5.86 to be installed.** This package version is fairly recent and may not yet be available in your distribution of choice, meaning Kalendar might not work.

**We also strongly recommend you install the `kdepim-runtime` package before starting Kalendar** -- this will provide you with the ability to add calendars from online resources. Having this package will also let Kalendar's backend automatically create a default local calendar. 

If you have already installed and started Kalendar and are now installing `kdepim-runtime`, make sure to run `akonadictl restart`; this will enable online resources and the local calendar after installing `kdepim-runtime`.

KDE Neon dependencies:
```
git cmake build-essential gettext extra-cmake-modules qtbase5-dev qtdeclarative5-dev libqt5svg5-dev qtquickcontrols2-5-dev qml-module-org-kde-kirigami2 kirigami2-dev libkf5i18n-dev gettext libkf5coreaddons-dev qml-module-qtquick-layouts qml-module-qtlocation qml-module-qt-labs-qmlmodels qtlocation5-dev qml-module-qtpositioning qtpositioning5-dev libkf5mime-dev libkf5calendarsupport-dev libkf5akonadicontact-dev libkf5akonadi-dev libkf5windowsystem-dev libkf5package-dev libkf5calendarcore-dev libkf5configwidgets-dev libkf5contacts-dev libkf5people-dev libkf5eventviews-dev kdepim-runtime ninja-build
```

```
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=~/.local/kde -GNinja
ninja
```

## License

This project is licensed under **GPL-3.0-only**. This is a stricter license than the original project to ensure the code remains free and open source.

Based on the original Merkuro/Kalendar project code.
