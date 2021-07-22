import QtQuick 2.15
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import "labelutils.js" as LabelUtils

import org.kde.kalendar 1.0

Kirigami.OverlayDrawer {
    id: incidenceInfo

    signal editIncidence(var incidencePtr, var collectionId)
    signal deleteIncidence(var incidencePtr, date deleteDate)

    /**
     * We use both incidenceData and incidenceWrapper to get info about the occurrence.
     * IncidenceData contains information about the specific occurrence (i.e. date of occurrence)
     * as well as some general data about the incidence such as summary and description.
     *
     * The incidenceWrapper contains more indepth data about reminders, attendees, etc. that is
     * general to the incidence as a whole, not a specific occurrence.
     **/

    property var incidenceData
    property var incidenceWrapper
    property var collectionData

    onIncidenceDataChanged: {
        incidenceWrapper = Qt.createQmlObject('import org.kde.kalendar 1.0; IncidenceWrapper {id: incidence}',
                                          incidenceInfo,
                                          "incidence");
        incidenceWrapper.incidencePtr = incidenceData.incidencePtr;
    }

    enabled: true
    interactive: enabled
    edge: Qt.application.layoutDirection == Qt.RightToLeft ? Qt.LeftEdge : Qt.RightEdge

    topPadding: 0
    leftPadding: 0
    rightPadding: 0

    Kirigami.Theme.colorSet: Kirigami.Theme.View

    contentItem: Loader {
        Layout.fillWidth: true
        Layout.fillHeight: true

        active: incidenceInfo.drawerOpen
        sourceComponent: QQC2.ScrollView {
            id: contentsView
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.rightMargin: Kirigami.Units.largeSpacing *5
            contentWidth: this.availableWidth

            ColumnLayout {
                id: detailsColumn
                Layout.fillWidth: true
                Layout.maximumWidth: contentsView.availableWidth
                Layout.minimumWidth: contentsView.availableWidth

                Kirigami.AbstractApplicationHeader {
                    Layout.fillWidth: true
                    topPadding: Kirigami.Units.smallSpacing / 2;
                    bottomPadding: Kirigami.Units.smallSpacing / 2;
                    rightPadding: Kirigami.Units.smallSpacing
                    leftPadding: Kirigami.Units.smallSpacing


                    RowLayout {
                        anchors.fill: parent
                        spacing: 0

                        Kirigami.Heading {
                            Layout.fillWidth: true
                            text: i18n(incidenceInfo.incidenceWrapper.incidenceTypeStr)
                        }

                        QQC2.ToolButton {
                            icon.name: "edit-entry"
                            text:i18n("Edit")
                            enabled: !incidenceInfo.collectionData.readOnly
                            onClicked: editIncidence(incidenceInfo.incidenceData.incidencePtr, incidenceInfo.incidenceData.collectionId)
                        }
                        QQC2.ToolButton {
                            icon.name: "edit-delete"
                            text:i18n("Delete")
                            enabled: !incidenceInfo.collectionData.readOnly
                            onClicked: deleteIncidence(incidenceInfo.incidenceData.incidencePtr, incidenceInfo.incidenceData.startTime)
                        }
                    }
                }

                GridLayout {
                    id: infoBody

                    Layout.margins: Kirigami.Units.largeSpacing
                    Layout.fillWidth: true
                    Layout.maximumWidth: contentsView.availableWidth - (Kirigami.Units.largeSpacing * 2)
                    Layout.minimumWidth: contentsView.availableWidth - (Kirigami.Units.largeSpacing * 2)

                    columns:2

                    RowLayout {
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        Layout.maximumWidth: contentsView.availableWidth - (Kirigami.Units.largeSpacing * 2)
                        Layout.minimumWidth: contentsView.availableWidth - (Kirigami.Units.largeSpacing * 2)

                        Kirigami.Heading {
                            Layout.fillWidth: true

                            text: "<b>" + incidenceInfo.incidenceData.text + "</b>"
                            wrapMode: Text.Wrap
                        }
                        Kirigami.Icon {
                            source: "tag-incidences"
                            // TODO: This will need dynamic changing with implementation of to-dos/journals
                        }
                        Kirigami.Icon {
                            source: "appointment-recurring"
                            visible: incidenceInfo.incidenceWrapper.recurrenceData.type
                        }
                        Kirigami.Icon {
                            source: "appointment-reminder"
                            visible: incidenceInfo.incidenceWrapper.remindersModel.rowCount() > 0
                        }
                    }
                    Rectangle {
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        height: Kirigami.Units.gridUnit / 2

                        color: incidenceInfo.incidenceData.color
                    }

                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        text: i18n("<b>Calendar:</b>")
                    }
                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        Layout.fillWidth: true

                        text: incidenceInfo.collectionData.displayName
                        wrapMode: Text.Wrap
                    }

                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        text: i18n("<b>Date:</b>")
                    }
                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        Layout.fillWidth: true

                        text: incidenceInfo.incidenceData.startTime.toDateString() == incidenceInfo.incidenceData.endTime.toDateString() ?
                        incidenceInfo.incidenceData.startTime.toLocaleDateString(Qt.locale()) :
                        incidenceInfo.incidenceData.startTime.toLocaleDateString(Qt.locale()) + " - " + incidenceInfo.incidenceData.endTime.toLocaleDateString(Qt.locale())
                        wrapMode: Text.Wrap
                    }

                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        text: i18n("<b>Time:</b>")
                        visible: !incidenceInfo.incidenceData.allDay &&
                        incidenceInfo.incidenceData.startTime.toDateString() == incidenceInfo.incidenceData.endTime.toDateString()
                    }
                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        Layout.fillWidth: true

                        text: {
                            if(incidenceInfo.incidenceData.startTime.toTimeString() != incidenceInfo.incidenceData.endTime.toTimeString()) {
                                incidenceInfo.incidenceData.startTime.toLocaleTimeString(Qt.locale(), Locale.ShortFormat) + " - " + incidenceInfo.incidenceData.endTime.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
                            } else if (incidenceInfo.incidenceData.startTime.toTimeString() == incidenceInfo.incidenceData.endTime.toTimeString()) {
                                incidenceInfo.incidenceData.startTime.toLocaleTimeString(Qt.locale(), Locale.ShortFormat)
                            }
                        }
                        wrapMode: Text.Wrap
                        visible: !incidenceInfo.incidenceData.allDay &&
                        incidenceInfo.incidenceData.startTime.toDateString() == incidenceInfo.incidenceData.endTime.toDateString()
                    }

                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        text: i18n("<b>Duration:</b>")
                        visible: incidenceInfo.incidenceData.durationString
                    }
                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        Layout.fillWidth: true

                        text: incidenceInfo.incidenceData.durationString
                        visible: incidenceInfo.incidenceData.durationString
                        wrapMode: Text.Wrap
                    }

                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        text: i18n("<b>Recurrence:</b>")
                        visible: incidenceInfo.incidenceWrapper.recurrenceData.type
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        visible: incidenceInfo.incidenceWrapper.recurrenceData.type

                        QQC2.Label {
                            Layout.alignment: Qt.AlignTop
                            Layout.fillWidth: true

                            text: LabelUtils.recurrenceToString(incidenceInfo.incidenceWrapper.recurrenceData)
                            wrapMode: Text.Wrap
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            visible: exceptionsRepeater.count

                            QQC2.Label {
                                Layout.alignment: Qt.AlignTop
                                visible: exceptionsRepeater.count

                                text: i18n("Except on:")

                            }
                            ColumnLayout {
                                Layout.fillWidth: true

                                Repeater {
                                    id: exceptionsRepeater
                                    model: incidenceInfo.incidenceWrapper.recurrenceExceptionsModel
                                    delegate: QQC2.Label {
                                        Layout.fillWidth: true
                                        text: date.toLocaleDateString(Qt.locale())
                                    }
                                }
                            }
                        }

                        QQC2.Label {
                            Layout.alignment: Qt.AlignTop
                            Layout.fillWidth: true
                            visible: incidenceInfo.incidenceWrapper.recurrenceData.duration > -1

                            text: LabelUtils.recurrenceEndToString(incidenceInfo.incidenceWrapper.recurrenceData)
                            wrapMode: Text.Wrap
                        }
                    }

                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        text: i18n("<b>Location:</b>")
                        visible: incidenceInfo.incidenceWrapper.location
                    }
                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        Layout.fillWidth: true

                        textFormat: Text.MarkdownText
                        text: incidenceInfo.incidenceWrapper.location.replace(LabelUtils.urlRegexp, (match) => `[${match}](${match})`)
                        onLinkActivated: Qt.openUrlExternally(link)
                        wrapMode: Text.Wrap
                        visible: incidenceInfo.incidenceWrapper.location
                    }

                    QQC2.Label {
                        id: descriptionLabel
                        Layout.alignment: Qt.AlignTop

                        text: i18n("<b>Description:</b>")
                        visible: incidenceInfo.incidenceWrapper.description
                    }
                    QQC2.Label {
                        id: descriptionText
                        Layout.alignment: Qt.AlignTop
                        Layout.fillWidth: true

                        textFormat: Text.MarkdownText
                        text: incidenceInfo.incidenceWrapper.description.replace(LabelUtils.urlRegexp, (match) => `[${match}](${match})`)
                        onLinkActivated: Qt.openUrlExternally(link)
                        wrapMode: Text.Wrap
                        visible: incidenceInfo.incidenceWrapper.description
                    }

                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        text: i18np("<b>Attachment:</b>", "<b>Attachments:</b>", incidenceInfo.incidenceWrapper.attachmentsModel.rowCount())
                        visible: incidenceInfo.incidenceWrapper.attachmentsModel.rowCount() > 0
                    }

                    ColumnLayout {
                        id: attachmentsColumn

                        Layout.fillWidth: true
                        visible: incidenceInfo.incidenceWrapper.attachmentsModel.rowCount() > 0

                        Repeater {
                            Layout.fillWidth: true

                            model: incidenceInfo.incidenceWrapper.attachmentsModel

                            delegate: QQC2.Label {
                                Layout.fillWidth: true
                                // This didn't work in Markdown format
                                text: `<a href="${uri}">${attachmentLabel}</a>`
                                onLinkActivated: Qt.openUrlExternally(link)
                                wrapMode: Text.Wrap
                            }
                        }
                    }

                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        text: i18n("<b>Reminders:</b>")
                        visible: incidenceInfo.incidenceWrapper.remindersModel.rowCount() > 0
                    }

                    ColumnLayout {
                        id: remindersColumn

                        Layout.fillWidth: true
                        visible: incidenceInfo.incidenceWrapper.remindersModel.rowCount() > 0

                        Repeater {
                            Layout.fillWidth: true

                            model: incidenceInfo.incidenceWrapper.remindersModel

                            delegate: QQC2.Label {
                                Layout.fillWidth: true
                                text: LabelUtils.secondsToReminderLabel(startOffset)
                                wrapMode: Text.Wrap
                            }
                        }
                    }

                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        text: i18n("<b>Organizer:</b>")
                        visible: incidenceInfo.incidenceWrapper.organizer.fullName
                    }
                    QQC2.Label {
                        Layout.fillWidth: true

                        property var organizer: incidenceInfo.incidenceWrapper.organizer

                        textFormat: Text.MarkdownText
                        text: organizer.name ?
                              `[${organizer.name}](mailto:${organizer.email})` :
                              `[${organizer.email}](mailto:${organizer.email})`
                        onLinkActivated: Qt.openUrlExternally(link)
                        wrapMode: Text.Wrap
                        visible: incidenceInfo.incidenceWrapper.organizer.fullName
                    }

                    QQC2.Label {
                        Layout.alignment: Qt.AlignTop
                        text: i18n("<b>Guests:</b>")
                        visible: incidenceInfo.incidenceWrapper.attendeesModel.rowCount() > 0
                    }
                    ColumnLayout {
                        id: attendeesColumn

                        Layout.fillWidth: true
                        visible: incidenceInfo.incidenceWrapper.attendeesModel.rowCount() > 0

                        Repeater {
                            Layout.fillWidth: true

                            model: incidenceInfo.incidenceWrapper.attendeesModel

                            delegate: QQC2.Label {
                                Layout.fillWidth: true
                                textFormat: Text.MarkdownText
                                text: name ? `[${name}](mailto:${email})` : `[${email}](mailto:${email})`
                                onLinkActivated: Qt.openUrlExternally(link)
                                wrapMode: Text.Wrap
                            }
                        }
                    }
                }
            }
        }
    }
}
