// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "contactgroupwrapper.h"

#include <Akonadi/ItemFetchJob>
#include <Akonadi/ItemFetchScope>
#include <Akonadi/Session>
#include <Akonadi/Monitor>
#include <QAbstractListModel>
#include "contactgroupmodel.h"

using namespace Akonadi;

ContactGroupWrapper::ContactGroupWrapper(QObject *parent)
    : QObject(parent)
    , m_model(new ContactGroupModel(this))
{
    KContacts::ContactGroup dummyGroup;
    m_model->loadContactGroup(dummyGroup);
}

QString ContactGroupWrapper::name() const
{
    return m_name;
}

QAbstractListModel *ContactGroupWrapper::model() const
{
    return (QAbstractListModel *)m_model;
}

Akonadi::Item ContactGroupWrapper::item() const
{
    return m_item;
}

void ContactGroupWrapper::setItem(const Akonadi::Item &item)
{
    m_item = item;
    auto job = new ItemFetchJob(item);
    job->fetchScope().fetchFullPayload();
    job->fetchScope().setAncestorRetrieval(Akonadi::ItemFetchScope::Parent);

    connect(job, &Akonadi::ItemFetchJob::result, this, [this](KJob *job) {
        itemFetchDone(job);
    });

    setupMonitor();
    m_monitor->setItemMonitored(item);
}

void ContactGroupWrapper::setupMonitor()
{
    delete m_monitor;
    m_monitor = new Monitor;
    m_monitor->setObjectName(QStringLiteral("ContactGroupWrapprMonitor"));
    m_monitor->ignoreSession(Session::defaultSession());

    QObject::connect(m_monitor, &Monitor::itemChanged, this, [this](const Akonadi::Item &, const QSet<QByteArray> &) {
        const auto group = m_item.payload<KContacts::ContactGroup>();
        loadContactGroup(group);
    });
}

void ContactGroupWrapper::itemFetchDone(KJob *job)
{
    if (job->error()) {
        return;
    }

    auto fetchJob = qobject_cast<ItemFetchJob *>(job);
    if (!fetchJob) {
        return;
    }

    if (fetchJob->items().isEmpty()) {
        return;
    }

    m_item = fetchJob->items().at(0);

    const auto group = m_item.payload<KContacts::ContactGroup>();
    loadContactGroup(group);
    Q_EMIT itemChanged();
}

void ContactGroupWrapper::loadContactGroup(const KContacts::ContactGroup &group)
{
    m_name = group.name();
    m_model->loadContactGroup(group);
}
