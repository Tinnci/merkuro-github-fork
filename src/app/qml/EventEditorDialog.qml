import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15

Dialog {
    id: root
    title: isEditMode ? "Edit Event" : "New Event"
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    modal: true
    focus: true
    standardButtons: Dialog.Ok | Dialog.Cancel

    property bool isEditMode: false
    property string eventUid: ""
    property date eventDate: new Date()

    // Properties to hold current values
    property alias eventTitle: titleField.text
    property alias eventDescription: descriptionField.text
    property alias eventLocation: locationField.text
    property string eventCalendarId: ""
    property bool allDay: allDaySwitch.checked

    // Helper function to get full Date objects from UI
    function getStartDateTime() {
        let d = new Date(eventDate);
        d.setHours(startHour.value);
        d.setMinutes(startMinute.value);
        return d;
    }

    function getEndDateTime() {
        let d = new Date(eventDate);
        d.setHours(endHour.value);
        d.setMinutes(endMinute.value);
        return d;
    }

    // Set UI values from Date objects
    function setTimes(start, end) {
        startHour.value = start.getHours();
        startMinute.value = start.getMinutes();

        endHour.value = end.getHours();
        endMinute.value = end.getMinutes();
    }

    onAccepted: {
        var start = getStartDateTime();
        var end = getEndDateTime();

        if (isEditMode) {
            CalendarApp.updateEvent(eventUid, titleField.text, start, end, allDaySwitch.checked, 
                                    descriptionField.text, locationField.text);
        } else {
            CalendarApp.createEvent(titleField.text, start, end, allDaySwitch.checked, 
                                   descriptionField.text, locationField.text, calendarSelector.currentValue);
        }

        // Reset fields
        titleField.text = "";
        descriptionField.text = "";
        locationField.text = "";
        isEditMode = false;
        eventUid = "";
    }
    
    // Refresh calendars when opened
    onOpened: {
        calendarModel.clear()
        var cals = CalendarApp.getCalendars()
        var foundIndex = -1
        for (var i = 0; i < cals.length; i++) {
            calendarModel.append(cals[i])
            if (cals[i].id === eventCalendarId) {
                foundIndex = i
            }
        }
        if (foundIndex !== -1) {
            calendarSelector.currentIndex = foundIndex
        } else if (cals.length > 0) {
            calendarSelector.currentIndex = 0
        }
    }
    
    ListModel {
        id: calendarModel
    }

    ColumnLayout {
        spacing: 15

        TextField {
            id: titleField
            placeholderText: "Event Title"
            Layout.preferredWidth: 300
            focus: true
            selectByMouse: true
        }

        RowLayout {
            Label { text: "All Day" }
            Switch {
                id: allDaySwitch
                onCheckedChanged: {
                    timeControls.enabled = !checked
                }
            }
        }

        GridLayout {
            id: timeControls
            columns: 2
            rowSpacing: 10
            columnSpacing: 10

            Label { text: "Start:" }
            RowLayout {
                SpinBox {
                    id: startHour
                    from: 0; to: 23
                    value: 12
                    editable: true
                }
                Label { text: ":" }
                SpinBox {
                    id: startMinute
                    from: 0; to: 55
                    stepSize: 5
                    value: 0
                    editable: true
                }
            }

            Label { text: "End:" }
            RowLayout {
                SpinBox {
                    id: endHour
                    from: 0; to: 23
                    value: 13
                    editable: true
                }
                Label { text: ":" }
                SpinBox {
                    id: endMinute
                    from: 0; to: 55
                    stepSize: 5
                    value: 0
                    editable: true
                }
            }
        }
        TextField {
            id: locationField
            placeholderText: "Location"
            Layout.fillWidth: true
            selectByMouse: true
        }

        TextArea {
            id: descriptionField
            placeholderText: "Description"
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            selectByMouse: true
            background: Rectangle {
                border.color: Material.color(Material.Grey, Material.Shade300)
                radius: 4
            }
        }

        RowLayout {
            Label { text: "Calendar:" }
            ComboBox {
                id: calendarSelector
                Layout.fillWidth: true
                model: calendarModel
                textRole: "name"
                valueRole: "id"
                enabled: !isEditMode // Lock calendar for editing
            }
        }
    }
}
