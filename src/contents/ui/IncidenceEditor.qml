// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.0
import org.kde.kirigami 2.15 as Kirigami
import org.kde.kalendar 1.0
import "labelutils.js" as LabelUtils

Kirigami.ScrollablePage {
    id: incidenceEditorSheet

    signal added(int collectionId, IncidenceWrapper incidence)
    signal edited(int collectionId, IncidenceWrapper incidence)
    signal cancel

    // Setting the incidenceWrapper here and now causes some *really* weird behaviour.
    // Set it after this component has already been instantiated.
    property var incidenceWrapper
    property bool editMode: false

    property bool validDates: //incidenceWrapper !== undefined &&
                              editorLoader.item.validFormDates &&
                              incidenceWrapper.incidenceStart < incidenceWrapper.incidenceEnd

    onIncidenceWrapperChanged: if(!editMode) { incidenceWrapper.collectionId = CalendarManager.defaultCalendarId }

    title: editMode ? i18n("Edit incidence") : i18n("Add incidence")

    footer: QQC2.DialogButtonBox {
        standardButtons: QQC2.DialogButtonBox.Cancel

        QQC2.Button {
            icon.name: editMode ? "document-save" : "list-add"
            text: editMode ? i18n("Save") : i18n("Add")
            enabled: incidenceEditorSheet.validDates && incidenceWrapper.summary && incidenceWrapper.collectionId
            QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.AcceptRole
        }

        onRejected: cancel()
        onAccepted: {
            if (editMode) {
                edited(incidenceWrapper.collectionId, incidenceWrapper);
            } else {
                added(incidenceWrapper.collectionId, incidenceWrapper);
            }
            cancel();
        }
    }

    Loader {
        id: editorLoader
        Layout.fillWidth: true
        Layout.fillHeight: true

        active: incidenceWrapper !== undefined
        sourceComponent: ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            /*property bool validFormDates: incidenceStartDateCombo.validDate &&
                                          (incidenceEndDateCombo.validDate || incidenceWrapper.allDay)

            Kirigami.InlineMessage {
                id: invalidDateMessage

                Layout.fillWidth: true
                visible: !incidenceEditorSheet.validDates
                type: Kirigami.MessageType.Error
                // Specify what the problem is to aid user
                text: incidenceEditorSheet.incidenceWrapper.incidenceStart < incidenceEditorSheet.incidenceWrapper.incidenceEnd ?
                      i18n("Invalid dates provided.") : i18n("End date cannot be before start date.")
            }*/

            Kirigami.FormLayout {
                id: incidenceForm

                property date todayDate: new Date()

                QQC2.ComboBox {
                    id: calendarCombo

                    Kirigami.FormData.label: i18n("Calendar:")
                    Layout.fillWidth: true

                    textRole: "display"
                    valueRole: "collectionId"
                    currentIndex: CalendarManager.getCalendarSelectableIndex(incidenceEditorSheet.incidenceWrapper.collectionId)
                    onCurrentValueChanged: incidenceEditorSheet.incidenceWrapper.collectionId = currentValue

                    // Should default to default collection
                    // Should also only show *calendars*
                    model: CalendarManager.selectableCalendars
                    delegate: Kirigami.BasicListItem {
                        label: display
                        icon: decoration
                    }
                    popup.z: 1000
                }
                QQC2.TextField {
                    id: titleField

                    Kirigami.FormData.label: i18n("<b>Title</b>:")
                    placeholderText: i18n("Required")
                    text: incidenceEditorSheet.incidenceWrapper.summary
                    onTextChanged: incidenceEditorSheet.incidenceWrapper.summary = text
                }
                QQC2.TextField {
                    id: locationField

                    Kirigami.FormData.label: i18n("Location:")
                    placeholderText: i18n("Optional")
                    text: incidenceEditorSheet.incidenceWrapper.location
                    onTextChanged: incidenceEditorSheet.incidenceWrapper.location = text
                }

                // Restrain the descriptionTextArea from getting too chonky
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.maximumWidth: eventForm.wideMode ? Kirigami.Units.gridUnit * 25 : -1
                    Kirigami.FormData.label: i18n("Description:")

                    QQC2.TextArea {
                        id: descriptionTextArea

                        Layout.fillWidth: true
                        placeholderText: i18n("Optional")
                        text: eventEditorSheet.eventWrapper.description
                        onTextChanged: eventEditorSheet.eventWrapper.description = text
                    }
                }

                Kirigami.Separator {
                    Kirigami.FormData.isSection: true
                }

                QQC2.CheckBox {
                    id: allDayCheckBox

                    text: i18n("All day incidence")
                    onCheckedChanged: incidenceEditorSheet.incidenceWrapper.allDay = checked
                }
                RowLayout {
                    id: incidenceStartLayout

                    Kirigami.FormData.label: i18n("Start:")
                    Layout.fillWidth: true

                    QQC2.ComboBox {
                        id: incidenceStartDateCombo
                        Layout.fillWidth: true

                        editable: true
                        editText: incidenceEditorSheet.incidenceWrapper.incidenceStart.toLocaleDateString(Qt.locale(), Locale.NarrowFormat)

                        inputMethodHints: Qt.ImhDate

                        property date dateFromText: Date.fromLocaleDateString(Qt.locale(), editText, Locale.NarrowFormat)
                        property bool validDate: !isNaN(dateFromText.getTime())

                        onEditTextChanged: {
                            // Set date from text here because it otherwise updates after this handler
                            dateFromText = Date.fromLocaleDateString(Qt.locale(), editText, Locale.NarrowFormat)
                            let datePicker = incidenceStartDatePicker

                            if (validDate && activeFocus) {
                                var timePicker = incidenceStartTimePicker
                                datePicker.selectedDate = dateFromText;
                                datePicker.clickedDate = dateFromText;
                                incidenceEditorSheet.incidenceWrapper.incidenceStart = new Date(dateFromText.setHours(timePicker.hours, timePicker.minutes));
                            }
                        }

                        popup: QQC2.Popup {
                            id: incidenceStartDatePopup

                            width: Kirigami.Units.gridUnit * 18
                            height: Kirigami.Units.gridUnit * 18
                            y: parent.y + parent.height
                            z: 1000

                            DatePicker {
                                id: incidenceStartDatePicker

                                anchors.fill: parent
                                onDatePicked: {
                                    incidenceStartDatePopup.close();
                                    let hours = incidenceEditorSheet.incidenceWrapper.incidenceStart.getHours();
                                    let minutes = incidenceEditorSheet.incidenceWrapper.incidenceStart.getMinutes();
                                    incidenceEditorSheet.incidenceWrapper.incidenceStart = new Date(pickedDate.setHours(hours, minutes));
                                }
                            }
                        }
                    }
                    QQC2.ComboBox {
                        id: incidenceStartTimeCombo

                        Layout.fillWidth: true

                        editable: true
                        editText: incidenceEditorSheet.incidenceWrapper.incidenceStart.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
                        enabled: !allDayCheckBox.checked
                        visible: !allDayCheckBox.checked

                        inputMethodHints: Qt.ImhTime
                        validator: RegularExpressionValidator {
                            regularExpression: /^([0-1]?[0-9]|2[0-3]):([0-5][0-9])(:[0-5][0-9])?$/
                        }

                        onEditTextChanged: {
                            let timePicker = incidenceStartTimePicker

                            if (acceptableInput && activeFocus) { // Need to check for activeFocus or on load the text gets reset to 00:00
                                timePicker.setToTimeFromString(editText);
                                incidenceEditorSheet.incidenceWrapper.incidenceStart = new Date(incidenceEditorSheet.incidenceWrapper.incidenceStart.setHours(timePicker.hours, timePicker.minutes));
                            }
                        }

                        popup: QQC2.Popup {
                            id: incidenceStartTimePopup
                            width: parent.width
                            height: parent.width * 2
                            y: parent.y + parent.height
                            z: 1000

                            TimePicker {
                                id: incidenceStartTimePicker

                                Component.onCompleted: minuteMultiples = 15
                                Connections {
                                    target: incidenceEditorSheet.incidenceWrapper
                                    function onIncidenceStartChanged() {
                                        incidenceStartTimePicker.dateTime = incidenceEditorSheet.incidenceWrapper.incidenceStart
                                    }
                                }

                                dateTime: incidenceEditorSheet.incidenceWrapper.incidenceStart

                                onDone: {
                                    incidenceStartTimePopup.close();
                                    incidenceEditorSheet.incidenceWrapper.incidenceStart = new Date(incidenceEditorSheet.incidenceWrapper.incidenceStart.setHours(hours, minutes));

                                }
                            }
                        }
                    }
                }
                RowLayout {
                    id: incidenceEndLayout

                    Kirigami.FormData.label: i18n("End:")
                    Layout.fillWidth: true
                    visible: !allDayCheckBox.checked

                    QQC2.ComboBox {
                        id: incidenceEndDateCombo

                        Layout.fillWidth: true

                        property date dateFromText: Date.fromLocaleDateString(Qt.locale(), editText, Locale.NarrowFormat)
                        property bool validDate: !isNaN(dateFromText.getTime())

                        editable: true
                        editText: incidenceEditorSheet.incidenceWrapper.incidenceEnd.toLocaleDateString(Qt.locale(), Locale.NarrowFormat)
                        enabled: !allDayCheckBox.checked

                        onEditTextChanged: {
                            // Set date from text here because it otherwise updates after this handler
                            dateFromText = Date.fromLocaleDateString(Qt.locale(), editText, Locale.NarrowFormat);
                            let datePicker = incidenceEndDatePicker;

                            if (validDate && activeFocus) {
                                var timePicker = incidenceEndTimePicker
                                datePicker.selectedDate = dateFromText;
                                datePicker.clickedDate = dateFromText;
                                incidenceEditorSheet.incidenceWrapper.incidenceEnd = new Date(dateFromText.setHours(timePicker.hours, timePicker.minutes));
                            }
                        }

                        popup: QQC2.Popup {
                            id: incidenceEndDatePopup

                            width: Kirigami.Units.gridUnit * 18
                            height: Kirigami.Units.gridUnit * 18
                            y: parent.y + parent.height
                            z: 1000

                            DatePicker {
                                id: incidenceEndDatePicker
                                anchors.fill: parent
                                onDatePicked: {
                                    incidenceEndDatePopup.close();
                                    let hours = incidenceEditorSheet.incidenceWrapper.incidenceEnd.getHours();
                                    let minutes = incidenceEditorSheet.incidenceWrapper.incidenceEnd.getMinutes();
                                    incidenceEditorSheet.incidenceWrapper.incidenceEnd = new Date(pickedDate.setHours(hours, minutes));
                                }
                            }
                        }
                    }
                    QQC2.ComboBox {
                        id: incidenceEndTimeCombo

                        Layout.fillWidth: true

                        editable: true
                        editText: incidenceEditorSheet.incidenceWrapper.incidenceEnd.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
                        enabled: !allDayCheckBox.checked

                        inputMethodHints: Qt.ImhTime
                        validator: RegularExpressionValidator {
                            regularExpression: /^([0-1]?[0-9]|2[0-3]):([0-5][0-9])(:[0-5][0-9])?$/
                        }

                        onEditTextChanged: {
                            let timePicker = incidenceEndTimePicker

                            if (acceptableInput && activeFocus) {
                                timePicker.setToTimeFromString(editText);
                                incidenceEditorSheet.incidenceWrapper.incidenceEnd = new Date(incidenceEditorSheet.incidenceWrapper.incidenceEnd.setHours(timePicker.hours, timePicker.minutes));
                            }
                        }

                        popup: QQC2.Popup {
                            id: incidenceEndTimePopup

                            width: parent.width
                            height: parent.width * 2
                            y: parent.y + parent.height
                            z: 1000

                            TimePicker {
                                id: incidenceEndTimePicker

                                Component.onCompleted: minuteMultiples = 15
                                Connections {
                                    target: incidenceEditorSheet.incidenceWrapper
                                    function onIncidenceEndChanged() {
                                        incidenceEndTimePicker.dateTime = incidenceEditorSheet.incidenceWrapper.incidenceEnd
                                    }
                                }

                                dateTime: incidenceEditorSheet.incidenceWrapper.incidenceEnd

                                onDone: {
                                    incidenceEndTimePopup.close();
                                    incidenceEditorSheet.incidenceWrapper.incidenceEnd = new Date(incidenceEditorSheet.incidenceWrapper.incidenceEnd.setHours(hours, minutes));
                                }
                            }
                        }
                    }
                }

                QQC2.ComboBox {
                    id: repeatComboBox
                    Kirigami.FormData.label: i18n("Repeat:")
                    Layout.fillWidth: true
                    textRole: "display"
                    valueRole: "interval"
                    onCurrentIndexChanged: if(currentIndex == 0) { incidenceEditorSheet.incidenceWrapper.clearRecurrences(); } // "Never"
                    onCurrentValueChanged: if(currentValue >= 0) { incidenceEditorSheet.incidenceWrapper.setRegularRecurrence(currentValue); }
                    currentIndex: {
                        switch(incidenceEditorSheet.incidenceWrapper.recurrenceData.type) {
                            case 0:
                                return incidenceEditorSheet.incidenceWrapper.recurrenceData.type;
                            case 3: // Daily
                                return incidenceEditorSheet.incidenceWrapper.recurrenceData.frequency === 1 ?
                                    incidenceEditorSheet.incidenceWrapper.recurrenceData.type - 2 : 5
                            case 4: // Weekly
                                return incidenceEditorSheet.incidenceWrapper.recurrenceData.frequency === 1 ?
                                    (incidenceEditorSheet.incidenceWrapper.recurrenceData.weekdays.filter(x => x === true).length === 0 ?
                                    incidenceEditorSheet.incidenceWrapper.recurrenceData.type - 2 : 5) : 5
                            case 5: // Monthly on position (e.g. third Monday)
                            case 7: // Yearly on month
                            case 9: // Yearly on position
                            case 10: // Other
                                return 5;
                            case 6: // Monthly on day (1st of month)
                                return 3;
                            case 8: // Yearly on day
                                return 4;
                        }
                    }
                    model: [
                        {key: "never", display: i18n("Never"), interval: -1},
                        {key: "daily", display: i18n("Daily"), interval: incidenceEditorSheet.incidenceWrapper.recurrenceIntervals.Daily},
                        {key: "weekly", display: i18n("Weekly"), interval: incidenceEditorSheet.incidenceWrapper.recurrenceIntervals.Weekly},
                        {key: "monthly", display: i18n("Monthly"), interval: incidenceEditorSheet.incidenceWrapper.recurrenceIntervals.Monthly},
                        {key: "yearly", display: i18n("Yearly"), interval: incidenceEditorSheet.incidenceWrapper.recurrenceIntervals.Yearly},
                        {key: "custom", display: i18n("Custom"), interval: -1}
                    ]
                    popup.z: 1000
                }

                Kirigami.FormLayout {
                    id: customRecurrenceLayout

                    Layout.fillWidth: true
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    visible: repeatComboBox.currentIndex > 0 // Not "Never" index

                    function setOcurrence() {
                        incidenceEditorSheet.incidenceWrapper.setRegularRecurrence(recurScaleRuleCombobox.currentValue, recurFreqRuleSpinbox.value);

                        if(recurScaleRuleCombobox.currentValue === incidenceEditorSheet.incidenceWrapper.recurrenceIntervals.Weekly) {
                            weekdayCheckboxRepeater.setWeekdaysRepeat();
                        }
                    }

                    // Custom controls
                    RowLayout {
                        Layout.fillWidth: true
                        Kirigami.FormData.label: i18n("Every:")
                        visible: repeatComboBox.currentIndex === 5

                        QQC2.SpinBox {
                            id: recurFreqRuleSpinbox

                            Layout.fillWidth: true
                            from: 1
                            value: incidenceEditorSheet.incidenceWrapper.recurrenceData.frequency
                            onValueChanged: if(visible) { incidenceEditorSheet.incidenceWrapper.recurrenceData.frequency = value }
                        }
                        QQC2.ComboBox {
                            id: recurScaleRuleCombobox

                            Layout.fillWidth: true
                            visible: repeatComboBox.currentIndex === 5
                            textRole: recurFreqRuleSpinbox.value > 1 ? "displayPlural" : "displaySingular"
                            valueRole: "interval"
                            onCurrentValueChanged: if(visible) { customRecurrenceLayout.setOcurrence(); }
                            currentIndex: {
                                if(incidenceEditorSheet.incidenceWrapper.recurrenceData.type === undefined) {
                                    return -1;
                                }

                                switch(incidenceEditorSheet.incidenceWrapper.recurrenceData.type) {
                                    case 3: // Daily
                                    case 4: // Weekly
                                        return incidenceEditorSheet.incidenceWrapper.recurrenceData.type - 3
                                    case 5: // Monthly on position (e.g. third Monday)
                                    case 6: // Monthly on day (1st of month)
                                        return 2;
                                    case 7: // Yearly on month
                                    case 8: // Yearly on day
                                    case 9: // Yearly on position
                                        return 3;
                                    default:
                                        return -1;
                                }
                            }

                            model: [
                                {key: "day", displaySingular: i18n("day"), displayPlural: i18n("days"), interval: incidenceEditorSheet.incidenceWrapper.recurrenceIntervals.Daily},
                                {key: "week", displaySingular: i18n("week"), displayPlural: i18n("weeks"), interval: incidenceEditorSheet.incidenceWrapper.recurrenceIntervals.Weekly},
                                {key: "month", displaySingular: i18n("month"), displayPlural: i18n("months"), interval: incidenceEditorSheet.incidenceWrapper.recurrenceIntervals.Monthly},
                                {key: "year", displaySingular: i18n("year"), displayPlural: i18n("years"), interval: incidenceEditorSheet.incidenceWrapper.recurrenceIntervals.Yearly},
                            ]
                            delegate: Kirigami.BasicListItem {
                                text: recurFreqRuleSpinbox.value > 1 ? modelData.displayPlural : modelData.displaySingular
                                onClicked: {
                                    incidenceEditorSheet.incidenceWrapper.setRegularRecurrence(modelData.interval, recurFreqRuleSpinbox.value);
                                    repeatComboBox.currentIndex = 5; // Otherwise resets to default daily/weekly/etc.
                                }
                            }
                            popup.z: 1000
                        }
                    }

                    // Custom controls specific to weekly
                    GridLayout {
                        id: recurWeekdayRuleLayout
                        Layout.fillWidth: true

                        columns: 7
                        visible: recurScaleRuleCombobox.currentIndex === 1 && repeatComboBox.currentIndex === 5 // "week"/"weeks" index

                        Repeater {
                            model: 7
                            delegate: QQC2.Label {
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignHCenter
                                text: Qt.locale().dayName(Qt.locale().firstDayOfWeek + index, Locale.ShortFormat)
                            }
                        }

                        Repeater {
                            id: weekdayCheckboxRepeater

                            property var checkboxes: []
                            function setWeekdaysRepeat() {
                                let selectedDays = new Array(7)
                                for(let checkbox of checkboxes) {
                                    // C++ func takes 7 bit array
                                    selectedDays[checkbox.dayNumber] = checkbox.checked
                                }
                                incidenceEditorSheet.incidenceWrapper.recurrenceData.weekdays = selectedDays;
                            }

                            model: 7
                            delegate: QQC2.CheckBox {
                                Layout.alignment: Qt.AlignHCenter
                                // We make sure we get dayNumber per the day of the week number used by C++ Qt
                                property int dayNumber: Qt.locale().firstDayOfWeek + index > 7 ?
                                                        Qt.locale().firstDayOfWeek + index - 1 - 7 :
                                                        Qt.locale().firstDayOfWeek + index - 1

                                checked: incidenceEditorSheet.incidenceWrapper.recurrenceData.weekdays[dayNumber]
                                onClicked: incidenceEditorSheet.incidenceWrapper.recurrenceData.weekdays[dayNumber] = !incidenceEditorSheet.incidenceWrapper.recurrenceData.weekdays[dayNumber]
                            }
                        }
                    }

                    // Controls specific to monthly recurrence
                    QQC2.ButtonGroup {
                        buttons: monthlyRecurRadioColumn.children
                    }

                    ColumnLayout {
                        id: monthlyRecurRadioColumn
                        Kirigami.FormData.label: i18n("On:")

                        Layout.fillWidth: true
                        visible: recurScaleRuleCombobox.currentIndex === 2 && repeatComboBox.currentIndex === 5 // "month/months" index

                        QQC2.RadioButton {
                            property int dateOfMonth: incidenceStartDateCombo.dateFromText.getDate()

                            text: i18nc("%1 is the day number of month", "the %1 of each month", LabelUtils.numberToString(dateOfMonth))

                            checked: incidenceEditorSheet.incidenceWrapper.recurrenceData.type == 6 // Monthly on day (1st of month)
                            onClicked: customRecurrenceLayout.setOcurrence()
                        }
                        QQC2.RadioButton {
                            property int dayOfWeek: incidenceStartDateCombo.dateFromText.getDay() > 0 ?
                                                    incidenceStartDateCombo.dateFromText.getDay() - 1 :
                                                    7 // C++ Qt day of week index goes Mon-Sun, 0-7
                            property int weekOfMonth: Math.ceil((incidenceStartDateCombo.dateFromText.getDate() + 6 - incidenceStartDateCombo.dateFromText.getDay())/7);
                            property string dayOfWeekString: Qt.locale().dayName(incidenceStartDateCombo.dateFromText.getDay())

                            text: i18nc("the weekOfMonth dayOfWeekString of each month", "the %1 %2 of each month", LabelUtils.numberToString(weekOfMonth), dayOfWeekString)
                            checked: incidenceEditorSheet.incidenceWrapper.recurrenceData.type == 5 // Monthly on position
                            onTextChanged: if(checked) { incidenceEditorSheet.incidenceWrapper.setMonthlyPosRecurrence(weekOfMonth, dayOfWeek); }
                            onClicked: incidenceEditorSheet.incidenceWrapper.setMonthlyPosRecurrence(weekOfMonth, dayOfWeek)
                        }
                    }


                    // Repeat end controls (visible on all recurrences)
                    RowLayout {
                        Layout.fillWidth: true
                        Kirigami.FormData.label: i18n("Ends:")

                        QQC2.ComboBox {
                            id: endRecurType

                            Layout.fillWidth: true
                            // Recurrence duration returns -1 for never ending and 0 when the recurrence
                            // end date is set. Any number larger is the set number of recurrences
                            currentIndex: incidenceEditorSheet.incidenceWrapper.recurrenceData.duration <= 0 ?
                                incidenceEditorSheet.incidenceWrapper.recurrenceData.duration + 1 : 2
                            textRole: "display"
                            valueRole: "duration"
                            model: [
                                {display: i18n("Never"), duration: -1},
                                {display: i18n("On"), duration: 0},
                                {display: i18n("After"), duration: 1}
                            ]
                            delegate: Kirigami.BasicListItem {
                                text: modelData.display
                                onClicked: incidenceEditorSheet.incidenceWrapper.recurrenceData.duration = modelData.duration
                            }
                            popup.z: 1000
                        }

                        QQC2.ComboBox {
                            id: recurEndDateCombo

                            Layout.fillWidth: true
                            visible: endRecurType.currentIndex == 1
                            editable: true
                            editText: incidenceEditorSheet.incidenceWrapper.recurrenceData.endDateTime.toLocaleDateString(Qt.locale(), Locale.NarrowFormat);

                            inputMethodHints: Qt.ImhDate

                            property date dateFromText: Date.fromLocaleDateString(Qt.locale(), editText, Locale.NarrowFormat)
                            property bool validDate: !isNaN(dateFromText.getTime())

                            onDateFromTextChanged: {
                                const datePicker = recurEndDatePicker
                                if (validDate && activeFocus) {
                                    datePicker.selectedDate = dateFromText;
                                    datePicker.clickedDate = dateFromText;

                                    if (visible) {
                                        incidenceEditorSheet.incidenceWrapper.recurrenceData.endDateTime = dateFromText
                                    }
                                }
                            }

                            popup: QQC2.Popup {
                                id: recurEndDatePopup

                                width: Kirigami.Units.gridUnit * 18
                                height: Kirigami.Units.gridUnit * 18
                                y: parent.y + parent.height
                                z: 1000

                                DatePicker {
                                    id: recurEndDatePicker
                                    anchors.fill: parent
                                    onDatePicked: {
                                        incidenceEditorSheet.incidenceWrapper.recurrenceData.endDateTime = pickedDate
                                        recurEndDatePopup.close()
                                    }
                                }
                            }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            visible: endRecurType.currentIndex === 2
                            onVisibleChanged: if (visible) { eventEditorSheet.eventWrapper.setRecurrenceOcurrences(recurOcurrenceEndSpinbox.value) }

                            QQC2.SpinBox {
                                id: recurOcurrenceEndSpinbox
                                Layout.fillWidth: true
                                from: 1
                                value: eventEditorSheet.eventWrapper.recurrenceData.duration
                                onValueChanged: eventEditorSheet.eventWrapper.setRecurrenceOcurrences(value)
                            }
                            QQC2.Label {
                                text: i18np("occurrence", "occurrences", recurOcurrenceEndSpinbox.value)
                            }
                        }
                    }

                    ColumnLayout {
                        Kirigami.FormData.label: i18n("Exceptions:")
                        Layout.fillWidth: true

                        QQC2.ComboBox {
                            id: exceptionAddButton
                            Layout.fillWidth: true
                            displayText: i18n("Add recurrence exception")

                            popup: QQC2.Popup {
                                id: recurExceptionPopup

                                width: Kirigami.Units.gridUnit * 18
                                height: Kirigami.Units.gridUnit * 18
                                y: parent.y + parent.height
                                z: 1000

                                DatePicker {
                                    id: recurExceptionPicker
                                    anchors.fill: parent
                                    selectedDate: incidenceStartDateCombo.dateFromText
                                    onDatePicked: {
                                        incidenceEditorSheet.incidenceWrapper.recurrenceExceptionsModel.addExceptionDateTime(pickedDate)
                                        recurExceptionPopup.close()
                                    }
                                }
                            }
                        }

                        Repeater {
                            id: exceptionsRepeater
                            model: incidenceEditorSheet.incidenceWrapper.recurrenceExceptionsModel
                            delegate: RowLayout {
                                Kirigami.BasicListItem {
                                    Layout.fillWidth: true
                                    text: date.toLocaleDateString(Qt.locale())
                                }
                                QQC2.Button {
                                    icon.name: "edit-delete-remove"
                                    onClicked: incidenceEditorSheet.incidenceWrapper.recurrenceExceptionsModel.deleteExceptionDateTime(date)
                                }
                            }
                        }
                    }
                }

                Kirigami.Separator {
                    Kirigami.FormData.isSection: true
                }

                ColumnLayout {
                    id: remindersColumn

                    Kirigami.FormData.label: i18n("Reminders:")
                    Kirigami.FormData.labelAlignment: remindersRepeater.count ? Qt.AlignTop : Qt.AlignVCenter
                    Layout.fillWidth: true

                    Repeater {
                        id: remindersRepeater

                        Layout.fillWidth: true

                        model: incidenceEditorSheet.incidenceWrapper.remindersModel
                        // All of the alarms are handled within the delegates.

                        delegate: RowLayout {
                            Layout.fillWidth: true

                            QQC2.ComboBox {
                                // There is also a chance here to add a feature for the user to pick reminder type.
                                Layout.fillWidth: true

                                property var selectedIndex: 0

                                displayText: LabelUtils.secondsToReminderLabel(startOffset)
                                //textRole: "DisplayNameRole"
                                onCurrentValueChanged: incidenceEditorSheet.incidenceWrapper.remindersModel.setData(incidenceEditorSheet.incidenceWrapper.remindersModel.index(index, 0),
                                                                                                            currentValue,
                                                                                                            incidenceEditorSheet.incidenceWrapper.remindersModel.dataroles.startOffset)
                                onCountChanged: selectedIndex = currentIndex // Gets called *just* before modelChanged
                                onModelChanged: currentIndex = selectedIndex

                                model: [0, // We times by -1 to make times be before incidence
                                        -1 * 5 * 60, // 5 minutes
                                        -1 * 10 * 60,
                                        -1 * 15 * 60,
                                        -1 * 30 * 60,
                                        -1 * 45 * 60,
                                        -1 * 1 * 60 * 60, // 1 hour
                                        -1 * 2 * 60 * 60,
                                        -1 * 1 * 24 * 60 * 60, // 1 day
                                        -1 * 2 * 24 * 60 * 60,
                                        -1 * 5 * 24 * 60 * 60]
                                        // All these times are in seconds.
                                delegate: Kirigami.BasicListItem {
                                    text: LabelUtils.secondsToReminderLabel(modelData)
                                }

                                popup.z: 1000
                            }

                            QQC2.Button {
                                icon.name: "edit-delete-remove"
                                onClicked: incidenceEditorSheet.incidenceWrapper.remindersModel.deleteAlarm(model.index);
                            }
                        }
                    }

                    QQC2.Button {
                        id: remindersButton

                        text: i18n("Add reminder")
                        Layout.fillWidth: true

                        onClicked: eventEditorSheet.eventWrapper.remindersModel.addAlarm();
                    }
                }

                Kirigami.Separator {
                    Kirigami.FormData.isSection: true
                }

                ColumnLayout {
                    id: attendeesColumn

                    Kirigami.FormData.label: i18n("Attendees:")
                    Kirigami.FormData.labelAlignment: attendeesRepeater.count ? Qt.AlignTop : Qt.AlignVCenter
                    Layout.fillWidth: true

                    Repeater {
                        id: attendeesRepeater
                        model: eventEditorSheet.eventWrapper.attendeesModel

                        // All of the alarms are handled within the delegates.
                        Layout.fillWidth: true

                        delegate: Kirigami.AbstractCard {

                            topPadding: Kirigami.Units.smallSpacing
                            bottomPadding: Kirigami.Units.smallSpacing

                            contentItem: Item {
                                implicitWidth: attendeeCardContent.implicitWidth
                                implicitHeight: attendeeCardContent.implicitHeight

                                GridLayout {
                                    id: attendeeCardContent

                                    anchors {
                                        left: parent.left
                                        top: parent.top
                                        right: parent.right
                                        //IMPORTANT: never put the bottom margin
                                    }

                                    columns: 6
                                    rows: 4

                                    QQC2.Label{
                                        Layout.row: 0
                                        Layout.column: 0
                                        text: i18n("Name:")
                                    }
                                    QQC2.TextField {
                                        Layout.fillWidth: true
                                        Layout.row: 0
                                        Layout.column: 1
                                        Layout.columnSpan: 4
                                        text: model.name
                                        onTextChanged: eventEditorSheet.eventWrapper.attendeesModel.setData(eventEditorSheet.eventWrapper.attendeesModel.index(index, 0),
                                                                                            text,
                                                                                            eventEditorSheet.eventWrapper.attendeesModel.dataroles.name)
                                    }

                                    QQC2.Button {
                                        Layout.alignment: Qt.AlignTop
                                        Layout.column: 5
                                        Layout.row: 0
                                        icon.name: "edit-delete-remove"
                                        onClicked: eventEditorSheet.eventWrapper.attendeesModel.deleteAttendee(index);
                                    }

                                    QQC2.Label {
                                        Layout.row: 1
                                        Layout.column: 0
                                        text: i18n("Email:")
                                    }
                                    QQC2.TextField {
                                        Layout.fillWidth: true
                                        Layout.row: 1
                                        Layout.column: 1
                                        Layout.columnSpan: 4
                                        text: model.email
                                        onTextChanged: eventEditorSheet.eventWrapper.attendeesModel.setData(eventEditorSheet.eventWrapper.attendeesModel.index(index, 0),
                                                                                            text,
                                                                                            eventEditorSheet.eventWrapper.attendeesModel.dataroles.email)
                                    }
                                    QQC2.Label {
                                        Layout.row: 2
                                        Layout.column: 0
                                        text: i18n("Status:")
                                        visible: eventEditorSheet.editMode
                                    }
                                    QQC2.ComboBox {
                                        Layout.fillWidth: true
                                        Layout.row: 2
                                        Layout.column: 1
                                        Layout.columnSpan: 2
                                        model: eventEditorSheet.eventWrapper.attendeesModel.attendeeStatusModel
                                        textRole: "display"
                                        valueRole: "value"
                                        currentIndex: status // role of parent
                                        onCurrentValueChanged: eventEditorSheet.eventWrapper.attendeesModel.setData(eventEditorSheet.eventWrapper.attendeesModel.index(index, 0),
                                                                                            currentValue,
                                                                                            eventEditorSheet.eventWrapper.attendeesModel.dataroles.status)

                                        popup.z: 1000
                                        visible: eventEditorSheet.editMode
                                    }
                                    QQC2.CheckBox {
                                        Layout.fillWidth: true
                                        Layout.row: 2
                                        Layout.column: 3
                                        Layout.columnSpan: 2
                                        text: i18n("Request RSVP")
                                        checked: model.rsvp
                                        onCheckedChanged: eventEditorSheet.eventWrapper.attendeesModel.setData(eventEditorSheet.eventWrapper.attendeesModel.index(index, 0),
                                                                                            checked,
                                                                                            eventEditorSheet.eventWrapper.attendeesModel.dataroles.rsvp)
                                        visible: eventEditorSheet.editMode
                                    }
                                }
                            }
                        }
                    }

                    QQC2.Button {
                        id: attendeesButton
                        text: i18n("Add attendee")
                        Layout.fillWidth: true

                        onClicked: eventEditorSheet.eventWrapper.attendeesModel.addAttendee();
                    }
                }

                Kirigami.Separator {
                    Kirigami.FormData.isSection: true
                }

                ColumnLayout {
                    id: attachmentsColumn

                    Kirigami.FormData.label: i18n("Attachments:")
                    Kirigami.FormData.labelAlignment: attachmentsRepeater.count ? Qt.AlignTop : Qt.AlignVCenter
                    Layout.fillWidth: true

                    Repeater {
                        id: attachmentsRepeater
                        model: eventEditorSheet.eventWrapper.attachmentsModel
                        delegate: RowLayout {
                            Kirigami.BasicListItem {
                                Layout.fillWidth: true
                                icon: iconName // Why isn't this icon.name??
                                label: attachmentLabel
                                onClicked: Qt.openUrlExternally(uri)
                            }
                            QQC2.Button {
                                icon.name: "edit-delete-remove"
                                onClicked: eventEditorSheet.eventWrapper.attachmentsModel.deleteAttachment(uri)
                            }
                        }
                    }

                    QQC2.Button {
                        id: attachmentsButton
                        text: i18n("Add attachment")
                        Layout.fillWidth: true
                        onClicked: attachmentFileDialog.open();

                        FileDialog {
                            id: attachmentFileDialog

                            title: "Add an attachment"
                            folder: shortcuts.home
                            onAccepted: incidenceEditorSheet.incidenceWrapper.attachmentsModel.addAttachment(fileUrls)
                        }
                    }

                    Repeater {
                        id: attachmentsRepeater
                        model: incidenceEditorSheet.incidenceWrapper.attachmentsModel
                        delegate: RowLayout {
                            QQC2.Label {
                                Layout.fillWidth: true
                                text: attachmentLabel
                            }
                            QQC2.Button {
                                icon.name: "edit-delete-remove"
                                onClicked: incidenceEditorSheet.incidenceWrapper.attachmentsModel.deleteAttachment(uri)
                            }
                        }
                    }
                }
            }
        }
    }
}
