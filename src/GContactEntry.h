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

#ifndef GCONTACT_H
#define GCONTACT_H

#include <QObject>
#include <QContactManager>

QTM_USE_NAMESPACE

class GContactEntry
{
public:

    explicit GContactEntry (bool generateXmlFlag);

    void setId (const QString id);
    QString id ();

    void setEtag (const QString etag);
    QString etag ();

    void setFullName (const QString fullName);
    QString getFullName ();

    void setAdditionalName (const QString additionalName);
    QString getAdditionalName ();

    void setFamilyName (const QString familyName);
    QString getFamilyName ();

    void setNamePrefix (const QString namePrefix);
    QString getNamePrefix ();

    void setNameSuffix (const QString nameSuffix);
    QString getNameSuffix ();

    void setGivenName (const QString givenName);
    QString getGivenName ();

    void setEmail (const QString address, const QString rel, const QString primary);
    QString email ();

    void setBillingInformation (const QString billingInfo);
    QString billingInformation ();

    void setBirthday (const QString birthday);
    QString birthday ();

    void setCalendarLink (const QString href, const QString rel, const QString primary);
    QString calendarLink ();

    void setDirectoryServer (const QString dirServer);
    QString directoryServer ();

    void setEvent (const QString event, const QString when);
    QString event ();

    void setExternalId (const QString externalId, const QString rel);
    QString externalId ();

    void setGender (const QString gender);
    QString gender ();

    void setGroupMembershipInfo (const QString membershipInfo, const QString deleted);
    QString groupMembershipInfo ();

    void setHobby (const QString hobby);
    QString hobby ();

    void setInitials (const QString initials);
    QString initials ();

    void setJot (const QString jot, const QString jotType);
    QString jot ();

    void setLanguage (const QString language);
    QString language ();

    void setMaidenName (const QString maidenName);
    QString maidenName ();

    void setMileage (const QString mileage);
    QString mileage ();

    void setNickname (const QString nickname);
    QString nickname ();

    void setOccupation (const QString occupation);
    QString occupation ();

    void setPriority (const QString priority);
    QString priority ();

    void setRelation (const QString relativeName, const QString relation);
    QString relation ();

    void setSensitivity (const QString sensitivity);
    QString sensitivity ();

    void setShortname (const QString shortname);
    QString shortname ();

    void setSubject (const QString subject);
    QString subject ();

    void setSystemGroup (const QString systemGroup);
    QString systemGroup ();

    void setUserDefinedField (const QString key, const QString value);
    QString userDefinedField ();

    void setWebsite (const QString website, const QString rel, const QString primary="false");
    QString website ();

    // gd:xxx schema handler methods
    void setComments (const QString comments);
    QString comments ();

    void setCountry (const QString country);
    QString country ();

    void setDeleted (const bool deleted);
    bool deleted ();

    void setExtendedProperty (const QString name, const QString extProperty);
    QString extendedProperty ();

    void setFeedLink (const QString feedLink);
    QString feedLink ();

    void setIm (const QString address, const QString rel, const QString protocol, QString primary);
    QString im ();

    void setMoney (const QString amount, const QString currencyCode);
    QString money ();

    void setOrgDepartment (const QString orgDept);
    QString orgDepartment ();

    void setOrgJobDescription (const QString orgJobDesc);
    QString orgJobDescription ();

    void setOrgName (const QString orgName);
    QString orgName ();

    void setOrgSymbol (const QString orgSymbol);
    QString orgSymbol ();

    void setOrgTitle (const QString orgTitle);
    QString orgTitle ();

    void setOriginalEvent (const QString origEvent);
    QString originalEvent ();

    void setPhoneNumber (const QString phoneNumber, const QString rel, const QString uri, const QString primary);
    QString phoneNumber ();

    void setRating (const QString max, const QString min, const QString average,
                    const QString numRaters, const QString rel, const QString value);
    QString rating ();

    void setPostalAddrAgent (const QString agent);
    QString postalAddrAgent ();

    void setPostalAddrHousename (const QString housename);
    QString postalAddrHousename ();

    void setPostalAddrStreet (const QString street);
    QString postalAddrStreet ();

    void setPostalAddrPobox (const QString pobox);
    QString postalAddrPobox ();

    void setPostalAddrNeighborhood (const QString neighborhood);
    QString postalAddrNeighborhood ();

    void setPostalAddrCity (const QString city);
    QString postalAddrCity ();

    void setPostalAddrSubregion (const QString subregion);
    QString postalAddrSubregion ();

    void setPostalAddrRegion (const QString region);
    QString postalAddrRegion ();

    void setPostalAddrPostCode (const QString postCode);
    QString postalAddrPostCode ();

    void setPostalAddrCountry (const QString country);
    QString postalAddrCountry ();

    void setPostalAddrFormatted (const QString formattedAddr);
    QString postalAddrFormatted ();

    void setPostalAddrAttrs(const QString rel, const QString mailClass, const QString usage, const QString primary);

    void setSyncTarget ();

    QContact qContact ();

private:

    bool mGenerateXmlFlag;

    QString mId;

    QContact mQContact;

    bool mDeleted;
};

#endif // GCONTACT_H
