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

    void setFullName (const QString fullName);

    void setAdditionalName (const QString additionalName);

    void setFamilyName (const QString familyName);

    void setNamePrefix (const QString namePrefix);

    void setNameSuffix (const QString nameSuffix);

    void setGivenName (const QString givenName);

    void setEmail (const QString address, const QString rel, const QString primary);

    void setBillingInformation (const QString billingInfo);

    void setBirthday (const QString birthday);

    void setCalendarLink (const QString href, const QString rel, const QString primary);

    void setDirectoryServer (const QString dirServer);

    void setEvent (const QString event, const QString when);

    void setExternalId (const QString externalId, const QString rel);

    void setGender (const QString gender);

    void setGroupMembershipInfo (const QString membershipInfo, const QString deleted);

    void setHobby (const QString hobby);

    void setInitials (const QString initials);

    void setJot (const QString jot, const QString jotType);

    void setLanguage (const QString language);

    void setMaidenName (const QString maidenName);

    void setMileage (const QString mileage);

    void setNickname (const QString nickname);

    void setOccupation (const QString occupation);

    void setPriority (const QString priority);

    void setRelation (const QString relativeName, const QString relation);

    void setSensitivity (const QString sensitivity);

    void setShortname (const QString shortname);

    void setSubject (const QString subject);

    void setSystemGroup (const QString systemGroup);

    void setUserDefinedField (const QString key, const QString value);

    void setWebsite (const QString website, const QString rel, const QString primary="false");

    // gd:xxx schema handler methods
    void setComments (const QString comments);

    void setCountry (const QString country);

    void setDeleted (const bool deleted);

    bool deleted ();

    void setExtendedProperty (const QString name, const QString extProperty);

    void setFeedLink (const QString feedLink);

    void setIm (const QString address, const QString rel, const QString protocol, QString primary);

    void setMoney (const QString amount, const QString currencyCode);

    void setOrgDepartment (const QString orgDept);

    void setOrgJobDescription (const QString orgJobDesc);

    void setOrgName (const QString orgName);

    void setOrgSymbol (const QString orgSymbol);

    void setOrgTitle (const QString orgTitle);

    void setOriginalEvent (const QString origEvent);

    void setPhoneNumber (const QString phoneNumber, const QString rel, const QString uri, const QString primary);

    void setRating (const QString max, const QString min, const QString average,
                    const QString numRaters, const QString rel, const QString value);

    void setPostalAddrAgent (const QString agent);

    void setPostalAddrHousename (const QString housename);

    void setPostalAddrStreet (const QString street);

    void setPostalAddrPobox (const QString pobox);

    void setPostalAddrNeighborhood (const QString neighborhood);

    void setPostalAddrCity (const QString city);

    void setPostalAddrSubregion (const QString subregion);

    void setPostalAddrRegion (const QString region);

    void setPostalAddrPostCode (const QString postCode);

    void setPostalAddrCountry (const QString country);

    void setPostalAddrFormatted (const QString formattedAddr);

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
