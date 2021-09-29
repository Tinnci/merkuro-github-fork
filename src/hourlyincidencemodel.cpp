// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "hourlyincidencemodel.h"
#include <QBitArray>
#include <QTimeZone>

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


static double getDuration(const QDateTime &start, const QDateTime &end, int periodLength)
{
    return ((start.secsTo(end) * 1.0) / 60.0) / periodLength;
}

// We first sort all occurences so we get all-day first (sorted by duration),
// and then the rest sorted by start-date.
QList<QModelIndex> HourlyIncidenceModel::sortedIncidencesFromSourceModel(const QDateTime &rowStart) const
{
    // Don't add days if we are going for a daily period
    const auto rowEnd = rowStart.date().endOfDay();
    QList<QModelIndex> sorted;
    sorted.reserve(mSourceModel->rowCount());
    // Get incidences from source model
    for (int row = 0; row < mSourceModel->rowCount(); row++) {
        const auto srcIdx = mSourceModel->index(row, 0, {});
        const auto start = srcIdx.data(IncidenceOccurrenceModel::StartTime).toDateTime().toTimeZone(QTimeZone::systemTimeZone());
        const auto end = srcIdx.data(IncidenceOccurrenceModel::EndTime).toDateTime().toTimeZone(QTimeZone::systemTimeZone());

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

    auto addToResultsAndGetPtr = [&result] (const QModelIndex &idx, double start, double duration) {
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
    };

    // Since our hourly view displays by the minute, we need to know how many incidences there are in each minute
    QHash<int, int> takenSpaces;
    auto setTakenSpaces = [&](int start, int end) {
        for(int i = start; i < end; i++) {
            if(!takenSpaces.contains(i)) {
                takenSpaces[i] = 1;
            } else {
                takenSpaces[i]++;
            }
        }
    };

    while (!sorted.isEmpty()) {
        const auto idx = sorted.takeFirst();
        const auto startDT = idx.data(IncidenceOccurrenceModel::StartTime).toDateTime().toTimeZone(QTimeZone::systemTimeZone());
        const auto endDT = idx.data(IncidenceOccurrenceModel::EndTime).toDateTime().toTimeZone(QTimeZone::systemTimeZone());
        // Need to convert ints into doubles to get more accurate starting positions
        const auto start = ((startDT.time().hour() * 1.0) * (60.0 / mPeriodLength)) + ((startDT.time().minute() * 1.0) / mPeriodLength);
        const auto duration = qMax(getDuration(startDT, idx.data(IncidenceOccurrenceModel::EndTime).toDateTime().toTimeZone(QTimeZone::systemTimeZone()), mPeriodLength), 1.0);
        const auto end = qMin(start + duration, (24.0 * 60.0) / mPeriodLength);

        const auto startMinutesFromDayStart = (startDT.time().hour() * 60) + startDT.time().minute();
        const auto endMinutesFromDayStart = qMin((endDT.time().hour() * 60) + endDT.time().minute(), 24 * 60 * 60);

        addToResultsAndGetPtr(idx, start, duration);
        setTakenSpaces(startMinutesFromDayStart, endMinutesFromDayStart);
    }

    // Set fraction of width
    QHash<int, double> takenWidth;
    QHash<int, double> startX;

    for(int i = 0; i < result.length(); i++) {
        auto incidence = result[i].value<QVariantMap>();
        int concurrentIncidences = 1;

        const auto startDT = incidence[QLatin1String("startTime")].toDateTime().toTimeZone(QTimeZone::systemTimeZone());
        const auto endDT = incidence[QLatin1String("endTime")].toDateTime().toTimeZone(QTimeZone::systemTimeZone());

        const auto endMinutesFromDayStart = qMin((endDT.time().hour() * 60) + endDT.time().minute(), 24 * 60 * 60);
        const auto startMinutesFromDayStart = startDT.isValid() ?
            (startDT.time().hour() * 60) + startDT.time().minute() : qMax(endMinutesFromDayStart - mPeriodLength, 0);

        for(int i = startMinutesFromDayStart; i < endMinutesFromDayStart; i++) {
            concurrentIncidences = qMax(concurrentIncidences, takenSpaces[i]);
        }

        incidence[QLatin1String("maxConcurrentIncidences")] = concurrentIncidences;
        double widthShare = 1.0 / (concurrentIncidences * 1.0);
        incidence[QLatin1String("widthShare")] = widthShare;

        double priorTakenWidthShare = 0;

        for(int i = startMinutesFromDayStart; i < endMinutesFromDayStart; i++) {
            if(!takenWidth.contains(i)) {
                takenWidth[i] = widthShare;
                startX[i] = priorTakenWidthShare;
            } else {
                priorTakenWidthShare = qMax(priorTakenWidthShare, takenWidth[i]);
                takenWidth[i] += widthShare;

                if(startX[i] > 0) {
                    priorTakenWidthShare = 0;
                    takenWidth[i] = widthShare;
                    startX[i] = 0;
                }
            }
        }

        if(takenSpaces[startMinutesFromDayStart] < takenSpaces[endMinutesFromDayStart - 1] && priorTakenWidthShare > 0)  {
            priorTakenWidthShare = widthShare * (takenSpaces[endMinutesFromDayStart - 1] - 1);
        }

        incidence[QLatin1String("priorTakenWidthShare")] = priorTakenWidthShare;

        result[i] = incidence;
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
    const auto rowStart = mSourceModel->start().addDays(idx.row()).startOfDay();
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
        {PeriodStartDateTime, "periodStartDateTime"}
    };
}
