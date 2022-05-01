// SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <Akonadi/Collection>
#include <Akonadi/EntityTreeModel>
#include <KContacts/Addressee>
#include <KContacts/ContactGroup>
#include <KCheckableProxyModel>
#include "collectionmodel.h"

static bool isStructuralCollection(const Akonadi::Collection &collection)
{
    const QStringList mimeTypes = {KContacts::Addressee::mimeType(), KContacts::ContactGroup::mimeType()};
    const QStringList collectionMimeTypes = collection.contentMimeTypes();
    for (const QString &mimeType : mimeTypes) {
        if (collectionMimeTypes.contains(mimeType)) {
            return false;
        }
    }
    return true;
}

StructuralCollectionsNotCheckableProxy::StructuralCollectionsNotCheckableProxy(QObject *parent)
    : KCheckableProxyModel(parent)
{
}

QVariant StructuralCollectionsNotCheckableProxy::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (role == Qt::CheckStateRole) {
        // Don't show the checkbox if the collection can't contain incidences
        const auto collection = index.data(Akonadi::EntityTreeModel::CollectionRole).value<Akonadi::Collection>();
        if (collection.isValid() && isStructuralCollection(collection)) {
            return {};
        }
    }
    return KCheckableProxyModel::data(index, role);
}
