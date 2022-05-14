#!/usr/bin/env bash

$XGETTEXT `find . -name '*.js' -o -name '*.qml'` -o $podir/plasma_applet_org.kde.kalendar.contact.pot
