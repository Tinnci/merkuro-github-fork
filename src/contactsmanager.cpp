// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <AkonadiCore/Session>
#include <AkonadiCore/ItemFetchScope>
#include <AkonadiCore/EntityDisplayAttribute>
#include <AkonadiCore/Monitor>
#include <KContacts/Addressee>
#include <KContacts/ContactGroup>
#include "contactsmanager.h"

ContactsManager::ContactsManager(QObject* parent)
    : QObject(parent)
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

    m_sourceModel = new Akonadi::ContactsTreeModel(monitor);
    m_filterModel = new Akonadi::ContactsFilterProxyModel;
    m_model = new KDescendantsProxyModel;

    m_filterModel->setSourceModel(m_sourceModel);
    m_filterModel->setFilterFlags(Akonadi::ContactsFilterProxyModel::HasEmail);
    m_model->setSourceModel(m_filterModel);
    m_model->sort(0);
}

KDescendantsProxyModel * ContactsManager::contactsModel()
{
    return m_model;
}
