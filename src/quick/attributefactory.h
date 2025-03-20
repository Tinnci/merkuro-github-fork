// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-;License-Identifier: LGPL-2.1-or-later

#pragma once

#include <Akonadi/Collection>
#include <Akonadi/CollectionColorAttribute>
#include <Akonadi/CollectionQuotaAttribute>
#include <Akonadi/Item>
#include <QObject>
#include <qqmlregistration.h>

class AttributeFactory : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit AttributeFactory(QObject *parent = nullptr);

    Q_INVOKABLE Akonadi::CollectionColorAttribute collectionColorAttribute(const Akonadi::Collection &collection);
    Q_INVOKABLE Akonadi::CollectionQuotaAttribute collectionQuotaAttribute(const Akonadi::Collection &collection);
};
