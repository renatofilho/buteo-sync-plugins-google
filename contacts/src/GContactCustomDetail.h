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

QTCONTACTS_USE_NAMESPACE

class GContactCustomDetail
{
public:


    static const QString FieldGContactETag;
    static const QString FieldGContactBillingInformation;
    static const QString FieldGContactCalendarLink;
    static const QString FieldGContactDirectoryServer;
    static const QString FieldGContactEvent;
    static const QString FieldGContactExternalId;
    static const QString FieldGContactGroupMembershipInfo;
    static const QString FieldGContactInitials;
    static const QString FieldGContactJot;
    static const QString FieldGContactLanguage;
    static const QString FieldGContactMileage;
    static const QString FieldGContactOccupation;
    static const QString FieldGContactPriority;
    static const QString FieldGContactRelation;
    static const QString FieldGContactSensitivity;
    static const QString FieldGContactSubject;
    static const QString FieldGContactSystemGroup;
    static const QString FieldGContactCountry;
    static const QString FieldGContactFeedLink;
    static const QString FieldGContactMoney;
    static const QString FieldGContactRating;
};

#endif // GCONTACTCUSTOMDETAIL_H
