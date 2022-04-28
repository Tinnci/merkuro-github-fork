// SPDX-FileCopyrightText: 2022 Jonah Brüchert <jbb@kaidan.im>
// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "avatarimageprovider.h"

#include "kalendar_debug.h"
#include <kad.h>
#include <persondata.h>

#include "kpeopledeclarative_debug.h"

AvatarResponse::AvatarResponse(qint64 itemId, QSize size)
    : m_itemId(itemId)
    , m_requestedSize(size)
    , errorStr(QStringLiteral("Image request hasn't started"))
{
    if (m_requestedSize.isEmpty()) {
        m_requestedSize.setHeight(100);
        m_requestedSize.setWidth(100);
    }

    QMetaObject::invokeMethod(this, &AvatarResponse::startRequest, Qt::QueuedConnection);
}

void AvatarResponse::startRequest()
{
    qint64 itemId = id.toLongLong();
    Akonadi::Item item(itemId);
    m_job = new Akonadi::ItemFetchJob(item);
    m_job->fetchScope().fetchFullPayload();

    connect(m_job, &Akonadi::ItemFetchJob::result, this, [this, itemId](KJob *job) {
        auto fetchJob = qobject_cast<Akonadi::ItemFetchJob *>(job);
        auto item = fetchJob->items().at(0);
        auto payload = item.payload<KContacts::Addressee>();
        QWriteLocker _(&lock);
    });
}

void AvatarResponse::doCancel()
{
    if (m_job) {
        m_job->kill();
    }
}

QQuickTextureFactory *AvatarResponse::textureFactory() const
{
    QReadLocker _(&m_lock);
    return QQuickTextureFactory::textureFactoryForImage(m_image);
}

QString ThumbnailResponse::errorString() const
{
    QReadLocker _(&m_lock);
    return errorStr;
}

void ThumbnailResponse::cancel()
{
    QMetaObject::invokeMethod(this, &ThumbnailResponse::doCancel, Qt::QueuedConnection);
}

AvatarImageProvider::AvatarImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap AvatarImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    qint64 itemId = id.toLongLong();
    Akonadi::Item item(itemId);
    auto job = new Akonadi::ItemFetchJob(item);
    job->fetchScope().fetchFullPayload();

    connect(job, &Akonadi::ItemFetchJob::result, this, [this, itemId](KJob *job) {
        auto fetchJob = qobject_cast<Akonadi::ItemFetchJob *>(job);
        auto item = fetchJob->items().at(0);
        auto payload = item.payload<KContacts::Addressee>();

        Q_EMIT emailsFetched(payload.emails(), itemId);
    });

    KPeople::PersonData person(personUri);
    if (!person.isValid()) {
        qCDebug(KALENDAR_LOG) << "AvatarImageProvider:"
                              << "No contact found with person uri" << personUri;
        return {};
    }

    const auto avatar = [&person]() -> QPixmap {
        QVariant pic = person.contactCustomProperty(KPeople::AbstractContact::PictureProperty);
        if (pic.canConvert<QImage>()) {
            return QPixmap::fromImage(pic.value<QImage>());
        } else if (pic.canConvert<QUrl>()) {
            return QPixmap(pic.toUrl().toLocalFile());
        }
        return {};
    }();

    if (avatar.isNull()) {
        return {};
    }

    if (size) {
        *size = requestedSize;
    }

    return avatar.scaled(requestedSize);
}
