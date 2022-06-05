// SPDX-FileCopyrightText: 2022 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import "labelutils.js" as LabelUtils

import org.kde.kalendar 1.0
import org.kde.kalendar.utils 1.0

Item {
    property IncidenceWrapper incidenceWrapper: modelData
    property var collectionData: CalendarManager.getCollectionDetails(incidenceWrapper.collectionId)

    IncidenceDelegateBackground {
        //isOpenOccurrence: parent.isOpenOccurrence
        //isDark: parent.isDark
        color: LabelUtils.getIncidenceDelegateBackgroundColor(collectionData.color, false) // TODO: Address darkmode
    }

    Layout.fillWidth: true
    Layout.preferredHeight: Kirigami.Units.gridUnit * 2

    RowLayout {
        id: incidenceContents
        clip: true
        property color textColor: LabelUtils.getIncidenceLabelColor(collectionData.color, false)

        anchors.fill: parent
        anchors.margins: Kirigami.Units.largeSpacing

        Kirigami.Icon {
            Layout.maximumHeight: parent.height
            Layout.maximumWidth: height

            source: incidenceWrapper.incidenceIconName
            isMask: true
            color: incidenceContents.textColor
            visible: incidenceWrapper.incidenceType === IncidenceWrapper.TypeTodo
        }

        QQC2.Label {
            Layout.fillWidth: true
            text: incidenceWrapper.summary
            elide: Text.ElideRight
            font.weight: Font.Medium
            font.strikeout: incidenceWrapper.todoCompleted
            color: incidenceContents.textColor
        }

        Kirigami.Icon {
            id: recurringIcon
            Layout.fillHeight: true
            source: "appointment-recurring"
            isMask: true
            color: incidenceContents.textColor
            visible: incidenceWrapper.recurrenceData.type
        }
        Kirigami.Icon {
            id: reminderIcon
            Layout.fillHeight: true
            source: "appointment-reminder"
            isMask: true
            color: incidenceContents.textColor
            visible: incidenceWrapper.remindersModel.rowCount() > 0
        }

        QQC2.Label {
            text: incidenceWrapper.incidenceType === IncidenceWrapper.TypeTodo ?
                incidenceWrapper.incidenceEnd.toLocaleTimeString(Qt.locale(), Locale.NarrowFormat) :
                incidenceWrapper.incidenceStart.toLocaleTimeString(Qt.locale(), Locale.NarrowFormat)
            color: incidenceContents.textColor
            visible: !incidenceWrapper.allDay
        }
    }

    IncidenceMouseArea {
        id: mouseArea
        incidenceData: KalendarUiUtils.fakeModelDataFromIncidenceWrapper(incidenceWrapper)
        collectionId: incidenceWrapper.collectionId

        preventStealing: !Kirigami.Settings.tabletMode && !Kirigami.Settings.isMobile
        //drag.target: !Kirigami.Settings.isMobile && !modelData.isReadOnly && incidenceDelegate.dragDropEnabled ? parent : undefined
        //onReleased: parent.Drag.drop()

        onViewClicked: KalendarUiUtils.setUpView(incidenceData)
        onEditClicked: KalendarUiUtils.setUpEdit(incidencePtr)
        onDeleteClicked: KalendarUiUtils.setUpDelete(incidencePtr, deleteDate)
        onTodoCompletedClicked: KalendarUiUtils.completeTodo(incidencePtr)
        onAddSubTodoClicked: KalendarUiUtils.setUpAddSubTodo(parentWrapper)
    }
}
