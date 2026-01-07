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

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Header Bar
        Pane {
            Layout.fillWidth: true
            Material.elevation: 2
            padding: 8

            RowLayout {
                anchors.fill: parent
                spacing: 10

                Label {
                    text: "Personal Calendar"
                    font.pixelSize: 20
                    font.bold: true
                }

                Label {
                    text: "(" + CalendarApp.backendName + ")"
                    color: Material.secondaryTextColor
                    font.pixelSize: 14
                }

                Item { Layout.fillWidth: true }

                Button {
                    text: "⟳ Sync"
                    flat: true
                    onClicked: CalendarApp.sync()
                }

                ToolButton {
                    text: "⚙"
                    font.pixelSize: 20
                    onClicked: settingsMenu.open()

                    Menu {
                        id: settingsMenu
                        y: parent.height

                        MenuItem {
                            text: "Use Local Backend"
                            checkable: true
                            checked: CalendarApp.backendName === "Local"
                            onTriggered: CalendarApp.switchBackend("local")
                        }
                        MenuItem {
                            text: "Use Akonadi Backend"
                            checkable: true
                            checked: CalendarApp.backendName === "Akonadi"
                            onTriggered: CalendarApp.switchBackend("akonadi")
                        }
                        MenuSeparator {}
                        MenuItem {
                            text: "Manage Calendars..."
                            onTriggered: calendarManagerDialog.open()
                        }
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
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
                        onClicked: CalendarApp.monthModel.previous()
                    }

                    Label {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        text: {
                            var date = new Date(CalendarApp.monthModel.year, CalendarApp.monthModel.month - 1, 1);
                            return Qt.formatDate(date, "MMMM yyyy");
                        }
                        font.pixelSize: 18
                        font.bold: true
                    }

                    ToolButton {
                        text: ">"
                        onClicked: CalendarApp.monthModel.next()
                    }

                    ToolButton {
                        text: "Today"
                        onClicked: CalendarApp.monthModel.goToday()
                        flat: true
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 0
                    Repeater {
                        model: CalendarApp.monthModel.weekDays
                        delegate: Label {
                            Layout.fillWidth: true
                            text: modelData
                            horizontalAlignment: Text.AlignHCenter
                            font.bold: true
                            font.pixelSize: 12
                            color: Material.secondaryTextColor
                        }
                    }
                }

                GridView {
                    id: monthGrid
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    cellWidth: width / 7
                    cellHeight: height / 6
                    clip: true
                    model: CalendarApp.monthModel

                    delegate: Item {
                        width: monthGrid.cellWidth
                        height: monthGrid.cellHeight

                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: 2
                            radius: width / 2
                            color: model.isSelected ? Material.accent : (model.isToday ? Material.color(Material.Grey, Material.Shade200) : "transparent")
                            visible: model.isSelected || model.isToday
                        }

                        Label {
                            anchors.centerIn: parent
                            text: model.dayNumber
                            opacity: model.sameMonth ? 1.0 : 0.3
                            color: model.isSelected ? "white" : Material.foreground
                            font.bold: model.isToday
                        }

                        // Event indicator dot
                        Rectangle {
                            anchors.bottom: parent.bottom
                            anchors.bottomMargin: 4
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: 4
                            height: 4
                            radius: 2
                            color: model.isSelected ? "white" : Material.accent
                            visible: model.hasEvents
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: CalendarApp.monthModel.selected = model.date
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

                        // Edit Event on Click
                        onClicked: {
                            editorDialog.isEditMode = true
                            editorDialog.eventUid = model.uid
                            editorDialog.eventTitle = model.title
                            editorDialog.eventDescription = model.description || ""
                            editorDialog.eventLocation = model.location || ""
                            editorDialog.eventCalendarId = model.calendarId || ""
                            editorDialog.eventDate = model.startDate
                            editorDialog.allDay = model.isAllDay
                            editorDialog.setTimes(model.startDate, model.endDate)
                            editorDialog.open()
                        }

                        contentItem: RowLayout {
                            spacing: 12
                            
                            Rectangle {
                                Layout.preferredWidth: 4
                                Layout.fillHeight: true
                                color: model.color || Material.accent
                                radius: 2
                            }
                            
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2

                                Label {
                                    text: model.title
                                    font.bold: true
                                    font.pixelSize: 16
                                    elide: Text.ElideRight
                                }

                                Label {
                                    text: {
                                        var timeStr = model.isAllDay ? "All Day" : 
                                            Qt.formatTime(model.startDate, "hh:mm") + " - " + Qt.formatTime(model.endDate, "hh:mm");
                                        if (model.location) {
                                            return timeStr + " • " + model.location;
                                        }
                                        return timeStr;
                                    }
                                    color: Material.secondaryTextColor
                                    elide: Text.ElideRight
                                }
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
                    editorDialog.eventDescription = ""
                    editorDialog.eventLocation = ""
                    editorDialog.eventCalendarId = ""
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
