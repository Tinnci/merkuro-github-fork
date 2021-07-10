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
