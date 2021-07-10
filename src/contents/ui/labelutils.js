// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

function numberToString(number) {
    // The code in here was adapted from an article by Johnathan Wood, see:
    // http://www.blackbeltcoder.com/Articles/strings/converting-numbers-to-ordinal-strings

    let numSuffixes = [ "th",
    "st",
    "nd",
    "rd",
    "th",
    "th",
    "th",
    "th",
    "th",
    "th"];

    let i = (number % 100);
    let j = (i > 10 && i < 20) ? 0 : (number % 10);
    return i18n(number + numSuffixes[j]);
}

function secondsToReminderLabel(seconds) { // Gives prettified time

    function numAndUnit(secs) {
        if(secs >= 2 * 24 * 60 * 60)
            return i18nc("%1 is 2 or more", "%1 days", Math.round(secs / (24*60*60))); // 2 days +
            else if (secs >= 1 * 24 * 60 * 60)
                return "1 day";
            else if (secs >= 2 * 60 * 60)
                return i18nc("%1 is 2 or mores", "%1 hours", Math.round(secs / (60*60))); // 2 hours +
                else if (secs >= 1 * 60 * 60)
                    return i18n("1 hour");
                else
                    return i18n("%1 minutes", Math.round(secs / 60));
    }

    if (seconds < 0) {
        return i18n("%1 before", numAndUnit(seconds * -1));
    } else if (seconds < 0) {
        return i18n("%1 after", numAndUnit(seconds));
    } else {
        return i18n("On event start");
    }
}

function recurrenceToString(recurrenceData) {
    let returnString = i18n("Every");

    switch(recurrenceData.type) {
        case 0:
            return i18n("Never");
        case 3: // Daily
            returnString += i18np(" day", " %1 days", recurrenceData.frequency);
            break;
        case 4: // Weekly
            returnString += i18np(" week", " %1 weeks", recurrenceData.frequency);

            if (recurrenceData.weekdays.filter(x => x === true).length > 0) {
                returnString += i18n(" on");

                for(let i = 0; i < recurrenceData.weekdays.length; i++) {
                    console.log(Qt.locale().dayName(i + Qt.locale().firstDayOfWeek, Locale.ShortFormat))

                    if(recurrenceData.weekdays[i]) {
                        returnString += ` ${Qt.locale().dayName(i + Qt.locale().firstDayOfWeek, Locale.ShortFormat)},`;
                    }
                }
                // Delete last comma
                returnString = returnString.slice(0, -1);
            }
            break;
        case 5: // Monthly on position (e.g. third Monday)
        case 6: // Monthly on day (1st of month)
            returnString += i18np(" month", " %1 months", recurrenceData.frequency);
        case 7: // Yearly on month
        case 8: // Yearly on day
        case 9: // Yearly on position
            returnString += i18np(" year", " %1 years", recurrenceData.frequency);
    }

    return returnString;


}
