// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-3.0-or-later

import org.kde.kirigami 2.12 as Kirigami
import org.kde.kalendar 1.0

ContactsPage {
    objectName: "contactView"
    property var mode: KalendarApplication.Contact
    model: ContactManager.filteredContacts
    contactDelegate: ContactListItem {
        height: Kirigami.Settings.isMobile ? Kirigami.Units.gridUnit * 3 : Kirigami.Units.gridUnit * 2
        name: model && model.display
        avatarIcon: model && model.decoration

        onClicked: applicationWindow().pageStack.push('qrc:/ContactPage.qml', {
            contact: model,
            itemId: model.itemId,
        })
    }
}
