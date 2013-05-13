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
    GContactCustomDetail etagDetail;
    etagDetail.setETag (etag);

    mQContact.saveDetail (&etagDetail);
}

QString
GContactEntry::etag ()
{
}

void
GContactEntry::setFullName (const QString fullName)
{
}

QString
GContactEntry::getFullName ()
{
    // TODO: Custom tag. Is it necessary to handle this, since
    // name is already part of gd:name
    // This has to go to QContactName as a schema change tag
}

void
GContactEntry::setAdditionalName (const QString additionalName)
{
    // TODO: Custom tag.
    // This has to go to QContactName as a schema change tag
}

QString
GContactEntry::getAdditionalName ()
{
}

void
GContactEntry::setFamilyName (const QString familyName)
{
    QContactName name;
    name.setLastName (familyName);
    qDebug() << mQContact.saveDetail (&name);
    qDebug() << "Family name set:" << name.lastName ();
}

QString
GContactEntry::getFamilyName ()
{
}

void
GContactEntry::setNamePrefix (const QString namePrefix)
{
    QContactName name;
    name.setPrefix (namePrefix);
    mQContact.saveDetail (&name);
}

QString
GContactEntry::getNamePrefix ()
{
}

void
GContactEntry::setNameSuffix (const QString nameSuffix)
{
    QContactName name;
    name.setSuffix (nameSuffix);
    mQContact.saveDetail (&name);
}

QString
GContactEntry::getNameSuffix ()
{
}

void
GContactEntry::setGivenName (const QString givenName)
{
    QContactName name;
    name.setFirstName (givenName);
    mQContact.saveDetail (&name);
    qDebug() << "Given name set:" << name.firstName ();
}

