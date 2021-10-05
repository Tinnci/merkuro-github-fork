// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import org.kde.kalendar 1.0 as Kalendar

GridLayout {
    id: headerLayout
    columns: 2
    rows: 2

    signal removeFilterTag(string tagName)

    property bool todoMode: false
    property int filterCollectionId
    property var filterCollectionDetails: filterCollectionId && filterCollectionId >= 0 ?
        Kalendar.CalendarManager.getCollectionDetails(filterCollectionId) : null
    property var tags: []

    visible: todoMode || tags.length > 0 || filterCollectionId > -1
    height: visible ? implicitHeight : 0

    Kirigami.Heading {
        Layout.row: 0
        Layout.column: 0
        Layout.columnSpan: headerLayout.width < Kirigami.Units.gridUnit * 30 || headerLayout.tags.length > 0 ? 1 : 2
        Layout.fillWidth: headerLayout.todoMode
        Layout.margins: Kirigami.Units.largeSpacing
        text: !headerLayout.todoMode ? i18n("Filtering by tags") : headerLayout.filterCollectionDetails && headerLayout.filterCollectionId > -1 ?
            headerLayout.filterCollectionDetails.displayName : i18n("All Tasks")
        font.weight: Font.Bold
        color: root.filterCollectionDetails && root.filterCollectionId > -1 ?
            LabelUtils.getIncidenceLabelColor(headerLayout.filterCollectionDetails.color, headerLayout.isDark) : Kirigami.Theme.textColor
        elide: Text.ElideRight
    }

    Flow {
        Layout.fillWidth: true
        Layout.row: 0
        Layout.column: 1
        Layout.margins: Kirigami.Units.largeSpacing
        spacing: Kirigami.Units.smallSpacing
        layoutDirection: Qt.RightToLeft
        visible: headerLayout.tags.length > 0

        Repeater {
            id: tagRepeater
            model: headerLayout.tags ? headerLayout.tags : {}

            Tag {
                id: filterTag

                text: modelData

                implicitWidth: itemLayout.implicitWidth > headerLayout.width - Kirigami.Units.gridUnit * 6 ?
                    headerLayout.width - Kirigami.Units.gridUnit * 6 : itemLayout.implicitWidth
                isHeading: headerLayout.tags.length < 4
                headingItem.color: root.filterCollectionDetails ?
                    LabelUtils.getIncidenceLabelColor(headerLayout.filterCollectionDetails.color, headerLayout.isDark) : Kirigami.Theme.textColor
                headingItem.font.weight: Font.Bold

                icon.name: "edit-delete-remove"
                onClicked: headerLayout.removeFilterTag(modelData)
                actionText: i18n("Remove filtering tag")
            }
        }
    }

    Kirigami.SearchField {
        id: searchField
        Layout.column: (root.width < Kirigami.Units.gridUnit * 30 || headerLayout.tags.length > 0) && headerLayout.todoMode ? 0 : 1
        Layout.row: (root.width < Kirigami.Units.gridUnit * 30 || headerLayout.tags.length > 0) && headerLayout.todoMode ? 1 : 0
        Layout.columnSpan: (root.width < Kirigami.Units.gridUnit * 30 || headerLayout.tags.length > 0) && headerLayout.todoMode ? 2 : 1
        Layout.fillWidth: Layout.row === 1
        Layout.margins: Kirigami.Units.largeSpacing
        onTextChanged: incompleteView.model.filterTodoName(text);
        visible: headerLayout.todoMode
    }
}
