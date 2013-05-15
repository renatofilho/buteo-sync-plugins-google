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

#include "GContactEntry.h"
#include "GContactCustomDetail.h"
#include <QDateTime>
#include <QDebug>
#include <LogMacros.h>

#include <QtContacts/QContact>
#include <QtContacts/QContactGuid>
#include <QtContacts/QContactName>
#include <QtContacts/QContactEmailAddress>
#include <QtContacts/QContactBirthday>
#include <QtContacts/QContactGender>
#include <QtContacts/QContactHobby>
#include <QtContacts/QContactNickname>
#include <QtContacts/QContactNote>
#include <QtContacts/QContactOnlineAccount>
#include <QtContacts/QContactOrganization>
#include <QtContacts/QContactPhoneNumber>
#include <QContactAddress>
#include <QContactSyncTarget>

const QString GDATA_SCHEMA ("http://schemas.google.com/g/2005");

GContactEntry::GContactEntry(bool generateXmlFlag) :
    mGenerateXmlFlag(generateXmlFlag)
{
}

void
GContactEntry::setId (QString id)
{
    QContactGuid contactGuid;
    contactGuid.setGuid (id);
    mQContact.saveDetail (&contactGuid);

    mId = id;
}

QString
GContactEntry::id ()
{
    return mId;
}

void
GContactEntry::setEtag (const QString etag)
{
    GContactCustomDetail etagDetail = mQContact.detail <GContactCustomDetail>();
    etagDetail.setETag (etag);

    mQContact.saveDetail (&etagDetail);
}

void
GContactEntry::setFullName (const QString fullName)
{
    // TODO: Custom tag
    // This has to go to QContactName as a schema change tag
}

void
GContactEntry::setAdditionalName (const QString additionalName)
{
    // TODO: Custom tag.
    // This has to go to QContactName as a schema change tag
}

void
GContactEntry::setFamilyName (const QString familyName)
{
    QContactName name = mQContact.detail <QContactName>();
    name.setLastName (familyName);
    mQContact.saveDetail (&name);
    LOG_DEBUG ("Family name set:" + name.lastName ());
}

void
GContactEntry::setNamePrefix (const QString namePrefix)
{
    QContactName name = mQContact.detail <QContactName>();
    name.setPrefix (namePrefix);
    mQContact.saveDetail (&name);
}

void
GContactEntry::setNameSuffix (const QString nameSuffix)
{
    QContactName name = mQContact.detail <QContactName>();
    name.setSuffix (nameSuffix);
    mQContact.saveDetail (&name);
}

void
GContactEntry::setGivenName (const QString givenName)
{
    QContactName name = mQContact.detail <QContactName>();
    name.setFirstName (givenName);
    mQContact.saveDetail (&name);
    LOG_DEBUG ("Given name set:" + name.firstName ());
}

void
GContactEntry::setEmail (const QString address, const QString rel,
                         const QString primary)
{
    QContactEmailAddress email;
    email.setEmailAddress (address);
    mQContact.saveDetail (&email);
    qDebug() << "Email set:" << email.emailAddress ();
}

void
GContactEntry::setBillingInformation (const QString billingInfo)
{
    GContactCustomDetail billing = mQContact.detail <GContactCustomDetail>();
    billing.setBillingInformation (billingInfo);

    mQContact.saveDetail (&billing);
}

void
GContactEntry::setBirthday (const QString birthday)
{
    qDebug() << birthday;
    QContactBirthday bday;

    bday.setDate (QDate::fromString (birthday, "yyyy-MM-dd"));
    qDebug() << mQContact.saveDetail (&bday);
}

void
GContactEntry::setCalendarLink (const QString href,
                                const QString rel, const QString primary)
{
    GContactCustomDetail calendarLink = mQContact.detail <GContactCustomDetail>();
    calendarLink.setCalendarLink (href);

    mQContact.saveDetail (&calendarLink);
}

void
GContactEntry::setDirectoryServer (const QString dirServer)
{
    GContactCustomDetail dirServerDetail = mQContact.detail <GContactCustomDetail>();
    dirServerDetail.setDirectoryServer (dirServer);

    mQContact.saveDetail (&dirServerDetail);
}

void
GContactEntry::setEvent (const QString event, const QString when)
{
    GContactCustomDetail eventDetail = mQContact.detail <GContactCustomDetail>();
    eventDetail.setEvent (event);

    mQContact.saveDetail (&eventDetail);
}

void
GContactEntry::setExternalId (const QString externalId,
                              const QString rel)
{
    GContactCustomDetail externalIdDetail = mQContact.detail <GContactCustomDetail>();
    externalIdDetail.setExternalId (externalId);

    mQContact.saveDetail (&externalIdDetail);
}

void
GContactEntry::setGender (const QString gender)
{
    QContactGender contactGender;
    contactGender.setGender (gender);
    mQContact.saveDetail (&contactGender);
}