QString
GContactEntry::getGivenName ()
{
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

QString
GContactEntry::email ()
{
}

void
GContactEntry::setBillingInformation (const QString billingInfo)
{
    GContactCustomDetail billing;
    billing.setBillingInformation (billingInfo);

    mQContact.saveDetail (&billing);
}

QString
billingInformation ()
{
}

void
GContactEntry::setBirthday (const QString birthday)
{
    qDebug() << birthday;
    QContactBirthday bday;

    bday.setDate (QDate::fromString (birthday, "yyyy-dd-mm"));
    qDebug() << mQContact.saveDetail (&bday);
    qDebug() << bday.date ().toString() << "DT";
}


/*
void
GContactEntry::setBirthday (QString birthday)
{
    qDebug() << birthday;
    QContactBirthday bday;

    QDate bdayDate = QDate::fromString( birthday, "yyyy-mm-dd");
    qDebug() << bdayDate.toString() << "D"; // does this show the correct date?
    bday.setDate (bdayDate);
    mQContact.saveDetail (&bday);
    qDebug() << bday.date().toString() << "DT"; // does it still match here?
    qDebug() << mQContact.detail<QContactBirthday>().date() << "CDT"; // does it match here, or not?
}
*/

QString
GContactEntry::birthday ()
{
}

void
GContactEntry::setCalendarLink (const QString href,
                                const QString rel, const QString primary)
{
    GContactCustomDetail calendarLink;
    calendarLink.setCalendarLink (href);

    mQContact.saveDetail (&calendarLink);
}

QString
GContactEntry::calendarLink ()
{
}

void
GContactEntry::setDirectoryServer (const QString dirServer)
{
    GContactCustomDetail dirServerDetail;
    dirServerDetail.setDirectoryServer (dirServer);

    mQContact.saveDetail (&dirServerDetail);
}

QString
GContactEntry::directoryServer ()
{
}

void
GContactEntry::setEvent (const QString event, const QString when)
{
    GContactCustomDetail eventDetail;
    eventDetail.setEvent (event);

    mQContact.saveDetail (&eventDetail);
}

QString
GContactEntry::event ()
{
}

void
GContactEntry::setExternalId (const QString externalId,
                              const QString rel)
{
    GContactCustomDetail externalIdDetail;
    externalIdDetail.setExternalId (externalId);

    mQContact.saveDetail (&externalIdDetail);
}

QString
GContactEntry::externalId ()
{
}

void
GContactEntry::setGender (const QString gender)
{
    QContactGender contactGender;
    contactGender.setGender (gender);
    mQContact.saveDetail (&contactGender);
}

QString
GContactEntry::gender ()
{
}

void
GContactEntry::setGroupMembershipInfo (const QString membershipInfo,
                                       const QString deleted)
{
    GContactCustomDetail membershipDetail;
    membershipDetail.setGroupMembershipInfo (membershipInfo);

    mQContact.saveDetail (&membershipDetail);
}

QString
GContactEntry::groupMembershipInfo ()
{
}

void
GContactEntry::setHobby (const QString hobby)
{
    QContactHobby contactHobby;
    contactHobby.setHobby (hobby);
    mQContact.saveDetail (&contactHobby);
}

QString
GContactEntry::hobby ()
{
}

void
GContactEntry::setInitials (const QString initials)
{
    GContactCustomDetail initialsDetail;
    initialsDetail.setInitials (initials);

    mQContact.saveDetail (&initialsDetail);
}

QString
GContactEntry::initials ()
{
}

void
GContactEntry::setJot (const QString jot, const QString jotType)
{
    GContactCustomDetail jotDetail;
    jotDetail.setJot (jot);

    mQContact.saveDetail (&jotDetail);
}

QString
GContactEntry::jot ()
{
}

void
GContactEntry::setLanguage (const QString language)
{
    GContactCustomDetail languageDetail;
    languageDetail.setLanguage (language);

    mQContact.saveDetail (&languageDetail);
}

QString
GContactEntry::language ()
{
}

void
GContactEntry::setMaidenName (const QString maidenName)
{
    // TODO: Custom field
    // This should be part QContactName
}

QString
GContactEntry::maidenName ()
{
}

void
GContactEntry::setMileage (const QString mileage)
{
    GContactCustomDetail mileageDetail;
    mileageDetail.setMileage (mileage);

    mQContact.saveDetail (&mileageDetail);
}

QString
GContactEntry::mileage ()
{
}

void
GContactEntry::setNickname (const QString nickname)
{
    QContactNickname nick;
    nick.setNickname (nickname);
    mQContact.saveDetail (&nick);
}

QString
GContactEntry::nickname ()
{
}

void
GContactEntry::setOccupation (const QString occupation)
{
    GContactCustomDetail occupationDetail;
    occupationDetail.setOccupation (occupation);

    mQContact.saveDetail (&occupationDetail);
}

QString
GContactEntry::occupation ()
{
}

void
GContactEntry::setPriority (const QString priority)
{
    GContactCustomDetail priorityDetail;
    priorityDetail.setPriority (priority);

    mQContact.saveDetail (&priorityDetail);
}

QString
GContactEntry::priority ()
{
}

void
GContactEntry::setRelation (const QString relativeName,
                            const QString relation)
{
    // TODO: Custom fiel. Support available for children and spouse
    // Create a if() condition to handle this
}

QString
GContactEntry::relation ()
{
}

void
GContactEntry::setSensitivity (const QString sensitivity)
{
    GContactCustomDetail sensitivityDetail;
    sensitivityDetail.setSensitivity (sensitivity);

    mQContact.saveDetail (&sensitivityDetail);
}

QString
GContactEntry::sensitivity ()
{
}

void
GContactEntry::setShortname (const QString shortname)
{
    // TODO: Custom field
}

QString
GContactEntry::shortname ()
{
}

void
GContactEntry::setSubject (const QString subject)
{
    GContactCustomDetail subjectDetail;
    subjectDetail.setSubject (subject);

    mQContact.saveDetail (&subjectDetail);
}

QString
GContactEntry::subject ()
{
}

void
GContactEntry::setSystemGroup (const QString systemGroup)
{
    GContactCustomDetail systemGroupDetail;
    systemGroupDetail.setSystemGroup (systemGroup);

    mQContact.saveDetail (&systemGroupDetail);
}

QString
GContactEntry::systemGroup ()
{
}

void
GContactEntry::setUserDefinedField (const QString key,
                                    const QString value)
{
    // TODO: Custom field
    // How should this be handled?
}

QString
GContactEntry::userDefinedField ()
{
}

void
GContactEntry::setWebsite (const QString website, const QString rel,
                           const QString primary)
{
    GContactCustomDetail websiteDetail;
    websiteDetail.setWebsite (website);

    mQContact.saveDetail (&websiteDetail);
}

QString
GContactEntry::website ()
{
}

// gd:xxx schema handler methods
void
GContactEntry::setComments (const QString comments)
{
    QContactNote note;
    note.setNote (comments);
    mQContact.saveDetail (&note);
}

QString
GContactEntry::comments ()
{
}

void
GContactEntry::setCountry (const QString country)
{
    GContactCustomDetail countryDetail;
    countryDetail.setCountry (country);

    mQContact.saveDetail (&countryDetail);
}

QString
GContactEntry::country ()
{
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

QString
GContactEntry::extendedProperty ()
{
}

void
GContactEntry::setFeedLink (const QString feedLink)
{
    GContactCustomDetail feedlinkDetail;
    feedlinkDetail.setFeedLink (feedLink);

    mQContact.saveDetail (&feedlinkDetail);
}

QString
GContactEntry::feedLink ()
{
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

QString
GContactEntry::im ()
{
}

void
GContactEntry::setMoney (const QString amount, const QString currencyCode)
{
    GContactCustomDetail moneyDetail;
    moneyDetail.setMoney (amount);

    mQContact.saveDetail (&moneyDetail);
}

QString
GContactEntry::money ()
{
    //TODO: Custom field
}

void
GContactEntry::setOrgDepartment (const QString orgDept)
{
    QContactOrganization org;
    QStringList deptList;
    deptList << orgDept;
    org.setDepartment (deptList);

    mQContact.saveDetail (&org);
}

QString
GContactEntry::orgDepartment ()
{
}

void
GContactEntry::setOrgJobDescription (const QString orgJobDesc)
{
    QContactOrganization org;
    org.setRole (orgJobDesc);

    mQContact.saveDetail (&org);
}

QString
GContactEntry::orgJobDescription ()
{
}

void
GContactEntry::setOrgName (const QString orgName)
{
    QContactOrganization org;
    org.setName (orgName);

    mQContact.saveDetail (&org);
}

QString
GContactEntry::orgName ()
{
}

void
GContactEntry::setOrgSymbol (const QString orgSymbol)
{
    //TODO: Custom field
    // Should be part of Organization detail
}

QString
GContactEntry::orgSymbol ()
{
}

void
GContactEntry::setOrgTitle (const QString orgTitle)
{
    QContactOrganization org;
    org.setTitle (orgTitle);

    mQContact.saveDetail (&org);
}

QString
GContactEntry::orgTitle ()
{
}

void
GContactEntry::setOriginalEvent (const QString origEvent)
{
    //TODO: Custom field
}

QString
GContactEntry::originalEvent ()
{
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

QString
GContactEntry::phoneNumber ()
{
}

void
GContactEntry::setRating (const QString max, const QString min, const QString average,
                    const QString numRaters, const QString rel, const QString value)
{
    //TODO: Custom field
}

QString
GContactEntry::rating ()
{
}

void
GContactEntry::setPostalAddrAgent (const QString agent)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrAgent ()
{
    //TODO: Custom field
}

void
GContactEntry::setPostalAddrHousename (const QString housename)
{
    QContactAddress address;
    address.setLocality (housename);

    mQContact.saveDetail (&address);
}

QString
GContactEntry::postalAddrHousename ()
{
}

void
GContactEntry::setPostalAddrStreet (const QString street)
{
    QContactAddress address;
    address.setStreet (street);

    mQContact.saveDetail (&address);
}

QString
GContactEntry::postalAddrStreet ()
{
}

void
GContactEntry::setPostalAddrPobox (const QString pobox)
{
    QContactAddress address;
    address.setPostOfficeBox (pobox);

    mQContact.saveDetail (&address);
}

QString
GContactEntry::postalAddrPobox ()
{
}

void
GContactEntry::setPostalAddrNeighborhood (const QString neighborhood)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrNeighborhood ()
{
}

void
GContactEntry::setPostalAddrCity (const QString city)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrCity ()
{
}

void
GContactEntry::setPostalAddrSubregion (const QString subregion)
{
    //TODO: Custom field
    // Belongs to QContactAddress
}

QString
GContactEntry::postalAddrSubregion ()
{
}

void
GContactEntry::setPostalAddrRegion (const QString region)
{
    QContactAddress address;
    address.setRegion (region);

    mQContact.saveDetail (&address);
}

QString
GContactEntry::postalAddrRegion ()
{
}

void
GContactEntry::setPostalAddrPostCode (const QString postCode)
{
    QContactAddress address;
    address.setPostcode (postCode);

    mQContact.saveDetail (&address);
}

QString
GContactEntry::postalAddrPostCode ()
{
}

void
GContactEntry::setPostalAddrCountry (const QString country)
{
    QContactAddress address;
    address.setCountry (country);

    mQContact.saveDetail (&address);
}

QString
GContactEntry::postalAddrCountry ()
{
}

void
GContactEntry::setPostalAddrFormatted (const QString formattedAddr)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrFormatted ()
{
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
