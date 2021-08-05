// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import org.kde.kalendar 1.0 as Kalendar
import "dateutils.js" as DateUtils

Kirigami.ScrollablePage {
    ListView {
        model: Kalendar.CalendarManager.totoModel
        delegate: Kirigami.BasicListItem {
            label: model.summary + "rjeoi"
        }
    }
}
