// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.15 as Kirigami
import "dateutils.js" as DateUtils

QQC2.ComboBox {
    id: root

    signal newTimeChosen(date newTime)

    property int timeZoneOffset: 0
    property date dateTime
    property RegularExpressionValidator timeValidator: RegularExpressionValidator {
        regularExpression: /^([0-1]?[0-9]|2[0-3]):([0-5][0-9])(:[0-5][0-9])?$/
    }
    property RegularExpressionValidator inputValidator: RegularExpressionValidator {
        regularExpression: /[0-9]{0,2}[:][0-9]{0,2}/
    }
    property alias timePicker: popupTimePicker

    editable: true
    editText: activeFocus ? editText : DateUtils.adjustDateTimeToLocalTimeZone(dateTime, timeZoneOffset).toLocaleTimeString(Qt.locale(), "HH:mm")

    inputMethodHints: Qt.ImhTime
    validator: activeFocus ? inputValidator : timeValidator

    onEditTextChanged: {
        if(!isNaN(dateTime.getTime())) {
            popupTimePicker.setToTimeFromString(editText)
        }
        if (acceptableInput && activeFocus) { // Need to check for activeFocus or on load the text gets reset to 00:00
            newTimeChosen(new Date(DateUtils.adjustDateTimeToLocalTimeZone(dateTime, timeZoneOffset).setHours(popupTimePicker.hours, popupTimePicker.minutes)));
        }
    }

    popup: QQC2.Popup {
        id: timePopup
        width: parent.width
        height: parent.width * 2
        y: parent.y + parent.height
        z: 1000

        TimePicker {
            id: popupTimePicker

            Component.onCompleted: minuteMultiples = 5
            Connections {
                target: root
                function onDateTimeChanged() {
                    popupTimePicker.setToTimeFromString(root.editText);
                }
            }

            onDateTimeChanged: if(visible) {
                let newDt = new Date(DateUtils.adjustDateTimeToLocalTimeZone(root.dateTime, root.timeZoneOffset).setHours(dateTime.getHours(), dateTime.getMinutes()));
                root.newTimeChosen(newDt);
            }
            onDone: timePopup.close();
        }
    }
}
