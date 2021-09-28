// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: GPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.15 as QQC2
import org.kde.kirigami 2.14 as Kirigami

import org.kde.kalendar 1.0 as Kalendar
import "dateutils.js" as DateUtils
import "labelutils.js" as LabelUtils

Kirigami.Page {
    id: root

    signal addIncidence(int type, date addDate)
    signal viewIncidence(var modelData, var collectionData)
    signal editIncidence(var incidencePtr, var collectionId)
    signal deleteIncidence(var incidencePtr, date deleteDate)
    signal completeTodo(var incidencePtr)
    signal addSubTodo(var parentWrapper)

    property var openOccurrence
    property date selectedDate: new Date()
    property date startDate: DateUtils.getFirstDayOfMonth(selectedDate)
    property int day: selectedDate.getDate()
    property int month: selectedDate.getMonth()
    property int year: selectedDate.getFullYear()
    property bool initialMonth: true
    readonly property bool isLarge: width > Kirigami.Units.gridUnit * 30
    readonly property bool isDark: LabelUtils.isDarkColor(Kirigami.Theme.backgroundColor)

    property real dayWidth: (root.width - hourLabelWidth) / 7
    property real incidenceSpacing: Kirigami.Units.smallSpacing / 2
    property real gridLineWidth: 1.0
    property real hourLabelWidth: Kirigami.Units.gridUnit * 3

    property var hourStrings: []
    Component.onCompleted: {
        let date = new Date(0, 0, 0, 0, 0, 0, 0);
        for(let i = 0; i < 23; i++) {
            date.setHours(i);
            hourStrings.push(date.toLocaleTimeString(Qt.locale(), Locale.NarrowFormat));
            hourStringsChanged();
        }
    }

    background: Rectangle {
        Kirigami.Theme.inherit: false
        Kirigami.Theme.colorSet: Kirigami.Theme.View
        color: Kirigami.Theme.backgroundColor
    }

    /*function setToDate(date, isInitialMonth = false) {
     *        root.initialMonth = isInitialMonth;
     *        let monthDiff = date.getMonth() - pathView.currentItem.firstDayOfMonth.getMonth() + (12 * (date.getFullYear() - pathView.currentItem.firstDayOfMonth.getFullYear()))
     *        let newIndex = pathView.currentIndex + monthDiff;
     *
     *        let firstItemDate = pathView.model.data(pathView.model.index(1,0), Kalendar.MonthViewModel.FirstDayOfMonthRole);
     *        let lastItemDate = pathView.model.data(pathView.model.index(pathView.model.rowCount() - 1,0), Kalendar.MonthViewModel.FirstDayOfMonthRole);
     *
     *        while(firstItemDate >= date) {
     *            pathView.model.addDates(false)
     *            firstItemDate = pathView.model.data(pathView.model.index(1,0), Kalendar.MonthViewModel.FirstDayOfMonthRole);
     *            newIndex = 0;
}
if(firstItemDate < date && newIndex === 0) {
    newIndex = date.getMonth() - firstItemDate.getMonth() + (12 * (date.getFullYear() - firstItemDate.getFullYear())) + 1;
}

while(lastItemDate <= date) {
    pathView.model.addDates(true)
    lastItemDate = pathView.model.data(pathView.model.index(pathView.model.rowCount() - 1,0), Kalendar.MonthViewModel.FirstDayOfMonthRole);
}
pathView.currentIndex = newIndex;
selectedDate = date;
}*/

    actions {
        left: Kirigami.Action {
            icon.name: "go-previous"
            text: i18n("Previous Week")
            shortcut: "Left"
            //onTriggered: setToDate(DateUtils.addMonthsToDate(pathView.currentItem.firstDayOfMonth, -1))
            displayHint: Kirigami.DisplayHint.IconOnly
        }
        right: Kirigami.Action {
            icon.name: "go-next"
            text: i18n("Next Week")
            shortcut: "Right"
            //onTriggered: setToDate(DateUtils.addMonthsToDate(pathView.currentItem.firstDayOfMonth, 1))
            displayHint: Kirigami.DisplayHint.IconOnly
        }
        main: Kirigami.Action {
            icon.name: "go-jump-today"
            text: i18n("Today")
            onTriggered: setToDate(new Date())
        }
    }

    padding: 0

    PathView {
        id: pathView

        anchors.fill: parent
        flickDeceleration: Kirigami.Units.longDuration
        preferredHighlightBegin: 0.5
        preferredHighlightEnd: 0.5
        snapMode: PathView.SnapToItem
        focus: true
        interactive: true //Kirigami.Settings.tabletMode

        path: Path {
            startX: - pathView.width * pathView.count / 2 + pathView.width / 2
            startY: pathView.height / 2
            PathLine {
                x: pathView.width * pathView.count / 2 + pathView.width / 2
                y: pathView.height / 2
            }
        }

        model: Kalendar.WeekViewModel {}

        property date dateToUse
        property int startIndex
        Component.onCompleted: {
            startIndex = count / 2;
            currentIndex = startIndex;
        }
        onCurrentIndexChanged: {
            root.startDate = currentItem.startDate;
            root.month = currentItem.month;
            root.year = currentItem.year;
            //root.initialMonth = false;

            if(currentIndex >= count - 2) {
                model.addDates(true);
            } else if (currentIndex <= 1) {
                model.addDates(false);
                startIndex += model.weeksToAdd;
            }
        }

        delegate: Loader {
            id: viewLoader

            property date startDate: model.startDate
            property int month: model.selectedMonth - 1 // Convert QDateTime month to JS month
            property int year: model.selectedYear

            property int index: model.index
            property bool isCurrentItem: PathView.isCurrentItem
            property bool isNextOrCurrentItem: index >= pathView.currentIndex -1 && index <= pathView.currentIndex + 1

            Loader {
                id: modelLoader
                active: true
                asynchronous: true
                sourceComponent: Kalendar.HourlyIncidenceModel {
                    id: hourlyModel
                    model: Kalendar.IncidenceOccurrenceModel {
                        id: occurrenceModel
                        objectName: "incidenceOccurrenceModel"
                        start: viewLoader.startDate
                        length: 7
                        filter: root.filter ? root.filter : {}
                        calendar: Kalendar.CalendarManager.calendar
                    }
                }
            }

            active: isNextOrCurrentItem
            //asynchronous: true
            sourceComponent: ColumnLayout {
                width: pathView.width
                height: pathView.height
                spacing: 0

                Row {
                    Layout.fillWidth: true
                    Layout.leftMargin: root.hourLabelWidth
                    Repeater {
                        id: dayHeadings
                        model: modelLoader.item.rowCount()
                        delegate: Kirigami.Heading {
                            width: root.dayWidth
                            horizontalAlignment: Text.AlignRight
                            padding: Kirigami.Units.smallSpacing
                            level: 2
                            text: DateUtils.addDaysToDate(viewLoader.startDate, index).toLocaleDateString(Qt.locale(), "ddd <b>dd</b>")
                        }
                    }
                }

                Kirigami.Separator {
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.fillWidth: true
                    height: root.gridLineWidth
                }

                Loader {
                    id: allDayIncidenceModelLoader
                    asynchronous: true
                    sourceComponent: Kalendar.MultiDayIncidenceModel {
                        periodLength: 7
                        filters: Kalendar.MultiDayIncidenceModel.AllDayOnly //| Kalendar.MultiDayIncidenceModel.NoStartDateOnly
                        model: Kalendar.IncidenceOccurrenceModel {
                            id: occurrenceModel
                            objectName: "incidenceOccurrenceModel"
                            start: viewLoader.startDate
                            length: 7
                            filter: root.filter ? root.filter : {}
                            calendar: Kalendar.CalendarManager.calendar
                        }
                    }
                }

                Loader {
                    id: allDayViewLoader
                    Layout.fillWidth: true
                    Layout.leftMargin: root.hourLabelWidth
                    height: visible ? implicitHeight : 0
                    visible: active
                    active: allDayIncidenceModelLoader.item.incidenceCount > 0
                    sourceComponent: Item {
                        implicitHeight: Kirigami.Units.gridUnit * 3
                        clip: true
                        Repeater {
                            model: allDayIncidenceModelLoader.item
                            Layout.topMargin: Kirigami.Units.largeSpacing
                            //One row => one week
                            Item {
                                width: parent.width
                                height: Kirigami.Units.gridUnit * 3
                                clip: true
                                RowLayout {
                                    width: parent.width
                                    height: parent.height
                                    spacing: root.spacing
                                    Item {
                                        id: dayDelegate
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        property date startDate: periodStartDate

                                        QQC2.ScrollView {
                                            anchors {
                                                fill: parent
                                            }

                                            QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

                                            ListView {
                                                id: linesRepeater
                                                Layout.fillWidth: true
                                                Layout.rightMargin: parent.contentHeight > parent.availableHeight ?
                                                    spacing : spacing

                                                clip: true
                                                spacing: Kirigami.Units.smallSpacing
                                                contentHeight: contentHeight + Kirigami.Units.largeSpacing

                                                DayMouseArea {
                                                    id: listViewMenu
                                                    anchors.fill: parent
                                                    z: -1

                                                    function useGridSquareDate(type, root, globalPos) {
                                                        for(var i in root.children) {
                                                            var child = root.children[i];
                                                            var localpos = child.mapFromGlobal(globalPos.x, globalPos.y);

                                                            if(child.contains(localpos) && child.gridSquareDate) {
                                                                addIncidence(type, child.gridSquareDate);
                                                            } else {
                                                                useGridSquareDate(type, child, globalPos);
                                                            }
                                                        }
                                                    }

                                                    onAddNewIncidence: useGridSquareDate(type, applicationWindow().contentItem, this.mapToGlobal(clickX, clickY))
                                                }

                                                model: incidences

                                                delegate: Item {
                                                    id: line
                                                    height: Kirigami.Units.gridUnit + Kirigami.Units.smallSpacing

                                                    //Incidences
                                                    Repeater {
                                                        id: incidencesRepeater
                                                        model: modelData
                                                        Rectangle {
                                                            x: ((root.dayWidth + root.spacing) * modelData.starts) + horizontalSpacing
                                                            y: horizontalSpacing
                                                            width: ((root.dayWidth + root.spacing) * modelData.duration) - (horizontalSpacing * 2) - root.spacing // Account for spacing added to x and for spacing at end of line
                                                            height: parent.height
                                                            opacity: isOpenOccurrence ||
                                                            modelData.endTime.getMonth() == root.month ||
                                                            modelData.startTime.getMonth() == root.month ?
                                                            1.0 : 0.5
                                                            radius: rectRadius
                                                            color: Qt.rgba(0,0,0,0)

                                                            property int rectRadius: 5
                                                            property int horizontalSpacing: linesRepeater.spacing

                                                            property bool isOpenOccurrence: root.openOccurrence ?
                                                            root.openOccurrence.incidenceId === modelData.incidenceId : false

                                                            Rectangle {
                                                                id: incidenceBackground
                                                                anchors.fill: parent
                                                                color: isOpenOccurrence ? modelData.color :
                                                                LabelUtils.getIncidenceBackgroundColor(modelData.color, root.isDark)
                                                                visible: isOpenOccurrence ||
                                                                modelData.endTime.getMonth() === root.month ||
                                                                modelData.startTime.getMonth() === root.month
                                                                radius: parent.rectRadius
                                                            }

                                                            RowLayout {
                                                                id: incidenceContents

                                                                property color textColor: LabelUtils.getIncidenceLabelColor(modelData.color, root.isDark)

                                                                function otherMonthTextColor(color) {
                                                                    if(root.isDark) {
                                                                        if(LabelUtils.getDarkness(color) >= 0.5) {
                                                                            return Qt.lighter(color, 2);
                                                                        }
                                                                        return Qt.lighter(color, 1.5);
                                                                    }
                                                                    return Qt.darker(color, 3);
                                                                }

                                                                anchors {
                                                                    fill: parent
                                                                    leftMargin: Kirigami.Units.smallSpacing
                                                                    rightMargin: Kirigami.Units.smallSpacing
                                                                }

                                                                Kirigami.Icon {
                                                                    Layout.maximumHeight: parent.height
                                                                    Layout.maximumWidth: height

                                                                    source: modelData.incidenceTypeIcon
                                                                    isMask: true
                                                                    color: isOpenOccurrence ? (LabelUtils.isDarkColor(modelData.color) ? "white" : "black") :
                                                                    incidenceBackground.visible ? incidenceContents.textColor :
                                                                    incidenceContents.otherMonthTextColor(modelData.color)
                                                                }

                                                                QQC2.Label {
                                                                    Layout.fillWidth: true
                                                                    text: modelData.text
                                                                    elide: Text.ElideRight
                                                                    font.weight: Font.Medium
                                                                    color: isOpenOccurrence ? (LabelUtils.isDarkColor(modelData.color) ? "white" : "black") :
                                                                    incidenceBackground.visible ? incidenceContents.textColor :
                                                                    incidenceContents.otherMonthTextColor(modelData.color)
                                                                }
                                                            }

                                                            IncidenceMouseArea {
                                                                incidenceData: modelData
                                                                collectionId: modelData.collectionId

                                                                onViewClicked: viewIncidence(modelData, collectionData)
                                                                onEditClicked: editIncidence(incidencePtr, collectionId)
                                                                onDeleteClicked: deleteIncidence(incidencePtr, deleteDate)
                                                                onTodoCompletedClicked: completeTodo(incidencePtr)
                                                                onAddSubTodoClicked: root.addSubTodo(parentWrapper)
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        Repeater {
                            id: allDayDayLineRepeater
                            anchors.fill: parent
                            model: modelLoader.item.rowCount() - 1 // Don't want line at beginning
                            delegate: Kirigami.Separator {
                                anchors.top: parent.top
                                height: hourlyView.dayHeight
                                x: ((index + 1) * root.dayWidth) - (root.gridLineWidth / 2)
                                width: root.gridLineWidth
                            }
                        }
                    }
                }

                Kirigami.Separator {
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.fillWidth: true
                    height: root.gridLineWidth
                    visible: allDayViewLoader.active
                }


                QQC2.ScrollView {
                    id: hourlyView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    contentWidth: availableWidth
                    contentHeight: dayHeight
                    QQC2.ScrollBar.horizontal.policy: QQC2.ScrollBar.AlwaysOff

                    property int periodsPerHour: 60 / modelLoader.item.periodLength
                    property int daySections: (60 * 24) / modelLoader.item.periodLength
                    property real dayHeight: daySections * Kirigami.Units.gridUnit

                    Item {
                        anchors.fill: parent
                        clip: true

                        Column {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.topMargin: fontMetrics.height / 2

                            FontMetrics {
                                id: fontMetrics
                            }

                            Repeater {
                                model: root.hourStrings
                                delegate: QQC2.Label {
                                    height: (Kirigami.Units.gridUnit * hourlyView.periodsPerHour)
                                    width: root.hourLabelWidth
                                    rightPadding: Kirigami.Units.smallSpacing
                                    verticalAlignment: Text.AlignBottom
                                    horizontalAlignment: Text.AlignRight
                                    text: modelData
                                    color: Kirigami.Theme.disabledTextColor
                                }
                            }
                        }

                        Item {
                            anchors.fill: parent
                            anchors.leftMargin: root.hourLabelWidth
                            clip: true

                            Repeater {
                                id: hourLineRepeater
                                model: 22 // No top and bottom separator
                                delegate: Kirigami.Separator {
                                    anchors.left: parent.left
                                    anchors.right: parent.right
                                    y: (((index + 1) * hourlyView.periodsPerHour) * Kirigami.Units.gridUnit) - (root.gridLineWidth / 2)
                                    height: root.gridLineWidth
                                }
                            }

                            Repeater {
                                id: dayLineRepeater
                                anchors.fill: parent
                                model: modelLoader.item.rowCount() - 1 // Don't want line at beginning
                                delegate: Kirigami.Separator {
                                    anchors.top: parent.top
                                    height: hourlyView.dayHeight
                                    x: ((index + 1) * root.dayWidth) - (root.gridLineWidth / 2)
                                    width: root.gridLineWidth
                                }
                            }

                            Row {
                                anchors.fill: parent

                                Repeater {
                                    model: modelLoader.item

                                    delegate: Item {
                                        id: dayColumn
                                        property int index: model.index
                                        width: root.dayWidth
                                        height: hourlyView.dayHeight
                                        clip: true

                                        Repeater {
                                            id: incidencesRepeater
                                            model: incidences
                                            delegate: Rectangle {
                                                x: root.incidenceSpacing + (root.gridLineWidth / 2)
                                                y: (modelData.starts * Kirigami.Units.gridUnit) + root.incidenceSpacing + (root.gridLineWidth / 2)
                                                width: root.dayWidth - (root.incidenceSpacing * 2) - root.gridLineWidth //* modelData.widthShare
                                                height: (modelData.duration * Kirigami.Units.gridUnit) - (root.incidenceSpacing * 2) - root.gridLineWidth
                                                color: Qt.rgba(0,0,0,0)
                                                property int rectRadius: 5
                                                Component.onCompleted: if(modelData.allDay) {
                                                    visible = false
                                                    allDayViewLoader.active = true
                                                } //console.log(modelData.starts, modelData.widthShare, modelData.duration)

                                                property bool isOpenOccurrence: root.openOccurrence ?
                                                root.openOccurrence.incidenceId === modelData.incidenceId : false

                                                Rectangle {
                                                    id: incidenceBackground
                                                    anchors.fill: parent
                                                    color: isOpenOccurrence ? modelData.color :
                                                    LabelUtils.getIncidenceBackgroundColor(modelData.color, root.isDark)
                                                    radius: parent.rectRadius
                                                }

                                                ColumnLayout {
                                                    id: incidenceContents

                                                    property color textColor: LabelUtils.getIncidenceLabelColor(modelData.color, root.isDark)

                                                    function otherMonthTextColor(color) {
                                                        if(root.isDark) {
                                                            if(LabelUtils.getDarkness(color) >= 0.5) {
                                                                return Qt.lighter(color, 2);
                                                            }
                                                            return Qt.lighter(color, 1.5);
                                                        }
                                                        return Qt.darker(color, 3);
                                                    }

                                                    anchors {
                                                        fill: parent
                                                        leftMargin: Kirigami.Units.smallSpacing
                                                        rightMargin: Kirigami.Units.smallSpacing
                                                        topMargin: Kirigami.Units.smallSpacing
                                                        bottomMargin: Kirigami.Units.smallSpacing
                                                    }

                                                    QQC2.Label {
                                                        Layout.fillWidth: true
                                                        Layout.fillHeight: true
                                                        text: modelData.text
                                                        wrapMode: Text.Wrap
                                                        elide: Text.ElideRight
                                                        font.weight: Font.Medium
                                                        color: isOpenOccurrence ? (LabelUtils.isDarkColor(modelData.color) ? "white" : "black") :
                                                        incidenceBackground.visible ? incidenceContents.textColor :
                                                        incidenceContents.otherMonthTextColor(modelData.color)
                                                    }
                                                }

                                                IncidenceMouseArea {
                                                    incidenceData: modelData
                                                    collectionId: modelData.collectionId

                                                    onViewClicked: viewIncidence(modelData, collectionData)
                                                    onEditClicked: editIncidence(incidencePtr, collectionId)
                                                    onDeleteClicked: deleteIncidence(incidencePtr, deleteDate)
                                                    onTodoCompletedClicked: completeTodo(incidencePtr)
                                                    onAddSubTodoClicked: root.addSubTodo(parentWrapper)
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
