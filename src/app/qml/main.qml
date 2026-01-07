import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls.Material 2.15

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
                        onClicked: console.log("Prev Month") // calendar.month -= 1
                    }

                    Label {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        text: "January 2026" // calendar.title
                        font.pixelSize: 18
                        font.bold: true
                    }

                    ToolButton {
                        text: ">"
                        onClicked: console.log("Next Month") // calendar.month += 1
                    }
                }

                // Days Header
                Label {
                    text: "S M T W T F S"
                    horizontalAlignment: Text.AlignHCenter
                    font.bold: true
                    Layout.fillWidth: true
                }

                // Calendar Grid Placeholder
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#f0f0f0"

                    Label {
                        anchors.centerIn: parent
                        text: "Calendar Grid (Placeholder)"
                    }
                }

                Button {
                    text: "Manage Calendars"
                    Layout.alignment: Qt.AlignHCenter
                    Layout.bottomMargin: 10
                    onClicked: calendarManagerDialog.open()
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

                        // Edit Event on Click
                        onClicked: {
                            editorDialog.isEditMode = true
                            editorDialog.eventUid = model.uid
                            editorDialog.eventTitle = model.title
                            editorDialog.eventDate = model.startDate
                            editorDialog.allDay = model.isAllDay
                            editorDialog.setTimes(model.startDate, model.endDate)
                            editorDialog.open()
                        }

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

                onClicked: {
                    editorDialog.isEditMode = false
                    editorDialog.eventUid = ""
                    editorDialog.eventTitle = ""
                    editorDialog.eventDate = CalendarApp.eventsModel.selectedDate
                    editorDialog.allDay = false

                    // Default times: 12:00 - 13:00
                    var start = new Date(CalendarApp.eventsModel.selectedDate)
                    start.setHours(12)
                    start.setMinutes(0)
                    var end = new Date(start)
                    end.setHours(13)
                    end.setMinutes(0)
                    editorDialog.setTimes(start, end)

                    editorDialog.open()
                }
            }
        }
    }

    // Event Editor Dialog
    EventEditorDialog {
        id: editorDialog
    }

    // Calendar Manager Dialog
    CalendarManagerDialog {
        id: calendarManagerDialog
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
