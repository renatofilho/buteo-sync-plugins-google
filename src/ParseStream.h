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

#ifndef PARSESTREAM_H
#define PARSESTREAM_H

#include <QObject>

#include <QXmlStreamReader>
#include <QMap>

#include "Atom.h"

class ParseStream : public QObject
{
    Q_OBJECT
public:
    explicit ParseStream(QObject *parent = 0);

    explicit ParseStream(QByteArray xmlStream);

    void parse();

    Atom* atom();

signals:

public slots:

private:

    void initFunctionMap();

    // Atom feed elements handler methods
    void handleAtomUpdated();
    void handleAtomCategory();
    void handleAtomAuthor();
    void handleAtomOpenSearch();
    void handleAtomEntry();

    // gContact:xxx schema handler methods
    void handleEntryId();
    void handleEntryBillingInformation();
    void handleEntryBirthday();
    void handleEntryCalendarLink();
    void handleEntryDirectoryServer();
    void handleEntryEvent();
    void handleEntryExternalId();
    void handleEntryGender();
    void handleEntryGroupMembershipInfo();
    void handleEntryHobby();
    void handleEntryInitials();
    void handleEntryJot();
    void handleEntryLanguage();
    void handleEntryMaidenName();
    void handleEntryMileage();
    void handleEntryNickname();
    void handleEntryOccupation();
    void handleEntryPriority();
    void handleEntryRelation();
    void handleEntrySensitivity();
    void handleEntryShortname();
    void handleEntrySubject();
    void handleEntrySystemGroup();
    void handleEntryUserDefinedField();
    void handleEntryWebsite();

    // gd:xxx schema handler methods
    void handleEntryName();
    void handleEntryOrganization();
    void handleEntryPhoneNumber();
    void handleEntryAdditionalName();
    void handleEntryComments();
    void handleEntryCountry();
    void handleEntryDeleted();
    void handleEntryEmail();
    void handleEntryEntryLink();
    void handleEntryExtendedProperty();
    void handleEntryFamilyName();
    void handleEntryFeedLink();
    void handleEntryGeoPt();
    void handleEntryGivenName();
    void handleEntryIm();
    void handleEntryMoney();
    void handleEntryOriginalEvent();
    void handleEntryRating();
    void handleEntryResourceId();
    void handleEntryStructuredPostalAddress();

    QXmlStreamReader* mXml;

    typedef void (ParseStream::*Handler)();

    QMap<QString, ParseStream::Handler> mAtomFunctionMap;

    QMap<QString, ParseStream::Handler> mContactFunctionMap;

    Atom* mAtom;

    GContactEntry* mContactEntry;
};

#endif // PARSESTREAM_H
