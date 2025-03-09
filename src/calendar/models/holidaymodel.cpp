// SPDX-FileCopyrightText: 2025 Shubham Shinde <shubshinde8381@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "holidaymodel.h"
#include "calendarconfig.h"
#include <KHolidays/HolidayRegion>
#include <QLocale>

HolidayModel::HolidayModel(QObject *parent)
    : QObject(parent)
{
}

QStringList HolidayModel::holidayRegions() const
{
    return m_holidayRegions;
}

void HolidayModel::setHolidayRegions(const QStringList &holidayRegions)
{
    qWarning() << Q_FUNC_INFO << holidayRegions;
    if (holidayRegions.isEmpty()) {
        const QLocale locale = QLocale::system();
        const QString country = locale.name().split(u'_').last().toUpper();
        const QString language = locale.name().split(u'_').constFirst().toLower();

        QString region = KHolidays::HolidayRegion::defaultRegionCode(country, language);
        if (KHolidays::HolidayRegion::isValid(region)) {
            m_holidayRegions = {region};
        }
        return;
    }
    if (m_holidayRegions == holidayRegions) {
        return;
    }
    m_holidayRegions = holidayRegions;
    Q_EMIT holidayRegionsChanged();
}

void HolidayModel::setDateRange(const QDate &start, const QDate &end)
{
    m_holidays.clear();
    for (const auto &regionCode : std::as_const(m_holidayRegions)) {
        KHolidays::HolidayRegion region(regionCode);
        const auto holidays = region.rawHolidays(start, end);
        for (const auto &holiday : holidays) {
            if (holiday.dayType() != KHolidays::Holiday::NonWorkday) {
                continue;
            }

            const QDate date = holiday.observedStartDate();
            m_holidays[date].append(holiday.name());
        }
    }
}

QDate HolidayModel::addDaysToDate(const QDate &date, int days) const
{
    return date.addDays(days);
}

QStringList HolidayModel::getHolidays(const QDate &date) const
{
    return m_holidays.value(date);
}

#include "moc_holidaymodel.cpp"
