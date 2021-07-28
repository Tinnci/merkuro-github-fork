// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <QSortFilterProxyModel>
#include <Akonadi/Contact/ContactsTreeModel>
#include <Akonadi/Contact/ContactsFilterProxyModel>
#include <KItemModels/KDescendantsProxyModel>

class ContactsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KDescendantsProxyModel * contactsModel READ contactsModel CONSTANT)

public:
    ContactsManager(QObject *parent = nullptr);
    ~ContactsManager() = default;

    KDescendantsProxyModel *contactsModel();

private:
    KDescendantsProxyModel *m_model = nullptr;
    Akonadi::ContactsTreeModel *m_sourceModel = nullptr;
    Akonadi::ContactsFilterProxyModel *m_filterModel = nullptr;
};
