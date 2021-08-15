// SPDX-FileCopyrightText: 2021 Carl Schwan <carl@carlschwan.eu>
// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>

// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import org.kde.kalendar 1.0 as Kalendar
import "dateutils.js" as DateUtils

Kirigami.ScrollablePage {
    title: i18n("Todos")

    signal viewTodo(var todoData, var collectionData)

    ListView {
        model: Kalendar.CalendarManager.todoModel
        delegate: Kirigami.BasicListItem {
            label: model.summary
            subtitle: model.dueDate
            leftPadding: ((Kirigami.Units.gridUnit * 1.5) * (kDescendantLevel - 1)) + Kirigami.Units.largeSpacing
            leading: QQC2.CheckBox {
                checked: model.checked
            }
            Component.onCompleted: console.log(Kalendar.CalendarManager.getTodoData(index));
            onClicked: viewTodo( Kalendar.CalendarManager.getTodoData(index), Kalendar.CalendarManager.getTodoModelCollectionDetails(index))
        }
    }
}
