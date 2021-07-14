// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "dailyeventsmodel.h"

enum Roles {
    Events = EventOccurrenceModel::LastRole,
    Date
};

DailyEventsModel::DailyEventsModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QModelIndex DailyEventsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    if (!parent.isValid()) {
        return createIndex(row, column);
    }
    return {};
}

QModelIndex DailyEventsModel::parent(const QModelIndex &) const
{
    return {};
}

int DailyEventsModel::rowCount(const QModelIndex &parent) const
{
    //Number of weeks
    if (!parent.isValid() && mSourceModel) {
        return qMax(mSourceModel->length(), 1);
    }
    return 0;
}

int DailyEventsModel::columnCount(const QModelIndex &) const
{
    return 1;
}


static long long getDuration(const QDate &start, const QDate &end)
{
    return qMax(start.daysTo(end) + 1, 1ll);
}

// We first sort all occurences so we get all-day first (sorted by duration),
// and then the rest sorted by start-date.
QList<QModelIndex> DailyEventsModel::sortedEventsFromSourceModel(const QDate &rowStart) const
{
    const auto rowEnd = rowStart;
    QList<QModelIndex> sorted;
    sorted.reserve(mSourceModel->rowCount());
    for (int row = 0; row < mSourceModel->rowCount(); row++) {
        const auto srcIdx = mSourceModel->index(row, 0, {});
        const auto start = srcIdx.data(EventOccurrenceModel::StartTime).toDateTime().date();
        const auto end = srcIdx.data(EventOccurrenceModel::EndTime).toDateTime().date();
        //Skip events not part of the week
        if (end < rowStart || start > rowEnd) {
            // qWarning() << "Skipping because not part of this week";
            continue;
        }
        // qWarning() << "found " << srcIdx.data(EventOccurrenceModel::StartTime).toDateTime() << srcIdx.data(EventOccurrenceModel::Summary).toString();
        sorted.append(srcIdx);
    }
    std::sort(sorted.begin(), sorted.end(), [&] (const QModelIndex &left, const QModelIndex &right) {
        //All-day first, sorted by duration (in the hope that we can fit multiple on the same line)
        const auto leftAllDay = left.data(EventOccurrenceModel::AllDay).toBool();
        const auto rightAllDay = right.data(EventOccurrenceModel::AllDay).toBool();
        if (leftAllDay && !rightAllDay) {
            return true;
        }
        if (!leftAllDay && rightAllDay) {
            return false;
        }
        if (leftAllDay && rightAllDay) {
            const auto leftDuration = getDuration(left.data(EventOccurrenceModel::StartTime).toDateTime().date(), left.data(EventOccurrenceModel::EndTime).toDateTime().date());
            const auto rightDuration = getDuration(right.data(EventOccurrenceModel::StartTime).toDateTime().date(), right.data(EventOccurrenceModel::EndTime).toDateTime().date());
            return leftDuration < rightDuration;
        }
        //The rest sored by start date
        return left.data(EventOccurrenceModel::StartTime).toDateTime() < right.data(EventOccurrenceModel::StartTime).toDateTime();
    });
    return sorted;
}

