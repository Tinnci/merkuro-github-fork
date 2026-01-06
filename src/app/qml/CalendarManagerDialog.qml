import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15

Dialog {
    id: root
    title: "Manage Calendars"
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: 400
    height: 500
    modal: true
    focus: true
    standardButtons: Dialog.Close

    function refresh() {
        calendarModel.clear()
        var calendars = CalendarApp.getCalendars()
        for (var i = 0; i < calendars.length; i++) {
            calendarModel.append(calendars[i])
        }
    }

    onOpened: refresh()

    ListModel {
        id: calendarModel
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: calendarModel

            delegate: ItemDelegate {
                width: parent.width

                contentItem: RowLayout {
                    CheckBox {
                        checked: model.visible
                        onToggled: {
                            model.visible = checked
                            CalendarApp.setCalendarVisibility(model.id, checked)
                        }
                    }

                    Rectangle {
                        width: 16
                        height: 16
                        radius: 8
                        color: model.color
                        border.color: "grey"
                        border.width: 1
                    }

                    Label {
                        text: model.name
                        Layout.fillWidth: true
                    }

                    Button {
                        text: "Delete"
                        flat: true
                        visible: model.id !== "personal" // Prevent deleting default
                        onClicked: {
                            CalendarApp.deleteCalendar(model.id)
                            root.refresh()
                        }
                    }
                }
            }
        }

        RowLayout {
            TextField {
                id: newCalName
                placeholderText: "New Calendar Name"
                Layout.fillWidth: true
            }

            // Simple Color Picker (ComboBox)
            ComboBox {
                id: colorPicker
                model: ["#2196F3", "#F44336", "#4CAF50", "#FFC107", "#9C27B0"]
                delegate: ItemDelegate {
                    width: parent.width
                    contentItem: RowLayout {
                        Rectangle { width: 20; height: 20; color: modelData }
                        Label { text: modelData }
                    }
                }
            }

            Button {
                text: "Add"
                enabled: newCalName.text.length > 0
                onClicked: {
                    CalendarApp.createCalendar(newCalName.text, colorPicker.currentValue)
                    newCalName.text = ""
                    root.refresh()
                }
            }
        }
    }
}
