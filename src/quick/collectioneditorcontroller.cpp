// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "collectioneditorcontroller.h"
#include <Akonadi/CollectionFetchJob>
#include <Akonadi/CollectionModifyJob>
#include <Akonadi/EntityDisplayAttribute>

CollectionEditorController::CollectionEditorController(QObject *parent)
    : QObject(parent)
{
}

Akonadi::Collection::Id CollectionEditorController::collectionId() const
{
    return mId;
}

void CollectionEditorController::setCollectionId(const Akonadi::Collection::Id &collectionId)
{
    if (mId == collectionId) {
        return;
    }
    mId = collectionId;
    mCollection = Akonadi::Collection{};
    Q_EMIT collectionIdChanged();
    setDisplayName({});

    auto job = new Akonadi::CollectionFetchJob(QList<Akonadi::Collection::Id>{collectionId});
    connect(job, &Akonadi::CollectionFetchJob::finished, this, [this, job](KJob *) {
        if (job->error()) {
            qWarning() << job->errorString();
            return;
        }
        const auto collections = job->collections();
        if (collections.isEmpty()) {
            return;
        }
        mCollection = collections.at(0);
        setDisplayName(mCollection.displayName());
        setIconName(mCollection.iconName());
        setCachePolicy(mCollection.cachePolicy());
    });
}

Akonadi::CachePolicy CollectionEditorController::cachePolicy() const
{
    return mCachePolicy;
}

void CollectionEditorController::setCachePolicy(const Akonadi::CachePolicy &cachePolicy)
{
    if (mCachePolicy == cachePolicy) {
        return;
    }
    mCachePolicy = cachePolicy;
    Q_EMIT cachePolicyChanged();
}

QString CollectionEditorController::displayName() const
{
    return mDisplayName;
}

void CollectionEditorController::setDisplayName(const QString &displayName)
{
    if (mDisplayName == displayName) {
        return;
    }
    mDisplayName = displayName;
    Q_EMIT displayNameChanged();
}

QString CollectionEditorController::iconName() const
{
    return mIconName;
}

void CollectionEditorController::setIconName(const QString &iconName)
{
    if (mIconName == iconName) {
        return;
    }
    mIconName = iconName;
    Q_EMIT iconNameChanged();
}

void CollectionEditorController::save()
{
    Akonadi::Collection modifiedCollection(mId);

    bool modified = false;
    if (!mCollection.isValid()) {
        return;
    }

    if (mDisplayName != mCollection.displayName() || mIconName != mCollection.iconName()) {
        auto displayAttribute = mCollection.attribute<Akonadi::EntityDisplayAttribute>();
        displayAttribute->setDisplayName(mDisplayName);
        displayAttribute->setIconName(mIconName);
        modifiedCollection.addAttribute(displayAttribute);
        modified = true;
    }

    if (modified) {
        auto job = new Akonadi::CollectionModifyJob(modifiedCollection);
        connect(job, &Akonadi::CollectionModifyJob::result, job, [this, job](KJob *) {
            if (job->error()) {
                qWarning() << job->errorString();
            }
            mCollection = job->collection();
            setDisplayName(mCollection.displayName());
            setIconName(mCollection.iconName());
            setCachePolicy(mCollection.cachePolicy());
        });
    }
}