/*
* Layout the lines:
*
* The line grouping algorithm then always picks the first event,
* and tries to add more to the same line.
*
* We never mix all-day and non-all day, and otherwise try to fit as much as possible
* on the same line. Same day time-order should be preserved because of the sorting.
*/
QVariantList DailyEventsModel::eventsOnDay(const QDate &rowStart) const
{
    auto getStart = [&rowStart] (const QDate &start) {
        return qMax(rowStart.daysTo(start), 0ll);
    };

    QList<QModelIndex> sorted = sortedEventsFromSourceModel(rowStart);

    auto result = QVariantList{};
    while (!sorted.isEmpty()) {
        const auto srcIdx = sorted.takeFirst();
        const auto startDate = srcIdx.data(EventOccurrenceModel::StartTime).toDateTime();
        const auto start = getStart(startDate.date());
        const auto duration = qMin(getDuration(startDate.date(), srcIdx.data(EventOccurrenceModel::EndTime).toDateTime().date()), mPeriodLength - start);
        // qWarning() << "First of line " << srcIdx.data(EventOccurrenceModel::StartTime).toDateTime() << duration << srcIdx.data(EventOccurrenceModel::Summary).toString();
        auto currentDay = QVariantList{};

        auto addToDay = [&currentDay] (const QModelIndex &idx, int start, int duration) {
            currentDay.append(QVariantMap{
                {QStringLiteral("text"), idx.data(EventOccurrenceModel::Summary)},
                {QStringLiteral("description"), idx.data(EventOccurrenceModel::Description)},
                {QStringLiteral("startTime"), idx.data(EventOccurrenceModel::StartTime)},
                {QStringLiteral("endTime"), idx.data(EventOccurrenceModel::EndTime)},
                {QStringLiteral("allDay"), idx.data(EventOccurrenceModel::AllDay)},
                {QStringLiteral("starts"), start},
                {QStringLiteral("duration"), duration},
                {QStringLiteral("durationString"), idx.data(EventOccurrenceModel::DurationString)},
                {QStringLiteral("color"), idx.data(EventOccurrenceModel::Color)},
                {QStringLiteral("collectionId"), idx.data(EventOccurrenceModel::CollectionId)},
                {QStringLiteral("eventPtr"), idx.data(EventOccurrenceModel::EventPtr)},
                {QStringLiteral("eventOccurrence"), idx.data(EventOccurrenceModel::EventOccurrence)}
            });
        };

        //Add first event of line
        addToDay(srcIdx, start, duration);
        const bool allDayEvent = srcIdx.data(EventOccurrenceModel::AllDay).toBool();

        for (auto it = sorted.begin(); it != sorted.end();) {
            const auto idx = *it;
            const auto start = getStart(idx.data(EventOccurrenceModel::StartTime).toDateTime().date());
            const auto duration = qMin(getDuration(idx.data(EventOccurrenceModel::StartTime).toDateTime().date(), idx.data(EventOccurrenceModel::EndTime).toDateTime().date()), mPeriodLength - start);

            // qWarning() << "Checking " << idx.data(EventOccurrenceModel::StartTime).toDateTime() << duration << idx.data(EventOccurrenceModel::Summary).toString();
            //Avoid mixing all-day and other events
            if (allDayEvent && !idx.data(EventOccurrenceModel::AllDay).toBool()) {
                break;
            }

            addToDay(idx, start, duration);
            it = sorted.erase(it);

        }
        // qWarning() << "Appending line " << currentLine;
        result.append(QVariant::fromValue(currentDay));
    }
    return result;
}

QVariant DailyEventsModel::data(const QModelIndex &idx, int role) const
{
    if (!hasIndex(idx.row(), idx.column())) {
        return {};
    }
    if (!mSourceModel) {
        return {};
    }
    const auto rowStart = mSourceModel->start().addDays(idx.row());
    switch (role) {
        case Date:
            return rowStart;
        case Events:
            return eventsOnDay(rowStart);
        default:
            Q_ASSERT(false);
            return {};
    }
}

void DailyEventsModel::setModel(EventOccurrenceModel *model)
{
    beginResetModel();
    mSourceModel = model;
    auto resetModel = [this] {
        beginResetModel();
        endResetModel();
    };
    QObject::connect(model, &QAbstractItemModel::dataChanged, this, resetModel);
    QObject::connect(model, &QAbstractItemModel::layoutChanged, this, resetModel);
    QObject::connect(model, &QAbstractItemModel::modelReset, this, resetModel);
    QObject::connect(model, &QAbstractItemModel::rowsInserted, this, resetModel);
    QObject::connect(model, &QAbstractItemModel::rowsMoved, this, resetModel);
    QObject::connect(model, &QAbstractItemModel::rowsRemoved, this, resetModel);
    endResetModel();
}

QHash<int, QByteArray> DailyEventsModel::roleNames() const
{
    return {
        {Events, "events"},
        {Date, "date"}
    };
}

