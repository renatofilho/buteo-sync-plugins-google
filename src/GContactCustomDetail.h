/*
 * This file is part of buteo-gcontact-plugin package
 *
 * Copyright (C) 2013 Jolla Ltd. and/or its subsidiary(-ies).
 *
 * Contributors: Sateesh Kavuri <sateesh.kavuri@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef GCONTACTCUSTOMDETAIL_H
#define GCONTACTCUSTOMDETAIL_H

#include <QContactDetail>

QTM_USE_NAMESPACE

class GContactCustomDetail : public QContactDetail
{
public:

    Q_DECLARE_CUSTOM_CONTACT_DETAIL(GContactCustomDetail, "GoogleContact")
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactETag, "GContactETag");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactBillingInformation, "GContactBillingInformation");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactCalendarLink, "GContactCalendarLink");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactDirectoryServer, "GContactDirectoryServer");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactEvent, "GContactEvent");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactExternalId, "GContactExternalId");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactGroupMembershipInfo, "GContactGroupMembershipInfo");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactInitials, "GContactInitials");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactJot, "GContactJot");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactLanguage, "GContactLanguate");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactMileage, "GContactMileage");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactOccupation, "GContactOccupation");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactPriority, "GContactPriority");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactRelation, "GContactRelation");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactSensitivity, "GContactSensitivity");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactSubject, "GContactSubject");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactSystemGroup, "GContactSystemGroup");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactWebsite, "GContactWebsite");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactCountry, "GContactCountry");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactFeedLink, "GContactFeedLink");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactMoney, "GContactMoney");
    Q_DECLARE_LATIN1_CONSTANT(FieldGContactRating, "GContactRating");

    void setETag (const QString& etag) { setValue(FieldGContactETag, etag); }
    QString etag () const { return value<QString>(FieldGContactETag); }

    void setBillingInformation (const QString& billingInfo) { setValue(FieldGContactBillingInformation, billingInfo); }
    QString billingInformation () const { return value<QString>(FieldGContactBillingInformation); }

    void setCalendarLink (const QString& calendarLink) { setValue(FieldGContactCalendarLink, calendarLink); }
    QString calendarLink () const { return value<QString>(FieldGContactCalendarLink); }

    void setDirectoryServer (const QString& directoryServer) { setValue(FieldGContactDirectoryServer, directoryServer); }
    QString directoryServer () const { return value<QString>(FieldGContactDirectoryServer); }

    void setEvent (const QString& event) { setValue(FieldGContactEvent, event); }
    QString event () const { return value<QString>(FieldGContactEvent); }

    void setExternalId (const QString& externalId) { setValue(FieldGContactExternalId, externalId); }
    QString externalId () const { return value<QString>(FieldGContactExternalId); }

    void setGroupMembershipInfo (const QString& groupMembershipInfo) { setValue(FieldGContactGroupMembershipInfo, groupMembershipInfo); }
    QString groupMembershipInfo () const { return value<QString>(FieldGContactGroupMembershipInfo); }

    void setInitials (const QString& initials) { setValue(FieldGContactInitials, initials); }
    QString initials () const { return value<QString>(FieldGContactInitials); }

    void setJot (const QString& jot) { setValue(FieldGContactJot, jot); }
    QString jot () const { return value<QString>(FieldGContactJot); }

    void setLanguage (const QString& language) { setValue(FieldGContactLanguage, language); }
    QString language () const { return value<QString>(FieldGContactLanguage); }

    void setMileage (const QString& mileage) { setValue(FieldGContactMileage, mileage); }
    QString mileage () const { return value<QString>(FieldGContactMileage); }

    void setOccupation (const QString& occupation) { setValue(FieldGContactOccupation, occupation); }
    QString occupation () const { return value<QString>(FieldGContactOccupation); }

    void setPriority (const QString& priority) { setValue(FieldGContactPriority, priority); }
    QString priority () const { return value<QString>(FieldGContactPriority); }

    void setRelation (const QString& relation) { setValue(FieldGContactRelation, relation); }
    QString relation () const { return value<QString>(FieldGContactRelation); }

    void setSensitivity (const QString& sensitivity) { setValue(FieldGContactSensitivity, sensitivity); }
    QString sensitivity () const { return value<QString>(FieldGContactSensitivity); }

    void setSubject (const QString& subject) { setValue(FieldGContactSubject, subject); }
    QString subject () const { return value<QString>(FieldGContactSubject); }

    void setSystemGroup (const QString& systemGroup) { setValue(FieldGContactSystemGroup, systemGroup); }
    QString systemGroup () const { return value<QString>(FieldGContactSystemGroup); }

    void setWebsite (const QString& website) { setValue(FieldGContactWebsite, website); }
    QString website () const { return value<QString>(FieldGContactWebsite); }

    void setCountry (const QString& country) { setValue(FieldGContactCountry, country); }
    QString country () const { return value<QString>(FieldGContactCountry); }

    void setFeedLink (const QString& feedLink) { setValue(FieldGContactFeedLink, feedLink); }
    QString feedLink () const { return value<QString>(FieldGContactFeedLink); }

    void setMoney (const QString& money) { setValue(FieldGContactMoney, money); }
    QString money () const { return value<QString>(FieldGContactMoney); }

    void setRating (const QString& rating) { setValue(FieldGContactRating, rating); }
    QString rating () const { return value<QString>(FieldGContactRating); }

};

#endif // GCONTACTCUSTOMDETAIL_H
