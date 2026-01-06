import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15
import Qt.labs.calendar 1.0

ApplicationWindow {
    id: window
    width: 800
    height: 600
    visible: true
    title: "Personal Calendar"

    Material.theme: Material.Light
    Material.accent: Material.Purple

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Left Panel: Calendar Picker
        Pane {
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            Material.elevation: 4
            padding: 0

            ColumnLayout {
                anchors.fill: parent
                spacing: 10

                // Month Navigation
                RowLayout {
                    Layout.fillWidth: true
                    Layout.margins: 10

                    ToolButton {
                        text: "<"
                        onClicked: calendar.month -= 1
                    }

                    Label {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        text: calendar.title
                        font.pixelSize: 18
                        font.bold: true
                    }

                    ToolButton {
                        text: ">"
                        onClicked: calendar.month += 1
                    }
                }

                // Days Header
                DayOfWeekRow {
                    locale: calendar.locale
                    Layout.fillWidth: true
                    delegate: Label {
                        text: model.shortName
                        horizontalAlignment: Text.AlignHCenter
                        font.bold: true
                    }
                }

                // Calendar Grid
                MonthGrid {
                    id: calendar
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    month: new Date().getMonth()
                    year: new Date().getFullYear()

                    delegate: Item {
                        readonly property bool isSelected:
                            model.date.getTime() === CalendarApp.eventsModel.selectedDate.getTime()

                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: 2
                            radius: width / 2
                            color: isSelected ? Material.accent : "transparent"

                            Label {
                                anchors.centerIn: parent
                                text: model.day
                                color: isSelected ? "white" : "black"
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                CalendarApp.eventsModel.selectedDate = model.date
                            }
                        }
                    }
                }
            }
        }

        // Right Panel: Events List
        Pane {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {
                anchors.fill: parent

                Label {
                    text: Qt.formatDate(CalendarApp.eventsModel.selectedDate, "dddd, MMMM d, yyyy")
                    font.pixelSize: 24
                    font.bold: true
                    Layout.bottomMargin: 10
                }

                ListView {
                    id: eventsList
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: CalendarApp.eventsModel
                    spacing: 10

                    delegate: ItemDelegate {
                        width: parent.width

                        contentItem: ColumnLayout {
                            spacing: 2

                            Label {
                                text: model.title
                                font.bold: true
                                font.pixelSize: 16
                            }

                            Label {
                                text: {
                                    if (model.isAllDay) return "All Day";
                                    return Qt.formatTime(model.startDate, "hh:mm") + " - " +
                                           Qt.formatTime(model.endDate, "hh:mm");
                                }
                                color: Material.secondaryTextColor
                            }
                        }

                        background: Rectangle {
                            color: parent.highlighted ? Material.listHighlightColor : "transparent"
                            border.color: Material.color(Material.Grey, Material.Shade300)
                            border.width: 1
                            radius: 4
                        }

                        // Right-click / Long-press menu to delete
                        MouseArea {
                            anchors.fill: parent
                            acceptedButtons: Qt.RightButton
                            onClicked: {
                                deleteMenu.targetUid = model.uid
                                deleteMenu.popup()
                            }
                        }
                    }
                }
            }

            // Floating Action Button
            RoundButton {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 20
                width: 56
                height: 56
                highlighted: true
                text: "+"
                font.pixelSize: 24

                onClicked: addEventDialog.open()
            }
        }
    }

    // Add Event Dialog
    Dialog {
        id: addEventDialog
        title: "New Event"
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        modal: true
        focus: true
        standardButtons: Dialog.Ok | Dialog.Cancel

        property alias eventTitle: titleField.text

        onAccepted: {
            // Default to 1 hour event starting now
            var start = new Date(CalendarApp.eventsModel.selectedDate)
            start.setHours(12) // Default noon

            var end = new Date(start)
            end.setHours(13)

            CalendarApp.createEvent(titleField.text, start, end, false)
            titleField.text = "" // Reset
        }

        ColumnLayout {
            spacing: 20

            TextField {
                id: titleField
                placeholderText: "Event Title"
                Layout.preferredWidth: 300
                focus: true
            }

            // Add more fields later (time picker, etc)
        }
    }

    Menu {
        id: deleteMenu
        property string targetUid: ""
        MenuItem {
            text: "Delete"
            onTriggered: CalendarApp.deleteEvent(deleteMenu.targetUid)
        }
    }
}
