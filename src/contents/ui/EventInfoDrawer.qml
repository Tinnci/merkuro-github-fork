import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

Kirigami.OverlayDrawer {
    id: eventInfo

    property var eventData
    property var collectionData

    edge: Qt.RightEdge
    modal: true
    handleVisible: this.visible

    contentItem: QQC2.ScrollView {
        id: contentView

        anchors.fill: parent

        Column {
            id: detailsColumn
            Layout.fillWidth: true
            padding: Kirigami.Units.largeSpacing

            property real usableWidth: contentView.availableWidth - ((padding * 3) + descriptionLabel.width)

            Kirigami.Heading {
                Layout.maximumWidth: parent.width
                text: "<b>" + eventInfo.eventData.text + "</b>"
                level: 3
                wrapMode: Text.Wrap
            }
            GridLayout {
                Layout.maximumWidth: parent.width

                columns:2

                QQC2.Label {
                    Layout.alignment: Qt.AlignTop
                    text: i18n("<b>Calendar:</b>")
                }
                QQC2.Label {
                    Layout.alignment: Qt.AlignTop
                    Layout.maximumWidth: parent.width
                    text: eventInfo.collectionData["displayName"]
                    wrapMode: Text.Wrap
                }

                QQC2.Label {
                    Layout.alignment: Qt.AlignTop
                    text: i18n("<b>Date:</b>")
                }
                QQC2.Label {
                    Layout.alignment: Qt.AlignTop
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 10
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
                    Layout.maximumWidth: parent.width
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
                    Layout.maximumWidth: parent.width
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
                    Layout.maximumWidth: Kirigami.Units.gridUnit * 20
                    text: eventInfo.eventData.description
                    wrapMode: Text.Wrap
                    visible: eventInfo.eventData.description
                }
            }
        }
    }
}
