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
            CalendarApp.updateEvent(eventUid, titleField.text, start, end, allDaySwitch.checked);
        } else {
            CalendarApp.createEvent(titleField.text, start, end, allDaySwitch.checked);
        }

        // Reset fields
        titleField.text = "";
        isEditMode = false;
        eventUid = "";
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
    }
}
