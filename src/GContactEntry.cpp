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
#include <QDateTime>

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

const QString GDATA_SCHEMA ("http://schemas.google.com/g/2005");

GContactEntry::GContactEntry(bool generateXmlFlag, QObject *parent) :
    mGenerateXmlFlag(generateXmlFlag), QObject(parent)
{
    mContact = new QContact ();
}

void
GContactEntry::setId (QString id)
{
    QContactGuid contactGuid;
    contactGuid.setGuid (id);
    mContact->saveDetail (&contactGuid);
}

void
GContactEntry::setEtag (QString etag)
{
    // TODO: Custom detail
}

QString
GContactEntry::etag ()
{
}

void
GContactEntry::setFullName (QString fullName)
{
   // TODO: Custom tag
}

QString
GContactEntry::getFullName ()
{
    // TODO: Custom tag. Is it necessary to handle this, since
    // name is already part of gd:name
}

void
GContactEntry::setAdditionalName (QString additionalName)
{
    // TODO: Custom tag.
}

QString
GContactEntry::getAdditionalName ()
{
}

void
GContactEntry::setFamilyName (QString familyName)
{
    QContactName name;
    name.setLastName (familyName);
    mContact->saveDetail (&name);
}

QString
GContactEntry::getFamilyName ()
{
}

void
GContactEntry::setNamePrefix (QString namePrefix)
{
    QContactName name;
    name.setPrefix (namePrefix);
    mContact->saveDetail (&name);
}

QString
GContactEntry::getNamePrefix ()
{
}

void
GContactEntry::setNameSuffix (QString nameSuffix)
{
    QContactName name;
    name.setSuffix (nameSuffix);
    mContact->saveDetail (&name);
}

QString
GContactEntry::getNameSuffix ()
{
}

void
GContactEntry::setGivenName (QString givenName)
{
    QContactName name;
    name.setFirstName (givenName);
    mContact->saveDetail (&name);
}

QString
GContactEntry::getGivenName ()
{
}

void
GContactEntry::setEmail (QString address, QString label, QString rel, bool primary)
{
    QContactEmailAddress email;
    email.setEmailAddress (address);
    mContact->saveDetail (&email);
}

QString
GContactEntry::email ()
{
}

void
GContactEntry::setBillingInformation (QString billingInfo)
{
    // TODO: Custom field
}

QString
billingInformation ()
{
}

void
GContactEntry::setBirthday (QString birthday)
{
    QContactBirthday bday;

    bday.setDate (QDate::fromString (birthday, "yyyy-mm-dd"));
    mContact->saveDetail (&bday);
}

QString
GContactEntry::birthday ()
{
}

void
GContactEntry::setCalendarLink (QString href, QString label, QString rel, QString primary)
{
    // TODO: custom field
}

QString
GContactEntry::calendarLink ()
{
}

void
GContactEntry::setDirectoryServer (QString dirServer)
{
    // TODO: Custom field
}

QString
GContactEntry::directoryServer ()
{
}

void
GContactEntry::setEvent (QString event, QString when)
{
    // TODO: Custom field
}

QString
GContactEntry::event ()
{
}

void
GContactEntry::setExternalId (QString externalId, QString rel)
{
    // TODO: Custom field
}

QString
GContactEntry::externalId ()
{
}

void
GContactEntry::setGender (QString gender)
{
    QContactGender contactGender;
    contactGender.setGender (gender);
    mContact->saveDetail (&contactGender);
}

QString
GContactEntry::gender ()
{
}

void
GContactEntry::setGroupMembershipInfo (QString membershipInfo, QString deleted)
{
    // TODO: Handle groups
}

QString
GContactEntry::groupMembershipInfo ()
{
}

void
GContactEntry::setHobby (QString hobby)
{
    QContactHobby contactHobby;
    contactHobby.setHobby (hobby);
    mContact->saveDetail (&contactHobby);
}

QString
GContactEntry::hobby ()
{
}

void
GContactEntry::setInitials (QString initials)
{
    // TODO: Custom field
}

QString
GContactEntry::initials ()
{
}

void
GContactEntry::setJot (QString jot, QString jotType)
{
    // TODO: Custom field
}

QString
GContactEntry::jot ()
{
}

void
GContactEntry::setLanguage (QString language)
{
    // TODO: Custom field
}

QString
GContactEntry::language ()
{
}

void
GContactEntry::setMaidenName (QString maidenName)
{
    // TODO: Custom field
}

QString
GContactEntry::maidenName ()
{
}

void
GContactEntry::setMileage (QString mileage)
{
    // TODO: Custom field
}

QString
GContactEntry::mileage ()
{
}

void
GContactEntry::setNickname (QString nickname)
{
    QContactNickname nick;
    nick.setNickname (nickname);
    mContact->saveDetail (&nick);
}

QString
GContactEntry::nickname ()
{
}

void
GContactEntry::setOccupation (QString occupation)
{
    // TODO: Custom field
}

QString
GContactEntry::occupation ()
{
}

void
GContactEntry::setPriority (QString priority)
{
    // TODO: Custom field
}

QString
GContactEntry::priority ()
{
}

void
GContactEntry::setRelation (QString relativeName, QString relation)
{
    // TODO: Custom fiel. Support available for children and spouse
    // Create a if() condition to handle this
}

QString
GContactEntry::relation ()
{
}

void
GContactEntry::setSensitivity (QString sensitivity)
{
    // TODO: Custom field
}

QString
GContactEntry::sensitivity ()
{
}

