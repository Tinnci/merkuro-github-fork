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

    signal addIncidence(int type, date addDate, bool includeTime)
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
    property bool initialWeek: true
    readonly property bool isLarge: width > Kirigami.Units.gridUnit * 30
    readonly property bool isDark: LabelUtils.isDarkColor(Kirigami.Theme.backgroundColor)

    property real dayWidth: ((root.width - hourLabelWidth - leftPadding) / 7) - gridLineWidth
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

    function setToDate(date, isInitialWeek = false) {
        root.initialWeek = isInitialWeek;
        date = DateUtils.getFirstDayOfWeek(date);
        let weekDiff = Math.round((date - pathView.currentItem.startDate) / (7 * 24 * 60 * 60 * 1000));

        let newIndex = pathView.currentIndex + weekDiff;
        console.log(weekDiff, DateUtils.getWeek(date), DateUtils.getWeek(pathView.currentItem.startDate));

        let firstItemDate = pathView.model.data(pathView.model.index(1,0), Kalendar.WeekViewModel.StartDateRole);
        let lastItemDate = pathView.model.data(pathView.model.index(pathView.model.rowCount() - 1,0), Kalendar.WeekViewModel.StartDateRole);

        while(firstItemDate >= date) {
            pathView.model.addDates(false)
            firstItemDate = pathView.model.data(pathView.model.index(1,0), Kalendar.WeekViewModel.StartDateRole);
            newIndex = 0;
        }
        if(firstItemDate < date && newIndex === 0) {
            newIndex = Math.round((date - firstItemDate) / (7 * 24 * 60 * 60 * 1000)) + 1
        }

        while(lastItemDate <= date) {
            pathView.model.addDates(true)
            lastItemDate = pathView.model.data(pathView.model.index(pathView.model.rowCount() - 1,0), Kalendar.MonthViewModel.StartDateRole);
        }
        pathView.currentIndex = newIndex;
        selectedDate = date;
    }

    actions {
        left: Kirigami.Action {
            icon.name: "go-previous"
            text: i18n("Previous Week")
            shortcut: "Left"
            onTriggered: setToDate(DateUtils.addDaysToDate(pathView.currentItem.startDate, -7))
            displayHint: Kirigami.DisplayHint.IconOnly
        }
        right: Kirigami.Action {
            icon.name: "go-next"
            text: i18n("Next Week")
            shortcut: "Right"
            onTriggered: setToDate(DateUtils.addDaysToDate(pathView.currentItem.startDate, 7))
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
            root.initialWeek = false;

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
                    filters: Kalendar.HourlyIncidenceModel.NoAllDay | Kalendar.HourlyIncidenceModel.NoMultiDay
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
                    id: headerTopSeparator
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    Layout.fillWidth: true
                    height: root.gridLineWidth
                }

                Loader {
                    id: allDayIncidenceModelLoader
                    asynchronous: true
                    sourceComponent: Kalendar.MultiDayIncidenceModel {
                        periodLength: 7
                        filters: Kalendar.MultiDayIncidenceModel.AllDayOnly | Kalendar.MultiDayIncidenceModel.MultiDayOnly
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

                Item {
                    id: allDayHeader
                    Layout.fillWidth: true
                    height: allDayViewLoader.implicitHeight
                    visible: allDayViewLoader.active
                    clip: true

                    QQC2.Label {
                        width: root.hourLabelWidth
                        padding: Kirigami.Units.smallSpacing
                        leftPadding: Kirigami.Units.largeSpacing
                        verticalAlignment: Text.AlignTop
                        horizontalAlignment: Text.AlignRight
                        text: i18n("All day")
                        wrapMode: Text.Wrap
                        color: Kirigami.Theme.disabledTextColor
                    }

                    Loader {
                        id: allDayViewLoader
                        anchors.fill: parent
                        anchors.leftMargin: root.hourLabelWidth
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
                                        spacing: root.gridLineWidth
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

                                                    DayMouseArea {
                                                        id: listViewMenu
                                                        anchors.fill: parent
                                                        z: -1

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
                                                                x: ((root.dayWidth + root.gridLineWidth) * modelData.starts) + horizontalSpacing
                                                                y: horizontalSpacing
                                                                width: ((root.dayWidth + root.gridLineWidth) * modelData.duration) - (horizontalSpacing * 2) - root.gridLineWidth // Account for spacing added to x and for spacing at end of line
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
                                    x: ((index + 1) * root.dayWidth) + (index * root.gridLineWidth)
                                    width: root.gridLineWidth
                                }
                            }
                        }
                    }
                }

                Kirigami.Separator {
                    id: headerBottomSeparator
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

                    property real periodsPerHour: 60 / modelLoader.item.periodLength
                    property real daySections: (60 * 24) / modelLoader.item.periodLength
                    property real dayHeight: (daySections * Kirigami.Units.gridUnit) + (root.gridLineWidth * 23)
                    property real hourHeight: periodsPerHour * Kirigami.Units.gridUnit

                    Item {
                        id: hourlyViewContents
                        anchors.fill: parent
                        clip: true

                        Column {
                            id: hourLabelsColumn
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.topMargin: (fontMetrics.height / 2) + (root.gridLineWidth / 2)
                            spacing: root.gridLineWidth

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
                            id: innerWeekView
                            anchors.fill: parent
                            anchors.leftMargin: root.hourLabelWidth
                            clip: true

                            Kirigami.Separator {
                                anchors.fill: parent
                            }

                            Row {
                                id: weekRow
                                anchors.fill: parent
                                spacing: root.gridLineWidth

                                Repeater {
                                    model: modelLoader.item

                                    delegate: Item {
                                        id: dayColumn
                                        property int index: model.index
                                        width: root.dayWidth
                                        height: hourlyView.dayHeight
                                        clip: true

                                        Column {
                                            anchors.fill: parent
                                            spacing: root.gridLineWidth
                                            Repeater {
                                                model: 24
                                                delegate: Rectangle {
                                                    Kirigami.Theme.inherit: false
                                                    Kirigami.Theme.colorSet: Kirigami.Theme.View
                                                    width: parent.width
                                                    height: hourlyView.hourHeight
                                                    color: Kirigami.Theme.backgroundColor

                                                    DayMouseArea {
                                                        anchors.fill: parent
                                                        addDate: new Date(DateUtils.addDaysToDate(viewLoader.startDate, dayColumn.index).setHours(index - 1))
                                                        onAddNewIncidence: addIncidence(type, addDate, true)
                                                    }
                                                }
                                            }
                                        }

                                        Repeater {
                                            id: incidencesRepeater
                                            model: incidences
                                            delegate: Rectangle {
                                                property int rectRadius: 5
                                                property real gridLineYCompensation: (modelData.starts / hourlyView.periodsPerHour) * root.gridLineWidth
                                                property real gridLineHeightCompensation: (modelData.duration / hourlyView.periodsPerHour) * root.gridLineWidth
                                                property bool isOpenOccurrence: root.openOccurrence ?
                                                    root.openOccurrence.incidenceId === modelData.incidenceId : false

                                                x: root.incidenceSpacing + (modelData.priorTakenWidthShare * root.dayWidth)
                                                y: (modelData.starts * Kirigami.Units.gridUnit) + root.incidenceSpacing + gridLineYCompensation
                                                width: (root.dayWidth * modelData.widthShare) - (root.incidenceSpacing * 2)// - ((root.maxConcurrentIncidences - 1) * root.incidenceSpacing)
                                                height: (modelData.duration * Kirigami.Units.gridUnit) - (root.incidenceSpacing * 2) + gridLineHeightCompensation - root.gridLineWidth
                                                color: Qt.rgba(0,0,0,0)
                                                visible: !modelData.allDay

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
