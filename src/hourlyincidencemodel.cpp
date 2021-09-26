// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "hourlyincidencemodel.h"
#include <QBitArray>

enum Roles {
    Incidences = IncidenceOccurrenceModel::LastRole,
    PeriodStartDateTime
};

HourlyIncidenceModel::HourlyIncidenceModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    mRefreshTimer.setSingleShot(true);
}

QModelIndex HourlyIncidenceModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return {};
    }

    if (!parent.isValid()) {
        return createIndex(row, column);
    }
    return {};
}

QModelIndex HourlyIncidenceModel::parent(const QModelIndex &) const
{
    return {};
}

int HourlyIncidenceModel::rowCount(const QModelIndex &parent) const
{
    //Number of weeks
    if (!parent.isValid() && mSourceModel) {
        return qMax(mSourceModel->length(), 1);
    }
    return 0;
}

int HourlyIncidenceModel::columnCount(const QModelIndex &) const
{
    return 1;
}


static long long getDuration(const QDateTime &start, const QDateTime &end, int periodLength)
{
    return (start.secsTo(end) / 60) / periodLength;
}

// We first sort all occurences so we get all-day first (sorted by duration),
// and then the rest sorted by start-date.
QList<QModelIndex> HourlyIncidenceModel::sortedIncidencesFromSourceModel(const QDateTime &rowStart) const
{
    // Don't add days if we are going for a daily period
    const auto rowEnd = rowStart.addDays(mPeriodLength > 1 ? mPeriodLength : 0);
    QList<QModelIndex> sorted;
    sorted.reserve(mSourceModel->rowCount());
    // Get incidences from source model
    for (int row = 0; row < mSourceModel->rowCount(); row++) {
        const auto srcIdx = mSourceModel->index(row, 0, {});
        const auto start = srcIdx.data(IncidenceOccurrenceModel::StartTime).toDateTime();
        const auto end = srcIdx.data(IncidenceOccurrenceModel::EndTime).toDateTime();

        //Skip incidences not part of the week
        if (end < rowStart || start > rowEnd) {
            // qWarning() << "Skipping because not part of this week";
            continue;
        }
        // qWarning() << "found " << srcIdx.data(IncidenceOccurrenceModel::StartTime).toDateTime() << srcIdx.data(IncidenceOccurrenceModel::Summary).toString();
        sorted.append(srcIdx);
    }

    // Sort incidences by date
    std::sort(sorted.begin(), sorted.end(), [&] (const QModelIndex &left, const QModelIndex &right) {
        //All-day first
        const auto leftAllDay = left.data(IncidenceOccurrenceModel::AllDay).toBool();
        const auto rightAllDay = right.data(IncidenceOccurrenceModel::AllDay).toBool();

        const auto leftDt = left.data(IncidenceOccurrenceModel::StartTime).toDateTime();
        const auto rightDt = right.data(IncidenceOccurrenceModel::StartTime).toDateTime();

        if (leftAllDay && !rightAllDay) {
            return true;
        }
        if (!leftAllDay && rightAllDay) {
            return false;
        }

        //The rest sorted by start date
        return leftDt < rightDt;
    });

    return sorted;
}