void
GContactEntry::setShortname (QString shortname)
{
    // TODO: Custom field
}

QString
GContactEntry::shortname ()
{
}

void
GContactEntry::setSubject (QString subject)
{
    // TODO: Custom field
}

QString
GContactEntry::subject ()
{
}

void
GContactEntry::setSystemGroup (QString systemGroup)
{
    // TODO: Custom field
}

QString
GContactEntry::systemGroup ()
{
}

void
GContactEntry::setUserDefinedField (QString key, QString value)
{
    // TODO: Custom field
}

QString
GContactEntry::userDefinedField ()
{
}

void
GContactEntry::setWebsite (QString website, QString rel, QString primary)
{
    // TODO: Custom field
}

QString
GContactEntry::website ()
{
}

// gd:xxx schema handler methods
void
GContactEntry::setComments (QString comments)
{
    QContactNote note;
    note.setNote (comments);
    mContact->saveDetail (&note);
}

QString
GContactEntry::comments ()
{
}

void
GContactEntry::setCountry (QString country)
{
    // TODO: Custom field
}

QString
GContactEntry::country ()
{
}

void
GContactEntry::setDeleted (bool deleted)
{
    // TODO: Handle deleted entry
}

bool
GContactEntry::deleted ()
{
}

void
GContactEntry::setEntryLink (QString link)
{
    // TODO: Custom field
}

QString
GContactEntry::entryLink ()
{
}

void
GContactEntry::setExtendedProperty (QString name, QString extProperty)
{
    // TODO: Custom field
}

QString
GContactEntry::extendedProperty ()
{
}

void
GContactEntry::setFeedLink (QString feedLink)
{
    // TODO: Custom field
}

QString
GContactEntry::feedLink ()
{
}

void
GContactEntry::setIm (QString address, QString rel, QString protocol, QString primary)
{
    QContactOnlineAccount imAccount;
    imAccount.setAccountUri (address);
    imAccount.setProtocol (protocol);
    imAccount.setServiceProvider (protocol);  // FIXME: How is protocol different from service provider?

    mContact->saveDetail (&imAccount);
}

QString
GContactEntry::im ()
{
}

void
GContactEntry::setMoney (QString amount, QString currencyCode)
{
}

QString
GContactEntry::money ()
{
    //TODO: Custom field
}

void
GContactEntry::setOrgDepartment (QString orgDept)
{
    QContactOrganization org;
    QStringList deptList;
    deptList << orgDept;
    org.setDepartment (deptList);

    mContact->saveDetail (&org);
}

QString
GContactEntry::orgDepartment ()
{
}

void
GContactEntry::setOrgJobDescription (QString orgJobDesc)
{
    QContactOrganization org;
    org.setRole (orgJobDesc);

    mContact->saveDetail (&org);
}

QString
GContactEntry::orgJobDescription ()
{
}

void
GContactEntry::setOrgName (QString orgName)
{
    QContactOrganization org;
    org.setName (orgName);

    mContact->saveDetail (&org);
}

QString
GContactEntry::orgName ()
{
}

void
GContactEntry::setOrgSymbol (QString orgSymbol)
{
    //TODO: Custom field
}

QString
GContactEntry::orgSymbol ()
{
}

void
GContactEntry::setOrgTitle (QString orgTitle)
{
    QContactOrganization org;
    org.setTitle (orgTitle);

    mContact->saveDetail (&org);
}

QString
GContactEntry::orgTitle ()
{
}

void
GContactEntry::setOriginalEvent (QString origEvent)
{
    //TODO: Custom field
}

QString
GContactEntry::originalEvent ()
{
}

void
GContactEntry::setPhoneNumber (QString phoneNumber, QString rel, QString uri, QString primary=false)
{
    QContactPhoneNumber number;
    number.setNumber (phoneNumber);
    number.setSubTypes (rel);

    //TODO: Handle primary and uri fields
    mContact->saveDetail (&number);
}

QString
GContactEntry::phoneNumber ()
{
}

void
GContactEntry::setRating (QString max, QString min, QString average,
                    QString numRaters, QString rel, QString value)
{
    //TODO: Custom field
}

QString
GContactEntry::rating ()
{
}

void
GContactEntry::setPostalAddrAgent (QString agent)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrAgent ()
{
    //TODO: Custom field
}

void
GContactEntry::setPostalAddrHousename (QString housename)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrHousename ()
{
}

void
GContactEntry::setPostalAddrStreet (QString street)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrStreet ()
{
}

void
GContactEntry::setPostalAddrPobox (QString pobox)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrPobox ()
{
}

void
GContactEntry::setPostalAddrNeighborhood (QString neighborhood)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrNeighborhood ()
{
}

void
GContactEntry::setPostalAddrCity (QString city)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrCity ()
{
    //TODO: Custom field
}

void
GContactEntry::setPostalAddrSubregion (QString subregion)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrSubregion ()
{
}

void
GContactEntry::setPostalAddrRegion (QString region)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrRegion ()
{
}

void
GContactEntry::setPostalAddrPostCode (QString postCode)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrPostCode ()
{
}

void
GContactEntry::setPostalAddrCountry (QString country)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrCountry ()
{
}

void
GContactEntry::setPostalAddrFormatted (QString formattedAddr)
{
    //TODO: Custom field
}

QString
GContactEntry::postalAddrFormatted ()
{
}

void
GContactEntry::setPostalAddrAttrs(QString rel, QString mailClass, QString usage, QString primary)
{
}