void
GContactEntry::setGroupMembershipInfo (const QString membershipInfo,
                                       const QString deleted)
{
    GContactCustomDetail membershipDetail = mQContact.detail <GContactCustomDetail>();
    membershipDetail.setGroupMembershipInfo (membershipInfo);

    mQContact.saveDetail (&membershipDetail);
}

void
GContactEntry::setHobby (const QString hobby)
{
    QContactHobby contactHobby;
    contactHobby.setHobby (hobby);
    mQContact.saveDetail (&contactHobby);
}

void
GContactEntry::setInitials (const QString initials)
{
    GContactCustomDetail initialsDetail = mQContact.detail <GContactCustomDetail>();
    initialsDetail.setInitials (initials);

    mQContact.saveDetail (&initialsDetail);
}

void
GContactEntry::setJot (const QString jot, const QString jotType)
{
    GContactCustomDetail jotDetail = mQContact.detail <GContactCustomDetail>();
    jotDetail.setJot (jot);

    mQContact.saveDetail (&jotDetail);
}

void
GContactEntry::setLanguage (const QString language)
{
    GContactCustomDetail languageDetail = mQContact.detail <GContactCustomDetail>();
    languageDetail.setLanguage (language);

    mQContact.saveDetail (&languageDetail);
}

void
GContactEntry::setMaidenName (const QString maidenName)
{
    // TODO: Custom field
    // This should be part QContactName
}

void
GContactEntry::setMileage (const QString mileage)
{
    GContactCustomDetail mileageDetail = mQContact.detail <GContactCustomDetail>();
    mileageDetail.setMileage (mileage);

    mQContact.saveDetail (&mileageDetail);
}

void
GContactEntry::setNickname (const QString nickname)
{
    QContactNickname nick;
    nick.setNickname (nickname);
    mQContact.saveDetail (&nick);
}

void
GContactEntry::setOccupation (const QString occupation)
{
    GContactCustomDetail occupationDetail = mQContact.detail <GContactCustomDetail>();
    occupationDetail.setOccupation (occupation);

    mQContact.saveDetail (&occupationDetail);
}

void
GContactEntry::setPriority (const QString priority)
{
    GContactCustomDetail priorityDetail;
    priorityDetail.setPriority (priority);

    mQContact.saveDetail (&priorityDetail);
}

void
GContactEntry::setRelation (const QString relativeName,
                            const QString relation)
{
    // TODO: Custom fiel. Support available for children and spouse
    // Create a if() condition to handle this
}

void
GContactEntry::setSensitivity (const QString sensitivity)
{
    GContactCustomDetail sensitivityDetail = mQContact.detail <GContactCustomDetail>();
    sensitivityDetail.setSensitivity (sensitivity);

    mQContact.saveDetail (&sensitivityDetail);
}

void
GContactEntry::setShortname (const QString shortname)
{
    // TODO: Custom field
}

void
GContactEntry::setSubject (const QString subject)
{
    GContactCustomDetail subjectDetail = mQContact.detail <GContactCustomDetail>();
    subjectDetail.setSubject (subject);

    mQContact.saveDetail (&subjectDetail);
}

void
GContactEntry::setSystemGroup (const QString systemGroup)
{
    GContactCustomDetail systemGroupDetail = mQContact.detail <GContactCustomDetail>();
    systemGroupDetail.setSystemGroup (systemGroup);

    mQContact.saveDetail (&systemGroupDetail);
}

void
GContactEntry::setUserDefinedField (const QString key,
                                    const QString value)
{
    // TODO: Custom field
    // How should this be handled?
}

void
GContactEntry::setWebsite (const QString website, const QString rel,
                           const QString primary)
{
    GContactCustomDetail websiteDetail = mQContact.detail <GContactCustomDetail>();
    websiteDetail.setWebsite (website);

    mQContact.saveDetail (&websiteDetail);
}

// gd:xxx schema handler methods
void
GContactEntry::setComments (const QString comments)
{
    QContactNote note;
    note.setNote (comments);
    mQContact.saveDetail (&note);
}

void
GContactEntry::setCountry (const QString country)
{
    GContactCustomDetail countryDetail = mQContact.detail <GContactCustomDetail>();
    countryDetail.setCountry (country);

    mQContact.saveDetail (&countryDetail);
}

void
GContactEntry::setDeleted (const bool deleted)
{
    mDeleted = true;
}

bool
GContactEntry::deleted ()
{
    return mDeleted;
}

void
GContactEntry::setExtendedProperty (const QString name,
                                    const QString extProperty)
{
    // TODO: Custom field
    // How should this be handled?
}

