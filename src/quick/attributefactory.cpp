// SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "attributefactory.h"

Akonadi::CollectionColorAttribute AttributeFactory::collectionColorAttribute(const Akonadi::Collection &collection)
{
    return *collection.attribute<Akonadi::CollectionColorAttribute>();
}

Akonadi::CollectionQuotaAttribute AttributeFactory::collectionQuotaAttribute(const Akonadi::Collection &collection)
{
    return *collection.attribute<Akonadi::CollectionQuotaAttribute>();
}
