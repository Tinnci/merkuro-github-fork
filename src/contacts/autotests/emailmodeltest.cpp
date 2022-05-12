// SPDX-FileCopyrightText: (C) 2020 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: BSD-2-Clause

#include <QObject>
#include <KContacts/Email>
#include <QtTest/QtTest>
#include "../emailmodel.h"

class EmailModelTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
    }

    void testReading()
    {
        KContacts::Email::List emails;
        emails.append(KContacts::Email(QStringLiteral("carl@carlschwan.eu")));
        emails.append(KContacts::Email(QStringLiteral("carl1@carlschwan.eu")));
        KContacts::Email email(QStringLiteral("carl2@carlschwan.eu"));
        email.setPreferred(true);
        email.setType(KContacts::Email::Home);
        emails.append(email);
        EmailModel emailModel;
        emailModel.setEmails(emails);

        QCOMPARE(emailModel.rowCount(), 3);
        QCOMPARE(emailModel.data(emailModel.index(2, 0), Qt::DisplayRole), QStringLiteral("carl@carlschwan.eu"));
    }
};

QTEST_MAIN(EmailModelTest)
#include "emailmodeltest.moc"