void
GContactEntry::setFeedLink (const QString feedLink)
{
    GContactCustomDetail feedlinkDetail = mQContact.detail <GContactCustomDetail>();
    feedlinkDetail.setFeedLink (feedLink);

    mQContact.saveDetail (&feedlinkDetail);
}

void
GContactEntry::setIm (const QString address, const QString rel,
                      const QString protocol, const QString primary)
{
    QContactOnlineAccount imAccount;
    imAccount.setAccountUri (address);
    imAccount.setProtocol (protocol);
    imAccount.setServiceProvider (protocol);  // FIXME: How is protocol different from service provider?

    mQContact.saveDetail (&imAccount);
}

void
GContactEntry::setMoney (const QString amount, const QString currencyCode)
{
    GContactCustomDetail moneyDetail = mQContact.detail <GContactCustomDetail>();
    moneyDetail.setMoney (amount);

    mQContact.saveDetail (&moneyDetail);
}

void
GContactEntry::setOrgDepartment (const QString orgDept)
{
    QContactOrganization org = mQContact.detail <QContactOrganization>();
    QStringList deptList;
    deptList << orgDept;
    org.setDepartment (deptList);

    mQContact.saveDetail (&org);
}

void
GContactEntry::setOrgJobDescription (const QString orgJobDesc)
{
    QContactOrganization org = mQContact.detail <QContactOrganization>();
    org.setRole (orgJobDesc);

    mQContact.saveDetail (&org);
}

void
GContactEntry::setOrgName (const QString orgName)
{
    QContactOrganization org = mQContact.detail <QContactOrganization>();
    org.setName (orgName);

    mQContact.saveDetail (&org);
}

void
GContactEntry::setOrgSymbol (const QString orgSymbol)
{
    //TODO: Custom field
    // Should be part of Organization detail
}

void
GContactEntry::setOrgTitle (const QString orgTitle)
{
    QContactOrganization org = mQContact.detail <QContactOrganization>();
    org.setTitle (orgTitle);

    mQContact.saveDetail (&org);
}

void
GContactEntry::setOriginalEvent (const QString origEvent)
{
    //TODO: Custom field
}

void
GContactEntry::setPhoneNumber (const QString phoneNumber, const QString rel,
                               const QString uri, const QString primary=false)
{
    QContactPhoneNumber number;
    number.setNumber (phoneNumber);
    number.setSubTypes (rel);

    //TODO: Handle primary and uri fields
    mQContact.saveDetail (&number);
}

void
GContactEntry::setRating (const QString max, const QString min, const QString average,
                    const QString numRaters, const QString rel, const QString value)
{
    //TODO: Custom field
}

void
GContactEntry::setPostalAddrAgent (const QString agent)
{
    //TODO: Custom field
}

void
GContactEntry::setPostalAddrHousename (const QString housename)
{
    QContactAddress address = mQContact.detail <QContactAddress>();
    address.setLocality (housename);

    mQContact.saveDetail (&address);
}

void
GContactEntry::setPostalAddrStreet (const QString street)
{
    QContactAddress address = mQContact.detail <QContactAddress>();
    address.setStreet (street);

    mQContact.saveDetail (&address);
}

void
GContactEntry::setPostalAddrPobox (const QString pobox)
{
    QContactAddress address = mQContact.detail <QContactAddress>();
    address.setPostOfficeBox (pobox);

    mQContact.saveDetail (&address);
}

void
GContactEntry::setPostalAddrNeighborhood (const QString neighborhood)
{
    //TODO: Custom field
}

void
GContactEntry::setPostalAddrCity (const QString city)
{
    //TODO: Custom field
}

void
GContactEntry::setPostalAddrSubregion (const QString subregion)
{
    //TODO: Custom field
    // Belongs to QContactAddress
}

void
GContactEntry::setPostalAddrRegion (const QString region)
{
    QContactAddress address = mQContact.detail <QContactAddress>();
    address.setRegion (region);

    mQContact.saveDetail (&address);
}

void
GContactEntry::setPostalAddrPostCode (const QString postCode)
{
    QContactAddress address = mQContact.detail <QContactAddress>();
    address.setPostcode (postCode);

    mQContact.saveDetail (&address);
}

void
GContactEntry::setPostalAddrCountry (const QString country)
{
    QContactAddress address = mQContact.detail <QContactAddress>();
    address.setCountry (country);

    mQContact.saveDetail (&address);
}

void
GContactEntry::setPostalAddrFormatted (const QString formattedAddr)
{
    //TODO: Custom field
}

void
GContactEntry::setPostalAddrAttrs(const QString rel, const QString mailClass,
                                  const QString usage, const QString primary)
{
}

void
GContactEntry::setSyncTarget ()
{
    QContactSyncTarget target;
    target.setSyncTarget ("buteo-google-contacts");

    mQContact.saveDetail (&target);
}

QContact
GContactEntry::qContact()
{
    return mQContact;
}