/*
* Layout the lines:
*
* The line grouping algorithm then always picks the first incidence,
* and tries to add more to the same line.
*
* We never mix all-day and non-all day, and otherwise try to fit as much as possible
* on the same line. Same day time-order should be preserved because of the sorting.
*/
QVariantList HourlyIncidenceModel::layoutLines(const QDateTime &rowStart) const
{

    QList<QModelIndex> sorted = sortedIncidencesFromSourceModel(rowStart);

    // for (const auto &srcIdx : sorted) {
    //     qWarning() << "sorted " << srcIdx.data(IncidenceOccurrenceModel::StartTime).toDateTime() << srcIdx.data(IncidenceOccurrenceModel::Summary).toString() << srcIdx.data(IncidenceOccurrenceModel::AllDay).toBool();
    // }
    auto result = QVariantList{};

    auto addToResultsAndGetPtr = [&result] (const QModelIndex &idx, int start, int duration) {
        auto incidenceMap = QVariantMap{
            {QStringLiteral("text"), idx.data(IncidenceOccurrenceModel::Summary)},
            {QStringLiteral("description"), idx.data(IncidenceOccurrenceModel::Description)},
            {QStringLiteral("location"), idx.data(IncidenceOccurrenceModel::Location)},
            {QStringLiteral("startTime"), idx.data(IncidenceOccurrenceModel::StartTime)},
            {QStringLiteral("endTime"), idx.data(IncidenceOccurrenceModel::EndTime)},
            {QStringLiteral("allDay"), idx.data(IncidenceOccurrenceModel::AllDay)},
            {QStringLiteral("todoCompleted"), idx.data(IncidenceOccurrenceModel::TodoCompleted)},
            {QStringLiteral("priority"), idx.data(IncidenceOccurrenceModel::Priority)},
            {QStringLiteral("starts"), start},
            {QStringLiteral("duration"), duration},
            {QStringLiteral("maxConcurrentIncidences"), 1}, // Important
            {QStringLiteral("durationString"), idx.data(IncidenceOccurrenceModel::DurationString)},
            {QStringLiteral("recurs"), idx.data(IncidenceOccurrenceModel::Recurs)},
            {QStringLiteral("hasReminders"), idx.data(IncidenceOccurrenceModel::HasReminders)},
            {QStringLiteral("isOverdue"), idx.data(IncidenceOccurrenceModel::IsOverdue)},
            {QStringLiteral("color"), idx.data(IncidenceOccurrenceModel::Color)},
            {QStringLiteral("collectionId"), idx.data(IncidenceOccurrenceModel::CollectionId)},
            {QStringLiteral("incidenceId"), idx.data(IncidenceOccurrenceModel::IncidenceId)},
            {QStringLiteral("incidenceType"), idx.data(IncidenceOccurrenceModel::IncidenceType)},
            {QStringLiteral("incidenceTypeStr"), idx.data(IncidenceOccurrenceModel::IncidenceTypeStr)},
            {QStringLiteral("incidenceTypeIcon"), idx.data(IncidenceOccurrenceModel::IncidenceTypeIcon)},
            {QStringLiteral("incidencePtr"), idx.data(IncidenceOccurrenceModel::IncidencePtr)},
            {QStringLiteral("incidenceOccurrence"), idx.data(IncidenceOccurrenceModel::IncidenceOccurrence)},
        };

        result.append(incidenceMap);

        QVariantMap *incidenceMapPtr = &incidenceMap;
        return incidenceMapPtr;
    };

    QVector<QVector<QVariantMap*>> takenSpaces((24 * 60) / mPeriodLength);

    auto concurrentIncidenceCountHelper = [&] (int start, int end, QVariantMap *mapPtr) {
        for(int i = start; i < end; i++) {
            takenSpaces[i].append(mapPtr);

            if(takenSpaces[i].length() > 1) {
                for(auto incidence : takenSpaces[i]) {
                    (*incidence)[QStringLiteral("maxConcurrentIncidences")] = qMax((*incidence)[QStringLiteral("maxConcurrentIncidences")].toInt(), takenSpaces[i].length());
                }
            }
        }
    };

    while (!sorted.isEmpty()) {
        const auto idx = sorted.takeFirst();
        const auto startDT = idx.data(IncidenceOccurrenceModel::StartTime).toDateTime();
        const auto start = (startDT.time().hour() * (60 / mPeriodLength)) + (startDT.time().minute() / mPeriodLength);
        const auto duration = qMin(getDuration(startDT, idx.data(IncidenceOccurrenceModel::EndTime).toDateTime(), mPeriodLength), 1ll);
        const auto end = start + duration;

        // This leaves a space in rows with all day events, making this y area of the row exclusively for all day events
        /*if (allDayLine && !idx.data(IncidenceOccurrenceModel::AllDay).toBool()) {
         *        continue;
    }*/

        concurrentIncidenceCountHelper(start, end, addToResultsAndGetPtr(idx, start, duration));
    }

    // Set fraction of width
    for(int i = 0; i < takenSpaces.length(); i++) {

        if(takenSpaces[i].length() > 1) {
            double availableWidthShare = 1.0;
            int remainingIncidencesOnLine = takenSpaces[i].length();

            for(auto incidence : takenSpaces[i]) {
                int start = (*incidence)[QStringLiteral("start")].toInt();
                int end = (*incidence)[QStringLiteral("end")].toInt();
                int maxConcurrentIncidences = (*incidence)[QStringLiteral("maxConcurrentIncidences")].toInt();
                double widthShare = 1.0;

                for(int j = start; j < end; j++) {
                   widthShare = qMin(availableWidthShare / remainingIncidencesOnLine, 1.0 / maxConcurrentIncidences);
                   (*incidence)[QStringLiteral("widthShare")] = widthShare;
                   availableWidthShare -= widthShare;
                }
            }
        }
    }

    return result;
}

QVariant HourlyIncidenceModel::data(const QModelIndex &idx, int role) const
{
    if (!hasIndex(idx.row(), idx.column())) {
        return {};
    }
    if (!mSourceModel) {
        return {};
    }
    const auto rowStart = mSourceModel->start().addDays(idx.row() * mPeriodLength).startOfDay();
    switch (role) {
        case PeriodStartDateTime:
            return rowStart;
        case Incidences:
            return layoutLines(rowStart);
        default:
            Q_ASSERT(false);
            return {};
    }
}

void HourlyIncidenceModel::setModel(IncidenceOccurrenceModel *model)
{
    beginResetModel();
    mSourceModel = model;
    auto resetModel = [this] {
        if (!mRefreshTimer.isActive()) {
            beginResetModel();
            endResetModel();
            mRefreshTimer.start(50);
        }
    };
    QObject::connect(model, &QAbstractItemModel::dataChanged, this, resetModel);
    QObject::connect(model, &QAbstractItemModel::layoutChanged, this, resetModel);
    QObject::connect(model, &QAbstractItemModel::modelReset, this, resetModel);
    QObject::connect(model, &QAbstractItemModel::rowsInserted, this, resetModel);
    QObject::connect(model, &QAbstractItemModel::rowsMoved, this, resetModel);
    QObject::connect(model, &QAbstractItemModel::rowsRemoved, this, resetModel);
    endResetModel();
}

int HourlyIncidenceModel::periodLength()
{
    return mPeriodLength;
}

void HourlyIncidenceModel::setPeriodLength(int periodLength)
{
    mPeriodLength = periodLength;
}

QHash<int, QByteArray> HourlyIncidenceModel::roleNames() const
{
    return {
        {Incidences, "incidences"},
        {PeriodStartDate, "periodStartDateTime"}
    };
}
