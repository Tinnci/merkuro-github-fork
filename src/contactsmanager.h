// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QSortFilterProxyModel>
#include <Akonadi/Contact/ContactsTreeModel>
#include <Akonadi/Contact/ContactsFilterProxyModel>
#include <AkonadiCore/EntityMimeTypeFilterModel>
#include <KItemModels/KDescendantsProxyModel>

class ContactsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Akonadi::EntityMimeTypeFilterModel * contactsModel READ contactsModel CONSTANT)

public:
    ContactsManager(QObject *parent = nullptr);
    ~ContactsManager() = default;

    Akonadi::EntityMimeTypeFilterModel *contactsModel();

private:
    Akonadi::EntityMimeTypeFilterModel *m_model = nullptr;
    KDescendantsProxyModel *m_flatModel = nullptr;
    Akonadi::ContactsTreeModel *m_sourceModel = nullptr;
    Akonadi::ContactsFilterProxyModel *m_filterModel = nullptr;
};
