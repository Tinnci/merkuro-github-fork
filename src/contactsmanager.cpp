// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <AkonadiCore/Item>
#include <AkonadiCore/ItemFetchJob>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/Monitor>
#include <Akonadi/Contact/EmailAddressSelectionModel>
#include <KContacts/Addressee>
#include <KContacts/ContactGroup>
#include "contactsmanager.h"

class ContactsModel : public QSortFilterProxyModel
{
public:
    explicit ContactsModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent)
    {
        auto sourceModel = new Akonadi::EmailAddressSelectionModel;
        auto filterModel = new Akonadi::ContactsFilterProxyModel;
        auto flatModel = new KDescendantsProxyModel;
        auto addresseeOnlyModel = new Akonadi::EntityMimeTypeFilterModel;

        filterModel->setSourceModel(sourceModel->model());
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
        // Eliminate duplicate Akonadi items
        const QModelIndex sourceIndex = sourceModel()->index(row, 0, sourceParent);
        Q_ASSERT(sourceIndex.isValid());

        auto data = sourceIndex.data(Akonadi::EntityTreeModel::ItemIdRole);
        auto matches = match(index(0,0), Akonadi::EntityTreeModel::ItemIdRole, data, 2, Qt::MatchExactly | Qt::MatchWrap | Qt::MatchRecursive);

        if(matches.length() >= 1) {
            qDebug() << matches[0].data();
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
