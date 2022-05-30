/*
     This file is part of Akonadi Contact.

     SPDX-FileCopyrightText: 2007-2009 Tobias Koenig <tokoe@kde.org>

     SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "collectioncomboboxmodel.h"

#include <Akonadi/CollectionFetchScope>
#include <Akonadi/CollectionFilterProxyModel>
#include <Akonadi/CollectionUtils>
#include <Akonadi/EntityRightsFilterModel>
#include <Akonadi/EntityTreeModel>
#include <Akonadi/Monitor>
#include <Akonadi/Session>

#include <KDescendantsProxyModel>

#include <QAbstractItemModel>
#include "colorproxymodel.h"

using namespace Akonadi::Quick;

class Akonadi::Quick::CollectionComboBoxModelPrivate
{
public:
    CollectionComboBoxModelPrivate(CollectionComboBoxModel *parent)
        : mParent(parent)
    {
        mMonitor = new Akonadi::Monitor(mParent);
        mMonitor->setObjectName(QStringLiteral("CollectionComboBoxMonitor"));
        mMonitor->fetchCollection(true);
        mMonitor->setCollectionMonitored(Akonadi::Collection::root());

        // This ETM will be set to only show collections with the wanted mimetype in setMimeTypeFilter
        auto entityModel = new Akonadi::EntityTreeModel(mMonitor, mParent);
        entityModel->setItemPopulationStrategy(Akonadi::EntityTreeModel::NoItemPopulation);
        entityModel->setListFilter(Akonadi::CollectionFetchScope::Display);

        // Display color
        auto colorProxy = new ColorProxyModel(mParent);
        colorProxy->setObjectName(QStringLiteral("Show collection colors"));
        colorProxy->setDynamicSortFilter(true);
        colorProxy->setSourceModel(entityModel);

        // Flatten the tree, e.g.
        // Kolab
        // Kolab / Inbox
        // Kolab / Inbox / Calendar
        auto proxyModel = new KDescendantsProxyModel(parent);
        proxyModel->setDisplayAncestorData(true);
        proxyModel->setSourceModel(colorProxy);

        // Filter it by mimetype again, to only keep
        // Kolab / Inbox / Calendar
        mMimeTypeFilterModel = new Akonadi::CollectionFilterProxyModel(parent);
        mMimeTypeFilterModel->setSourceModel(proxyModel);

        // Filter by access rights. TODO: maybe this functionality could be provided by CollectionFilterProxyModel, to save one proxy?
        mRightsFilterModel = new Akonadi::EntityRightsFilterModel(parent);
        mRightsFilterModel->setSourceModel(mMimeTypeFilterModel);

        mParent->setSourceModel(mRightsFilterModel);
        // mRightsFilterModel->sort(mParent->modelColumn());

        mParent->connect(mRightsFilterModel, &QAbstractItemModel::rowsInserted, mParent, [this](const QModelIndex &parent, int start, int end) {
            for (int i = start; i <= end; ++i) {
                scanSubTree(mRightsFilterModel->index(i, 0, parent));
            }
        });
    }

    ~CollectionComboBoxModelPrivate() = default;

    bool scanSubTree(const QModelIndex &index);

    CollectionComboBoxModel *const mParent;

    Akonadi::Monitor *mMonitor = nullptr;
    Akonadi::CollectionFilterProxyModel *mMimeTypeFilterModel = nullptr;
    Akonadi::EntityRightsFilterModel *mRightsFilterModel = nullptr;
    Akonadi::Collection mDefaultCollection;
    qint64 mDefaultCollectionId = -1;
    int mCurrentIndex = -1;
};

bool CollectionComboBoxModelPrivate::scanSubTree(const QModelIndex &index)
{
    if (!index.isValid()) {
        return false;
    }

    const Akonadi::Collection::Id id = index.data(EntityTreeModel::CollectionIdRole).toLongLong();

    if (mDefaultCollectionId == id) {
        mParent->setCurrentIndex(index.row());
        return true;
    }

    for (int row = 0; row < mRightsFilterModel->rowCount(index); ++row) {
        const QModelIndex childIndex = mRightsFilterModel->index(row, 0, index);
        // This should not normally happen, but if it does we end up in an endless loop
        if (!childIndex.isValid()) {
            qWarning() << "Invalid child detected: " << index.data().toString();
            Q_ASSERT(false);
            return false;
        }
        if (scanSubTree(childIndex)) {
            qDebug() << "end 1";
            return true;
        }
    }

    return false;
}

CollectionComboBoxModel::CollectionComboBoxModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , d(new CollectionComboBoxModelPrivate(this))
{
    connect(this, &CollectionComboBoxModel::currentIndexChanged, this, [this]() {
        const QModelIndex modelIndex = index(d->mCurrentIndex, 0);
        if (modelIndex.isValid()) {
            Q_EMIT currentCollectionChanged();
        }
    });
}

CollectionComboBoxModel::~CollectionComboBoxModel() = default;

void CollectionComboBoxModel::setMimeTypeFilter(const QStringList &contentMimeTypes)
{
    d->mMimeTypeFilterModel->clearFilters();
    d->mMimeTypeFilterModel->addMimeTypeFilters(contentMimeTypes);

    if (d->mMonitor) {
        for (const QString &mimeType : contentMimeTypes) {
            d->mMonitor->setMimeTypeMonitored(mimeType, true);
        }
    }
}

QStringList CollectionComboBoxModel::mimeTypeFilter() const
{
    return d->mMimeTypeFilterModel->mimeTypeFilters();
}

void CollectionComboBoxModel::setAccessRightsFilter(int rights)
{
    d->mRightsFilterModel->setAccessRights((Collection::Right)rights);
    Q_EMIT accessRightsFilterChanged();
}

int CollectionComboBoxModel::accessRightsFilter() const
{
    return (int)d->mRightsFilterModel->accessRights();
}

qint64 CollectionComboBoxModel::defaultCollectionId() const
{
    auto collection = currentCollection();
    if (collection.isValid()) {
        return collection.id();
    } else {
        return d->mDefaultCollectionId;
    }
}

void CollectionComboBoxModel::setDefaultCollectionId(qint64 collectionId)
{
    if (d->mDefaultCollectionId == collectionId || collectionId < -1) {
        return;
    }
    d->mDefaultCollectionId = collectionId;
    d->scanSubTree({});
}

void CollectionComboBoxModel::setDefaultCollection(const Collection &collection)
{
    d->mDefaultCollection = collection;
    d->mDefaultCollectionId = collection.id();
    d->scanSubTree({});
}

Akonadi::Collection CollectionComboBoxModel::currentCollection() const
{
    const QModelIndex modelIndex = index(currentIndex(), 0);
    if (modelIndex.isValid()) {
        return modelIndex.data(Akonadi::EntityTreeModel::CollectionRole).value<Collection>();
    } else {
        return Akonadi::Collection();
    }
}

void CollectionComboBoxModel::setExcludeVirtualCollections(bool b)
{
    d->mMimeTypeFilterModel->setExcludeVirtualCollections(b);
}

bool CollectionComboBoxModel::excludeVirtualCollections() const
{
    return d->mMimeTypeFilterModel->excludeVirtualCollections();
}

int CollectionComboBoxModel::currentIndex() const
{
    return d->mCurrentIndex;
}

void CollectionComboBoxModel::setCurrentIndex(int currentIndex)
{
    if (d->mCurrentIndex == currentIndex) {
        return;
    }
    d->mCurrentIndex = currentIndex;
    Q_EMIT currentIndexChanged();
}

#include "moc_collectioncomboboxmodel.cpp"