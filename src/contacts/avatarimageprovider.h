e / SPDX
    - FileCopyrightText : 2022 Jonah Brüchert<jbb @kaidan.im>
// SPDX-FileCopyrightText: 2022 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <Akonadi/ItemFetchJob>
#include <QAtomicPointer>
#include <QObject>
#include <QQuickAsyncImageProvider>
#include <QReadWriteLock>

    class AvatarResponse : public QQuickImageResponse
{
    Q_OBJECT
public:
    AvatarResponse(qint64 itemId, QSize requestedSize);
    ~AvatarResponse() override = default;

private Q_SLOTS:
    void startRequest();
    void doCancel();

private:
    const qint64 m_itemId;
    QSize m_requestedSize;

    QImage m_image;
    QString errorStr;
    mutable QReadWriteLock m_lock; // Guards ONLY these two members above

    QQuickTextureFactory *textureFactory() const override;
    QString errorString() const override;
    void cancel() override;

    Akonadi::ItemFetchJob *m_job = nullptr;
};

/**
 * @brief An image provider for displaying a contact's photo
 */
class AvatarImageProvider : public QQuickImageProvider
{
public:
    explicit AvatarImageProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
};
