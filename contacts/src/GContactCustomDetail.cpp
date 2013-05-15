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

#include "GContactCustomDetail.h"

Q_IMPLEMENT_CUSTOM_CONTACT_DETAIL(GContactCustomDetail, "GoogleContact");

Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactETag, "GContactETag");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactBillingInformation, "GContactBillingInformation");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactCalendarLink, "GContactCalendarLink");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactDirectoryServer, "GContactDirectoryServer");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactEvent, "GContactEvent");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactExternalId, "GContactExternalId");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactGroupMembershipInfo, "GContactGroupMembershipInfo");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactInitials, "GContactInitials");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactJot, "GContactJot");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactLanguage, "GContactLanguate");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactMileage, "GContactMileage");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactOccupation, "GContactOccupation");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactPriority, "GContactPriority");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactRelation, "GContactRelation");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactSensitivity, "GContactSensitivity");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactSubject, "GContactSubject");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactSystemGroup, "GContactSystemGroup");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactWebsite, "GContactWebsite");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactCountry, "GContactCountry");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactFeedLink, "GContactFeedLink");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactMoney, "GContactMoney");
Q_DEFINE_LATIN1_CONSTANT(GContactCustomDetail::FieldGContactRating, "GContactRating");
