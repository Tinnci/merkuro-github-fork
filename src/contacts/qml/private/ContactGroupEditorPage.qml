// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kalendar.contact 1.0
import org.kde.akonadi 1.0 as Akonadi

Kirigami.ScrollablePage {
    property alias mode: contactGroupEditor.mode
    property var item
    title: mode === ContactGroupEditor.EditMode && contactGroupEditor.name ? i18n('Edit %1', contactGroupEditor.name) : i18n('Create a new contact group')

    onItemChanged: contactGroupEditor.loadContactGroup(item)

    property ContactGroupEditor contactGroupEditor: ContactGroupEditor {
        id: contactGroupEditor
        mode: ContactGroupEditor.CreateMode
        onFinished: root.closeDialog()
        onErrorOccured: {
            errorContainer.errorMessage = error;
            errorContainer.visible = true;
        }
        onItemChangedExternally: itemChangedExternallySheet.open()
    }

    header: QQC2.Control {
        id: errorContainer
        property bool displayError: false
        property string errorMessage: ''
        padding: Kirigami.Units.smallSpacing
        contentItem: Kirigami.InlineMessage {
            type: Kirigami.MessageType.Error
            visible: errorContainer.displayError
            text: errorContainer.errorMessage
            showCloseButton: true
        }
    }

    Kirigami.FormLayout {
        Layout.fillWidth: true

        Akonadi.CollectionComboBox {
            id: addressBookComboBox

            Kirigami.FormData.label: i18n("Address Book:")
            Layout.fillWidth: true
            enabled: mode === ContactGroupEditor.CreateMode

            defaultCollectionId: if (mode === ContactGroupEditor.CreateMode) {
                return ContactConfig.lastUsedAddressBookCollection;
            } else {
                return contactGroupEditor.collectionId;
            }

            mimeTypeFilter: [Akonadi.MimeTypes.address, Akonadi.MimeTypes.contactGroup]
            accessRightsFilter: Akonadi.Collection.CanCreateItem
            onSelectedCollectionChanged: contactGroupEditor.setDefaultAddressBook(collection)
        }

        ColumnLayout {
            Kirigami.FormData.label: i18n("Members:")
            Kirigami.FormData.labelAlignment: repeater.count > 0 ? Qt.AlignTop : Qt.AlignVCenter
            Repeater {
                id: repeater
                model: contactGroupEditor.groupModel
                RowLayout {
                    QQC2.TextField {
                        text: model.display
                        readOnly: true
                    }
                    QQC2.TextField {
                        text: model.email
                        readOnly: true
                    }
                    QQC2.Button {
                        icon.name: 'list-remove'
                    }
                }
            }
        }
        RowLayout {
            QQC2.TextField {
                placeholderText: i18n('Name')
            }
            QQC2.TextField {
                placeholderText: i18n('Email')
            }
            QQC2.Button {
                icon.name: 'list-add'
            }
        }
    }

    footer: QQC2.DialogButtonBox {
        standardButtons: QQC2.DialogButtonBox.Cancel

        QQC2.Button {
            icon.name: mode === ContactGroupEditor.EditMode ? "document-save" : "list-add"
            text: mode === ContactGroupEditor.EditMode ? i18n("Save") : i18n("Add")
            enabled: contactGroupEditor.name.length > 0
            QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.AcceptRole
        }

        onRejected: {
            ContactConfig.lastUsedAddressBookCollection = addressBookComboBox.defaultCollectionId;
            ContactConfig.save();
            root.closeDialog();
        }
        onAccepted: {
            contactEditor.saveContactInAddressBook()
            ContactConfig.lastUsedAddressBookCollection = addressBookComboBox.defaultCollectionId;
            ContactConfig.save();
        }
    }

    QQC2.Dialog {
        id: itemChangedExternallySheet
        visible: false
        title: i18n('The contact group has been changed by someone else')
        modal: true
        focus: true
        x: (parent.width - width) / 2
        y: parent.height / 3
        width: Math.min(parent.width - Kirigami.Units.gridUnit * 4, Kirigami.Units.gridUnit * 20)

        contentItem: QQC2.Label {
            text: i18n('What should be done?')
        }
        onRejected: itemChangedExternallySheet.close()
        onAccepted: {
            contactGroupEditor.fetchItem();
            itemChangedExternallySheet.close();
        }

        footer: QQC2.DialogButtonBox {
            QQC2.Button {
                text: i18n("Take over changes")
                QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.AcceptRole
            }

            QQC2.Button {
                text: i18n("Ignore and Overwrite changes")
                QQC2.DialogButtonBox.buttonRole: QQC2.DialogButtonBox.RejectRole
            }
        }
    }
}