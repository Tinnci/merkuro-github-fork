// SPDX-FileCopyrightText: 2022 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "addresseewrapper.h"
#include "kalendar_debug.h"
#include <KLocalizedString>
#include <QBitArray>
#include <QJSValue>
#include <akonadi/itemmonitor.h>
#include <kcontacts/addressee.h>

AddresseeWrapper::AddresseeWrapper(QObject *parent)
    : QObject(parent)
    , Akonadi::ItemMonitor()
{
    Akonadi::ItemFetchScope scope;
    scope.fetchFullPayload();
    scope.fetchAllAttributes();
    scope.setFetchRelations(true);
    scope.setAncestorRetrieval(Akonadi::ItemFetchScope::Parent);
    setFetchScope(scope);
}

AddresseeWrapper::~AddresseeWrapper() = default;

void AddresseeWrapper::notifyDataChanged()
{
    Q_EMIT collectionIdChanged();
    Q_EMIT nameChanged();
    Q_EMIT birthdayChanged();
    Q_EMIT photoChanged();
    Q_EMIT phoneNumbersChanged();
    Q_EMIT preferredEmailChanged();
}

Akonadi::Item AddresseeWrapper::addresseeItem() const
{
    return item();
}

qint64 AddresseeWrapper::itemId() const
{
    return item().id();
}

void AddresseeWrapper::setItemId(qint64 itemId)
{
    Akonadi::Item item(itemId);

    auto job = new Akonadi::ItemFetchJob(item);
    job->fetchScope().fetchFullPayload();

    connect(job, &Akonadi::ItemFetchJob::result, this, [this, itemId](KJob *job) {
        auto fetchJob = qobject_cast<Akonadi::ItemFetchJob *>(job);
        setAddresseeItem(fetchJob->items().at(0));
    });
}

void AddresseeWrapper::setAddresseeItem(const Akonadi::Item &addresseeItem)
{
    if (addresseeItem.hasPayload<KContacts::Addressee>()) {
        setItem(addresseeItem);
        setAddressee(addresseeItem.payload<KContacts::Addressee>());
        Q_EMIT addresseeItemChanged();
        Q_EMIT collectionIdChanged();
    } else {
        qCWarning(KALENDAR_LOG) << "This is not an incidence item.";
    }
}

void AddresseeWrapper::itemChanged(const Akonadi::Item &item)
{
    setAddressee(item.payload<KContacts::Addressee>());
}

void AddresseeWrapper::setAddressee(const KContacts::Addressee &addressee)
{
    m_addressee = addressee;
    notifyDataChanged();
}

QString AddresseeWrapper::uid() const
{
    return m_addressee.uid();
}

qint64 AddresseeWrapper::collectionId() const
{
    return m_collectionId < 0 ? item().parentCollection().id() : m_collectionId;
}

void AddresseeWrapper::setCollectionId(qint64 collectionId)
{
    m_collectionId = collectionId;
    Q_EMIT collectionIdChanged();
}

QString AddresseeWrapper::name() const
{
    qDebug() << m_addressee.formattedName();
    return m_addressee.formattedName();
}

void AddresseeWrapper::setName(const QString &name)
{
    if (name == m_addressee.formattedName()) {
        return;
    }
    m_addressee.setFormattedName(name);
    Q_EMIT nameChanged();
}

QDateTime AddresseeWrapper::birthday() const
{
    return m_addressee.birthday();
}

void AddresseeWrapper::setBirthday(const QDateTime &birthday)
{
    if (birthday == m_addressee.birthday()) {
        return;
    }
    m_addressee.setBirthday(birthday);
    Q_EMIT birthdayChanged();
}

KContacts::PhoneNumber::List AddresseeWrapper::phoneNumbers() const
{
    return m_addressee.phoneNumbers();
}

KContacts::Picture AddresseeWrapper::photo() const
{
    return m_addressee.photo();
}

QString AddresseeWrapper::preferredEmail() const
{
    return m_addressee.preferredEmail();
}
