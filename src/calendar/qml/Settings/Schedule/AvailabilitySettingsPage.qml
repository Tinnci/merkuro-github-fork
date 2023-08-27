// SPDX-FileCopyrightText: 2023 Carl Schwan <carlschwan@kde.org>
// SPDX-License-Identifier: LGPL-3.0-or-later

import QtQuick 2.15
import org.kde.kirigami 2.19 as Kirigami
import QtQuick.Layouts 1.15
import org.kde.akonadi 1.0 as Akonadi
import org.kde.merkuro.calendar 1.0 as Calendar
import org.kde.kirigamiaddons.labs.mobileform 0.1 as MobileForm
import QtQuick.Controls 2.15 as QQC2

Kirigami.ScrollablePage {
    id: availabilitySettingsPage
    title: i18n("Configure Availability")
    leftPadding: 0
    rightPadding: 0

    property var availabilityWrapper 
    property bool mondayChecked : false;

    ColumnLayout {
         MobileForm.FormCard {
            id: availabilityPublishSection
            Layout.fillWidth: true
            Layout.topMargin: Kirigami.Units.largeSpacing
            contentItem: ColumnLayout {
                spacing: 0
                MobileForm.FormCardHeader {
                    id: availbilityPublishHeader
                    title: i18n("Availability")
                }

                MobileForm.FormDelegateSeparator {}

                Kirigami.FormLayout{

                    // TODO put these inside MobileForm components
                    Akonadi.CollectionComboBox {
                        id: calendarCombo

                        Kirigami.FormData.label: i18n("Calendar:")
                        Layout.fillWidth: true

                        defaultCollectionId: {
                            if (root.availabilityWrapper.collectionId === -1) {
                                return Config.lastUsedEventCollection;
                            }
                            return root.availabilityWrapper.collectionId;
                        }

                        // TODO not sure about below?
                        mimeTypeFilter: [Akonadi.MimeTypes.calendar]
                        accessRightsFilter: Akonadi.Collection.CanCreateItem
                        onUserSelectedCollection: root.availabilityWrapper.setCollection(collection)
                    }

                    QQC2.ComboBox {
                        id: timeZoneComboBox
                        Kirigami.FormData.label: i18n("Timezone:")
                        Layout.fillWidth: true

                        model: Calendar.TimeZoneListModel {
                            id: timeZonesModel
                        }

                        textRole: "display"
                        valueRole: "id"
                        currentIndex: model ? timeZonesModel.getTimeZoneRow(root.availabilityWrapper.timeZone) : -1
                        delegate: Kirigami.BasicListItem {
                            label: model.display
                            onClicked: root.availabilityWrapper.timeZone = model.id
                        }

                        enabled: !incidenceForm.isTodo || (incidenceForm.isTodo && incidenceEndCheckBox.checked)
                    }

                    MobileForm.FormDelegateSeparator {}

                    // TODO make below in a list?
                    RowLayout {

                        id: rowLayoutMonday
                        spacing: Kirigami.Units.gridUnit

                        ColoredCheckbox {
                            id: mondayCheckBox

                            // visible: model.checkState != null
                            // color: collectionSourceItem.collectionColor ?? Kirigami.Theme.highlightedTextColor
                            // checked: model.checkState === 2
                            // onCheckedChanged: root.collectionCheckChanged()
                            onClicked: {
                                print("Monday clicked");
                                root.mondayChecked = true;
                                print(root.mondayChecked);
                                // model.checkState = model.checkState === 0 ? 2 : 0
                                // root.collectionCheckChanged()
                            }

                            Layout.alignment: Qt.AlignLeft
                        }

                        Kirigami.Heading {
                            Layout.fillWidth: rowLayoutMonday.children.length === 1
                            Layout.alignment: Qt.AlignVCenter

                            opacity: 0.7
                            level: 5
                            type: Kirigami.Heading.Primary
                            text: "Monday"
                            elide: Text.ElideRight

                            font.weight: Font.Bold
                            color: isOverdue ? Kirigami.Theme.negativeTextColor : isToday ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                        }

                        TimeCombo {
                            id: incidenceStartTimeCombo

                            Layout.fillWidth: true
                            // timeZoneOffset: root.availabilityWrapper.startTimeZoneUTCOffsetMins
                            // display: root.availabilityWrapper.incidenceEndTimeDisplay
                            // dateTime: root.availabilityWrapper.incidenceStart
                            onNewTimeChosen: root.availabilityWrapper.setMondayStartTime(hours, minutes)
                            enabled: false
                            // enabled: !allDayCheckBox.checked && (!incidenceForm.isTodo || incidenceStartCheckBox.checked)
                            // visible: !allDayCheckBox.checked
                        }

                        Kirigami.Heading {
                            Layout.fillWidth: rowLayoutMonday.children.length === 1
                            Layout.alignment: Qt.AlignVCenter

                            opacity: 0.7
                            level: 5
                            type: Kirigami.Heading.Primary
                            text: "to"
                            elide: Text.ElideRight

                            font.weight: Font.Bold
                            color: isOverdue ? Kirigami.Theme.negativeTextColor : isToday ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                        }

                        TimeCombo {
                            id: incidenceEndTimeCombo

                            Layout.fillWidth: true
                            // timeZoneOffset: root.availabilityWrapper.startTimeZoneUTCOffsetMins
                            // display: root.availabilityWrapper.incidenceEndTimeDisplay
                            // dateTime: root.availabilityWrapper.incidenceStart
                            // onNewTimeChosen: root.availabilityWrapper.setIncidenceStartTime(hours, minutes)
                            // enabled: !allDayCheckBox.checked && (!incidenceForm.isTodo || incidenceStartCheckBox.checked)
                            // visible: !allDayCheckBox.checked
                        }
                    }

                    RowLayout {

                        id: rowLayoutTuesday
                        spacing: Kirigami.Units.gridUnit

                        ColoredCheckbox {
                            id: tuesdayCheckBox

                            // visible: model.checkState != null
                            // color: collectionSourceItem.collectionColor ?? Kirigami.Theme.highlightedTextColor
                            // checked: model.checkState === 2
                            // onCheckedChanged: root.collectionCheckChanged()
                            onClicked: {
                                print("Tuesday clicked");
                                root.tuesdayChecked = true;
                                print(root.tuesdayChecked);
                                // model.checkState = model.checkState === 0 ? 2 : 0
                                // root.collectionCheckChanged()
                            }

                            Layout.alignment: Qt.AlignLeft
                        }

                        Kirigami.Heading {
                            Layout.fillWidth: rowLayoutMonday.children.length === 1
                            Layout.alignment: Qt.AlignVCenter

                            opacity: 0.7
                            level: 5
                            type: Kirigami.Heading.Primary
                            text: "Tuesday"
                            elide: Text.ElideRight

                            font.weight: Font.Bold
                            color: isOverdue ? Kirigami.Theme.negativeTextColor : isToday ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                        }

                        TimeCombo {
                            id: incidenceTuesdayStartTimeCombo

                            Layout.fillWidth: true
                            // timeZoneOffset: root.availabilityWrapper.startTimeZoneUTCOffsetMins
                            // display: root.availabilityWrapper.incidenceEndTimeDisplay
                            // dateTime: root.availabilityWrapper.incidenceStart
                            // onNewTimeChosen: root.availabilityWrapper.setIncidenceStartTime(hours, minutes)
                            enabled: false
                            // enabled: !allDayCheckBox.checked && (!incidenceForm.isTodo || incidenceStartCheckBox.checked)
                            // visible: !allDayCheckBox.checked
                        }

                        Kirigami.Heading {
                            Layout.fillWidth: rowLayoutMonday.children.length === 1
                            Layout.alignment: Qt.AlignVCenter

                            opacity: 0.7
                            level: 5
                            type: Kirigami.Heading.Primary
                            text: "to"
                            elide: Text.ElideRight

                            font.weight: Font.Bold
                            color: isOverdue ? Kirigami.Theme.negativeTextColor : isToday ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                        }

                        TimeCombo {
                            id: incidenceTuesdayEndTimeCombo

                            Layout.fillWidth: true
                            // timeZoneOffset: root.availabilityWrapper.startTimeZoneUTCOffsetMins
                            // display: root.availabilityWrapper.incidenceEndTimeDisplay
                            // dateTime: root.availabilityWrapper.incidenceStart
                            // onNewTimeChosen: root.availabilityWrapper.setIncidenceStartTime(hours, minutes)
                            // enabled: !allDayCheckBox.checked && (!incidenceForm.isTodo || incidenceStartCheckBox.checked)
                            // visible: !allDayCheckBox.checked
                        }
                    }


                }


                MobileForm.FormDelegateSeparator {}
                MobileForm.AbstractFormDelegate {
                    Layout.fillWidth: true
                    contentItem: RowLayout {
                        QQC2.Button {
                            text: i18n("Update")
                            onClicked: {
                                let availabilityWrapper = Calendar.CalendarManager.createAvailabilityWrapper();
                                print(availabilityWrapper)
                                print("setting summary")
                                print("after setting summary")
                                Calendar.CalendarManager.addAvailability(availabilityWrapper);
                            }
                        }
                    }
                }
            }
         }
    }
}
