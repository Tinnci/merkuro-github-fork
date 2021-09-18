// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami
import org.kde.kalendar 1.0

Kirigami.ScrollablePage {
    id: root

    title: i18n("Manage Tags")

    ListView {
        currentIndex: -1
        model: TagManager.tagModel
        delegate: Kirigami.BasicListItem {
            contentItem: Item {
                implicitWidth: delegateLayout.implicitWidth
                implicitHeight: delegateLayout.implicitHeight
                RowLayout {
                    id: delegateLayout

                    property bool editMode: false

                    anchors {
                        left: parent.left
                        top: parent.top
                        right: parent.right
                    }

                    QQC2.Label {
                        Layout.fillWidth: true
                        text: model.display
                        visible: !delegateLayout.editMode
                    }
                    QQC2.ToolButton {
                        icon.name: "edit-rename"
                        onClicked: delegateLayout.editMode = true
                        visible: !delegateLayout.editMode
                    }
                    QQC2.ToolButton {
                        icon.name: "delete"
                        visible: !delegateLayout.editMode
                    }

                    QQC2.TextField {
                        id: tagNameField
                        Layout.fillWidth: true
                        text: model.display
                        visible: delegateLayout.editMode
                    }
                    QQC2.ToolButton {
                        icon.name: "gtk-apply"
                        visible: delegateLayout.editMode
                        onClicked: {
                            TagManager.renameTag(model.tag, tagNameField.text)
                            delegateLayout.editMode = false;
                        }
                    }
                    QQC2.ToolButton {
                        icon.name: "gtk-cancel"
                        onClicked: {
                            delegateLayout.editMode = false;
                            tagNameField.text = model.display;
                        }
                        visible: delegateLayout.editMode
                    }
                }
            }
        }
    }


    footer: ColumnLayout {

        RowLayout {
            QQC2.TextField {
                Layout.fillWidth: true
                placeholderText: i18n("Create a New Tag...")
            }
            QQC2.Button {
                Layout.column: 4
                Layout.row: 0
                text: i18n("Create Tag")
            }
        }
    }
}
