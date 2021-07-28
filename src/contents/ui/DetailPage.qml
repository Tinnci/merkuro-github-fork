/*
 *   SPDX-FileCopyrightText: 2018 Fabian Riethmayer
 *   SPDX-FileCopyrightText: 2021 Nicolas Fella <nicolas.fella@gmx.de>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQuick 2.6
import QtQuick.Controls 2.13
import QtQuick.Layouts 1.2
import org.kde.people 1.0 as KPeople
import org.kde.kirigami 2.10 as Kirigami
import org.kde.phonebook 1.0

Kirigami.ScrollablePage {
    property string personUri;
    id: page

    signal addAttendee(var personData)

    leftPadding: 0
    rightPadding: 0
    topPadding: 0

    title: personData.person.name
    Kirigami.Theme.colorSet: Kirigami.Theme.View

    KPeople.PersonData {
        id: personData
        personUri: page.personUri
    }

    KPeople.PersonActions {
        id: personActions
        personUri: page.personUri
    }

    property var addressee: ContactController.addresseeFromVCard(personData.person.contactCustomProperty("vcard"))

    Column {
        Header {
            id: header
            width: parent.width
            height: Kirigami.Units.gridUnit * 8

            content.anchors.leftMargin: page.width > 400 ? 100 : Kirigami.Units.largeSpacing
            content.anchors.topMargin: Kirigami.Units.largeSpacing
            content.anchors.bottomMargin: Kirigami.Units.largeSpacing

            source: personData.person.photo

            // There might be edge-cases when photo but not pictureUrl is set.
            // For the background image it's more important to provide something else than the default pixmap though.
            backgroundSource: personData.person.pictureUrl ? personData.person.pictureUrl : "" //"qrc:/fallbackBackground.png"

            ColumnLayout {
                Kirigami.Heading {
                    text: personData.person.name
                    //color: "#fcfcfc"
                    level: 2
                }
                Kirigami.Heading {
                    text: personData.person.contactCustomProperty("phoneNumber") ? personData.person.contactCustomProperty("phoneNumber") : ""
                    //color: "#fcfcfc"
                    level: 3
                }
            }
        }

        Instantiator {
            model: personActions

            delegate: Kirigami.Action {
                text: model.display
                iconName: model.iconName
            }

            onObjectAdded: toolbar.actions.push(object)
        }

        ToolBar {
            width: parent.width

            Kirigami.ActionToolBar {
                id: toolbar
                width: parent.width

            }
        }

        Kirigami.FormLayout {
            width: parent.width
            Label {
                visible: text != ""
                text: {
                    // We do not always have the year
                    if (addressee.birthday.getFullYear() === 0) {
                        return Qt.formatDate(addressee.birthday, "dd.MM.")
                    } else {
                        return Qt.formatDate(addressee.birthday)
                    }
                }
                Kirigami.FormData.label: i18n("Birthday:")
            }
        }
    }

    actions {
        main: Kirigami.Action {
            iconName: "list-add"
            text: i18n("Add attendee")
            onTriggered: {
                pageStack.push(Qt.resolvedUrl("AddContactPage.qml"), {state: "update", person: personData.person, addressee: page.addressee})
            }
        }
        left: Kirigami.Action {
            text: i18n("Cancel")
            icon.name: "dialog-cancel"

            onTriggered: {
                pageStack.pop()
            }
        }
        
    }
}
