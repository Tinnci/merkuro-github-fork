import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

Kirigami.ScrollablePage {
    id: eventInfo

    property var eventData
    property var collectionData
    Layout.maximumWidth: Kirigami.Units.gridUnit * 30

    ColumnLayout {
        id: detailsColumn
        Layout.fillWidth: true

        Kirigami.Heading {
            Layout.fillWidth: true
            text: "<b>" + eventInfo.eventData.text + "</b>"
            level: 1
            wrapMode: Text.Wrap
        }
        GridLayout {
            Layout.fillWidth: true

            columns:2

            QQC2.Label {
                Layout.alignment: Qt.AlignTop
                text: i18n("<b>Calendar:</b>")
            }
            QQC2.Label {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                text: eventInfo.collectionData["displayName"]
                wrapMode: Text.Wrap
            }

            QQC2.Label {
                Layout.alignment: Qt.AlignTop
                text: i18n("<b>Date:</b>")
            }
            QQC2.Label {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                text: eventInfo.eventData.startTime.toDateString() == eventInfo.eventData.endTime.toDateString() ?
                eventInfo.eventData.startTime.toLocaleDateString(Qt.locale()) :
                eventInfo.eventData.startTime.toLocaleDateString(Qt.locale()) + " - " + eventInfo.eventData.endTime.toLocaleDateString(Qt.locale())
                wrapMode: Text.Wrap
            }

            QQC2.Label {
                Layout.alignment: Qt.AlignTop
                text: i18n("<b>Time: </b>")
                visible: !eventInfo.eventData.allDay &&
                eventInfo.eventData.startTime.toDateString() == eventInfo.eventData.endTime.toDateString()
            }
            QQC2.Label {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                text: {
                    if(eventInfo.eventData.startTime.toTimeString() != eventInfo.eventData.endTime.toTimeString()) {
                        eventInfo.eventData.startTime.toLocaleTimeString(Qt.locale(), Locale.ShortFormat) + " - " + eventInfo.eventData.endTime.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
                    } else if (eventInfo.eventData.startTime.toTimeString() == eventInfo.eventData.endTime.toTimeString()) {
                        eventInfo.eventData.startTime.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
                    }
                }
                wrapMode: Text.Wrap
                visible: !eventInfo.eventData.allDay &&
                eventInfo.eventData.startTime.toDateString() == eventInfo.eventData.endTime.toDateString()
            }

            QQC2.Label {
                Layout.alignment: Qt.AlignTop
                text: i18n("<b>Duration: </b>")
                visible: eventInfo.eventData.durationString
            }
            QQC2.Label {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                text: eventInfo.eventData.durationString
                visible: eventInfo.eventData.durationString
                wrapMode: Text.Wrap
            }

            QQC2.Label {
                id: descriptionLabel
                Layout.alignment: Qt.AlignTop
                text: i18n("<b>Description: </b>")
                visible: eventInfo.eventData.description
            }
            QQC2.Label {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                text: eventInfo.eventData.description
                wrapMode: Text.Wrap
                visible: eventInfo.eventData.description
            }
        }
    }
}
