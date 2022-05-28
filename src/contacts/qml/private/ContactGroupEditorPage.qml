// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15

import org.kde.kirigami 2.19 as Kirigami
import org.kde.kalendar.contact 1.0
import org.kde.akonadi 1.0 as Akonadi

Kirigami.ScrollablePage {
    property alias mode: contactEditor.mode
    property alias item: contactEditor.item

    property ContactGroupEditor contactEditor: ContactGroupEditor {
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

    contentItem: ListView {

        model: contactGroupEditor.groupModel
        delegate: Kirigami.BasicListDelegate {
            label: model.display
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
                contactEditor.fetchItem();
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
}