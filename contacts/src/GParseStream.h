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

class GAtom;
class GContactEntry;

class GParseStream : public QObject
{
    Q_OBJECT
public:
    explicit GParseStream (bool response, QObject* parent = 0);

    explicit GParseStream (QByteArray xmlStream, QObject *parent = 0);

    ~GParseStream ();

    GAtom* parse(const QByteArray xmlBuffer);

signals:

    void parseDone (bool);

public slots:

private:

    void initAtomFunctionMap ();

    void initResponseFunctionMap ();

    void initFunctionMap();

    // Atom feed elements handler methods
    void handleAtomUpdated ();
    void handleAtomCategory ();
    void handleAtomAuthor ();
    void handleAtomOpenSearch ();
    void handleAtomEntry ();
    void handleAtomLink ();

    // Following are for the response received from the server
    // incase of failures
    void handleEntryBatchStatus ();
    void handleEntryBatchOperation ();
    void handleEntryBatchId();

    // gContact:xxx schema handler methods
    void handleEntryContent();
    void handleEntryId();
    void handleEntryTitle();
    void handleEntryLink ();
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
    void handleEntryAdditionalName();
    void handleEntryComments();
    void handleEntryDeleted();
    void handleEntryCountry();
    void handleEntryEmail();
    void handleEntryExtendedProperty();
    void handleEntryFeedLink();
    void handleEntryIm();
    void handleEntryMoney();
    void handleEntryName();
    void handleEntryOrganization();
    void handleEntryOriginalEvent();
    void handleEntryPhoneNumber();
    void handleEntryRating();
    void handleEntryStructuredPostalAddress();

    QXmlStreamReader* mXml;

    typedef void (GParseStream::*Handler)();

    QMap<QString, GParseStream::Handler> mAtomFunctionMap;

    QMap<QString, GParseStream::Handler> mContactFunctionMap;

    GAtom* mAtom;

    GContactEntry* mContactEntry;
};

#endif // PARSESTREAM_H
