// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.1-or-later
#pragma once

#include <KSharedConfig>
#include <KirigamiAbstractApplication>
#include <QObject>
#include <QSortFilterProxyModel>

class AbstractApplication : public KirigamiAbstractApplication
{
    Q_OBJECT

    /// This property holds whether the menubar is visible.
    Q_PROPERTY(bool menubarVisible READ menubarVisible NOTIFY menubarVisibleChanged)

public:
    explicit AbstractApplication(QObject *parent = nullptr);
    void toggleMenubar();

    bool menubarVisible() const;

Q_SIGNALS:
    void openSettings();
    void openTagManager();
    void menubarVisibleChanged();

protected:
    void setupActions() override;

private:
    KSharedConfig::Ptr m_shared;
};
