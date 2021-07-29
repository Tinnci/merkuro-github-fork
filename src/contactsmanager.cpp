// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <AkonadiCore/Session>
#include <AkonadiCore/Item>
#include <AkonadiCore/ItemFetchJob>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/EntityDisplayAttribute>
#include <AkonadiCore/Monitor>
#include <KContacts/Addressee>
#include <KContacts/ContactGroup>
#include "contactsmanager.h"

class ContactsModel : public QSortFilterProxyModel
{
public:
    explicit ContactsModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
        Akonadi::Session *session = new Akonadi::Session( "MySession" );
        Akonadi::ItemFetchScope scope;
        // fetch all content of the contacts, including images
        scope.fetchFullPayload( true );
        // fetch the EntityDisplayAttribute, which contains custom names and icons
        scope.fetchAttribute<Akonadi::EntityDisplayAttribute>();
        Akonadi::Monitor *monitor = new Akonadi::Monitor;
        monitor->setSession( session );
        // include fetching the collection tree
        monitor->fetchCollection( true );
        // set the fetch scope that shall be used
        monitor->setItemFetchScope( scope );
        // monitor all collections below the root collection for changes
        monitor->setCollectionMonitored( Akonadi::Collection::root() );
        // list only contacts and contact groups
        monitor->setMimeTypeMonitored( KContacts::Addressee::mimeType(), true );
        monitor->setMimeTypeMonitored( KContacts::ContactGroup::mimeType(), true );

        auto sourceModel = new Akonadi::ContactsTreeModel(monitor);
        auto filterModel = new Akonadi::ContactsFilterProxyModel;
        auto flatModel = new KDescendantsProxyModel;
        auto addresseeOnlyModel = new Akonadi::EntityMimeTypeFilterModel;

        filterModel->setSourceModel(sourceModel);
        filterModel->setFilterFlags(Akonadi::ContactsFilterProxyModel::HasEmail);
        flatModel->setSourceModel(filterModel);

        addresseeOnlyModel->setSourceModel(flatModel);
        addresseeOnlyModel->addMimeTypeInclusionFilter(KContacts::Addressee::mimeType());

        setSourceModel(addresseeOnlyModel);
        setDynamicSortFilter(true);
        sort(0);
    }

protected:
    bool filterAcceptsRow(int row, const QModelIndex &sourceParent) const override
    {
        const QModelIndex sourceIndex = sourceModel()->index(row, 0, sourceParent);
        Q_ASSERT(sourceIndex.isValid());

        auto data = index(row, 0).data(Akonadi::EntityTreeModel::ItemIdRole);
        auto matches = match(index(0,0), Akonadi::EntityTreeModel::ItemIdRole, data, 2, Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive);

        if(matches.length() >= 1) {
            return false;
        }

        return true;
    }
};

ContactsManager::ContactsManager(QObject* parent)
    : QObject(parent)
{
    auto model = new ContactsModel(this);
    m_model = new QSortFilterProxyModel;
    m_model->setSourceModel(model);
    m_model->setDynamicSortFilter(true);
    m_model->setSortCaseSensitivity(Qt::CaseInsensitive);
    m_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_model->sort(0);
}

QSortFilterProxyModel * ContactsManager::contactsModel()
{
    return m_model;
}

void ContactsManager::contactEmails(qint64 itemId)
{
    Akonadi::Item item(itemId);

    Akonadi::ItemFetchJob *job = new Akonadi::ItemFetchJob(item);
    job->fetchScope().fetchFullPayload();

    connect(job, &Akonadi::ItemFetchJob::result, this, [this, itemId] (KJob *job) {

        Akonadi::ItemFetchJob *fetchJob = qobject_cast<Akonadi::ItemFetchJob*>(job);
        auto item = fetchJob->items().at(0);
        auto payload = item.payload<KContacts::Addressee>();

        Q_EMIT emailsFetched(payload.emails(), itemId);
    });
}
