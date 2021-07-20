// SPDX-FileCopyrightText: 2021 Claudio Cambra <claudio.cambra@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QObject>
#include <CalendarSupport/KCalPrefs>
#include <CalendarSupport/Utils>
#include <AkonadiCore/CollectionIdentificationAttribute>
#include "remindersmodel.h"
#include "attendeesmodel.h"
#include "recurrenceexceptionsmodel.h"
#include "attachmentsmodel.h"

/**
 * This class is a wrapper for a KCalendarCore::Incidence::Ptr object.
 * We can use it to create new incidences, or create incidence pointers from
 * pre-existing incidences, to more cleanly pass around to our QML code
 * or to the CalendarManager, which handles the back-end stuff of
 * adding and editing the incidence in the collection of our choice.
 */

class IncidenceWrapper : public QObject
{
    Q_OBJECT
    Q_PROPERTY(KCalendarCore::Incidence::Ptr incidencePtr READ incidencePtr WRITE setIncidencePtr NOTIFY incidencePtrChanged)
    Q_PROPERTY(KCalendarCore::Incidence::Ptr originalIncidencePtr READ originalIncidencePtr NOTIFY originalIncidencePtrChanged)
    Q_PROPERTY(qint64 collectionId READ collectionId WRITE setCollectionId NOTIFY collectionIdChanged)
    Q_PROPERTY(QString summary READ summary WRITE setSummary NOTIFY summaryChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QString location READ location WRITE setLocation NOTIFY locationChanged)
    Q_PROPERTY(QDateTime incidenceStart READ incidenceStart WRITE setIncidenceStart NOTIFY incidenceStartChanged)
    Q_PROPERTY(QDateTime incidenceEnd READ incidenceEnd WRITE setIncidenceEnd NOTIFY incidenceEndChanged)
    Q_PROPERTY(bool allDay READ allDay WRITE setAllDay NOTIFY allDayChanged)
    Q_PROPERTY(KCalendarCore::Recurrence * recurrence READ recurrence)
    Q_PROPERTY(QVariantMap recurrenceData READ recurrenceData NOTIFY recurrenceDataChanged)
    Q_PROPERTY(QVariantMap organizer READ organizer NOTIFY organizerChanged)
    Q_PROPERTY(KCalendarCore::Attendee::List attendees READ attendees)
    Q_PROPERTY(RemindersModel * remindersModel READ remindersModel NOTIFY remindersModelChanged)
    Q_PROPERTY(AttendeesModel * attendeesModel READ attendeesModel NOTIFY attendeesModelChanged)
    Q_PROPERTY(RecurrenceExceptionsModel * recurrenceExceptionsModel READ recurrenceExceptionsModel NOTIFY recurrenceExceptionsModelChanged)
    Q_PROPERTY(AttachmentsModel * attachmentsModel READ attachmentsModel NOTIFY attachmentsModelChanged)
    Q_PROPERTY(QVariantMap recurrenceIntervals READ recurrenceIntervals CONSTANT)

public:
    enum RecurrenceIntervals {
        Daily,
        Weekly,
        Monthly,
        Yearly
    };
    Q_ENUM(RecurrenceIntervals);

    IncidenceWrapper(QObject *parent = nullptr);
    ~IncidenceWrapper() = default;

    KCalendarCore::Incidence::Ptr incidencePtr() const;
    void setIncidencePtr(KCalendarCore::Incidence::Ptr incidencePtr);
    KCalendarCore::Incidence::Ptr originalIncidencePtr();
    qint64 collectionId();
    void setCollectionId(qint64 collectionId);
    QString summary() const;
    void setSummary(QString summary);
    QString description() const;
    void setDescription(QString description);
    QString location() const;
    void setLocation(QString location);
    QDateTime incidenceStart() const;
    void setIncidenceStart(QDateTime incidenceStart);
    QDateTime incidenceEnd() const;
    void setIncidenceEnd(QDateTime incidenceEnd);
    bool allDay() const;
    void setAllDay(bool allDay);

    KCalendarCore::Recurrence * recurrence() const;
    QVariantMap recurrenceData();
    Q_INVOKABLE void setRecurrenceDataItem(QString key, QVariant value);
    QVariantMap recurrenceIntervals();

    QVariantMap organizer();
    KCalendarCore::Attendee::List attendees() const;

    RemindersModel * remindersModel();
    AttendeesModel * attendeesModel();
    RecurrenceExceptionsModel * recurrenceExceptionsModel();
    AttachmentsModel * attachmentsModel();

    Q_INVOKABLE void addAlarms(KCalendarCore::Alarm::List alarms);
    Q_INVOKABLE void setRegularRecurrence(RecurrenceIntervals interval, int freq = 1);
    Q_INVOKABLE void setMonthlyPosRecurrence(short pos, int day);
    Q_INVOKABLE void setRecurrenceOcurrences(int ocurrences);
    Q_INVOKABLE void clearRecurrences();

Q_SIGNALS:
    void incidencePtrChanged(KCalendarCore::Incidence::Ptr incidencePtr);
    void originalIncidencePtrChanged();
    void collectionIdChanged();
    void summaryChanged();
    void descriptionChanged();
    void locationChanged();
    void incidenceStartChanged();
    void incidenceEndChanged();
    void allDayChanged();
    void remindersModelChanged();
    void recurrenceDataChanged();
    void organizerChanged();
    void attendeesModelChanged();
    void recurrenceExceptionsModelChanged();
    void attachmentsModelChanged();

private:
    KCalendarCore::Incidence::Ptr m_incidence;
    KCalendarCore::Incidence::Ptr m_originalIncidence;
    qint64 m_collectionId;
    RemindersModel m_remindersModel;
    AttendeesModel m_attendeesModel;
    RecurrenceExceptionsModel m_recurrenceExceptionsModel;
    AttachmentsModel m_attachmentsModel;
    QVariantMap m_recurrenceIntervals;
};
