// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import org.kde.merkuro.calendar 1.0 as Merkuro
import "labelutils.js" as LabelUtils

RowLayout {
    id: headerLayout

    property bool isDark: CalendarUiUtils.darkMode
    property var mode: Merkuro.CalendarApplication.Event
    property var filterCollectionDetails: Merkuro.Filter.collectionId >= 0 ?
        Merkuro.CalendarManager.getCollectionDetails(Merkuro.Filter.collectionId) : null

    visible: mode === Merkuro.CalendarApplication.Todo || Merkuro.Filter.tags.length > 0 || Merkuro.Filter.collectionId > -1
    height: visible ? implicitHeight : 0

    spacing: Kirigami.Units.smallSpacing

    Connections {
        target: Merkuro.CalendarManager
        function onCollectionColorsChanged() {
            // Trick into reevaluating filterCollectionDetails
            Merkuro.Filter.tagsChanged();
        }
    }

    RowLayout {
        Layout.margins: Kirigami.Units.largeSpacing
        Kirigami.Heading {
            id: heading

            Layout.alignment: Qt.AlignVCenter
            width: implicitWidth

            text: headerLayout.mode !== Merkuro.CalendarApplication.Todo ? i18n("Filtering by tags") : headerLayout.filterCollectionDetails && Merkuro.Filter.collectionId > -1 ?
                headerLayout.filterCollectionDetails.displayName : i18n("All Tasks")
            font.weight: headerLayout.mode !== Merkuro.CalendarApplication.Todo ? Font.Normal : Font.Bold
            color: headerLayout.mode === Merkuro.CalendarApplication.Todo && headerLayout.filterCollectionDetails && Merkuro.Filter.collectionId > -1 ?
                headerLayout.filterCollectionDetails.color : Kirigami.Theme.textColor
            elide: Text.ElideRight
            level: headerLayout.mode === Merkuro.CalendarApplication.Todo ? 1 : 2
        }
        QQC2.ToolButton {
            Layout.alignment: Qt.AlignVCenter
            icon.name: "edit-reset"
            visible: headerLayout.mode === Merkuro.CalendarApplication.Todo && Merkuro.Filter.collectionId > -1
            onClicked: Merkuro.Filter.collectionId = -1
        }
    }

    Flow {
        id: tagFlow

        Layout.fillWidth: true
        Layout.margins: Kirigami.Units.largeSpacing
        Layout.bottomMargin: headerLayout.rows > 1 ? Kirigami.Units.smallSpacing : Kirigami.Units.largeSpacing

        spacing: Kirigami.Units.smallSpacing
        layoutDirection: Qt.RightToLeft
        clip: true
        visible: Merkuro.Filter.tags.length > 0

        Repeater {
            id: tagRepeater
            model: Merkuro.Filter ? Merkuro.Filter.tags : {}

            Tag {
                id: filterTag

                text: modelData

                implicitWidth: itemLayout.implicitWidth > tagFlow.width ?
                    tagFlow.width : itemLayout.implicitWidth
                isHeading: true
                headingItem.color: headerLayout.mode === Merkuro.CalendarApplication.Todo && headerLayout.filterCollectionDetails ?
                    headerLayout.filterCollectionDetails.color : Kirigami.Theme.textColor

                onClicked: Merkuro.Filter.removeTag(modelData)
                actionIcon.name: "edit-delete-remove"
                actionText: i18n("Remove filtering tag")
            }
        }
    }

    Kirigami.Heading {
        id: numTasksHeading

        Layout.fillWidth: true
        Layout.rightMargin: Kirigami.Units.largeSpacing
        horizontalAlignment: Text.AlignRight

        function updateTasksCount() {
            if (headerLayout.mode === Merkuro.CalendarApplication.Todo) {
                text = applicationWindow().pageStack.currentItem.incompleteView.model.rowCount();
            }
        }

        Connections {
            target: headerLayout.mode === Merkuro.CalendarApplication.Todo ? applicationWindow().pageStack.currentItem.incompleteView.model : null
            function onRowsInserted() {
                numTasksHeading.updateTasksCount();
            }

            function onRowsRemoved() {
                numTasksHeading.updateTasksCount();
            }
        }

        text: headerLayout.mode === Merkuro.CalendarApplication.Todo ? applicationWindow().pageStack.currentItem.incompleteView.model.rowCount() : ''
        font.weight: Font.Bold
        color: headerLayout.mode === Merkuro.CalendarApplication.Todo && headerLayout.filterCollectionDetails && Merkuro.Filter.collectionId > -1 ?
            headerLayout.filterCollectionDetails.color : Kirigami.Theme.textColor
        elide: Text.ElideRight
        visible: headerLayout.mode === Merkuro.CalendarApplication.Todo
    }
}
