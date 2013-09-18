/*
 * This file is part of buteo-gcontact-plugin package
 *
 * Copyright (C) 2013 Jolla Ltd. and/or its subsidiary(-ies).
 *
 * Contributors: Sateesh Kavuri <sateesh.kavuri@gmail.com>
 *               Mani Chandrasekar <maninc@gmail.com>
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

#include <QContactUrl>
#include <QContactAnniversary>
#include <QContact>
#include <QContactGuid>
#include <QContactName>
#include <QContactEmailAddress>
#include <QContactBirthday>
#include <QContactGender>
#include <QContactHobby>
#include <QContactNickname>
#include <QContactNote>
#include <QContactOnlineAccount>
#include <QContactOrganization>
#include <QContactPhoneNumber>
#include <QContactAddress>
#include <QContactSyncTarget>
#include <QContactTimestamp>
#include <QContactAvatar>
#include <QContactOriginMetadata>
#include <qcontactoriginmetadata_impl.h>


const QString GDATA_SCHEMA ("http://schemas.google.com/g/2005");

GContactEntry::GContactEntry() :
    mDeleted (false), mHasPhoto (false)
{
    // Set the timestamp explictly, since otherwise, qtcontacts-tracker
    // seems to mess up the created timestamp
    QContactTimestamp ts;
    ts.setCreated (QDateTime::currentDateTimeUtc ());
    mQContact.saveDetail (&ts);
}

void
GContactEntry::setGuid (const QString id)
{
    LOG_DEBUG ("### GUID=" << id);
    QContactGuid contactGuid = mQContact.detail<QContactGuid> ();
    contactGuid.setGuid (id);
    mQContact.saveDetail (&contactGuid);
    mId = id;
}

QString
GContactEntry::guid ()
{
    return mId;
}

void GContactEntry::setLocalId(const QString localId) {
    mLocalId = localId;
    QContactId id;
    id.fromString(mLocalId);
    mQContact.setId (id);
}

QString GContactEntry::localId() {
    return mLocalId;
}

void
GContactEntry::setEtag (const QString etag)
{
    QtContacts::QContactOriginMetadata etagDetail;
    etagDetail.setId(etag);
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
    QContactName name = mQContact.detail <QContactName>();
    name.setMiddleName (additionalName);
    mQContact.saveDetail (&name);
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
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail billing = mQContact.detail <GContactCustomDetail>();
    billing.setBillingInformation (billingInfo);

    mQContact.saveDetail (&billing);
#endif
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
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail calendarLink = mQContact.detail <GContactCustomDetail>();
    calendarLink.setCalendarLink (href);

    mQContact.saveDetail (&calendarLink);
#endif
}

void
GContactEntry::setDirectoryServer (const QString dirServer)
{
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail dirServerDetail = mQContact.detail <GContactCustomDetail>();
    dirServerDetail.setDirectoryServer (dirServer);

    mQContact.saveDetail (&dirServerDetail);
#endif
}

void
GContactEntry::setEvent (const QString event, const QString when)
{
    QContactAnniversary anniversary;

    anniversary.setEvent (event);
    anniversary.setOriginalDate (QDate::fromString (when, Qt::ISODate));

    mQContact.saveDetail (&anniversary);
}

void
GContactEntry::setExternalId (const QString externalId,
                              const QString rel)
{
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail externalIdDetail = mQContact.detail <GContactCustomDetail>();
    externalIdDetail.setExternalId (externalId);

    mQContact.saveDetail (&externalIdDetail);
#endif
}

void
GContactEntry::setGender (const QString gender)
{
    QContactGender contactGender;
    if (gender.compare("male", Qt::CaseInsensitive)) {
        contactGender.setGender (QContactGender::GenderMale);
    } else if (gender.compare("female", Qt::CaseInsensitive)) {
        contactGender.setGender (QContactGender::GenderMale);
    } else {
        contactGender.setGender (QContactGender::GenderUnspecified);
    }
    mQContact.saveDetail (&contactGender);
}

void
GContactEntry::setGroupMembershipInfo (const QString membershipInfo,
                                       const QString deleted)
{
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail membershipDetail = mQContact.detail <GContactCustomDetail>();
    membershipDetail.setGroupMembershipInfo (membershipInfo);

    mQContact.saveDetail (&membershipDetail);
#endif
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
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail initialsDetail = mQContact.detail <GContactCustomDetail>();
    initialsDetail.setInitials (initials);

    mQContact.saveDetail (&initialsDetail);
#endif
}

void
GContactEntry::setJot (const QString jot, const QString jotType)
{
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail jotDetail = mQContact.detail <GContactCustomDetail>();
    jotDetail.setJot (jot);

    mQContact.saveDetail (&jotDetail);
#endif
}

void
GContactEntry::setLanguage (const QString language)
{
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail languageDetail = mQContact.detail <GContactCustomDetail>();
    languageDetail.setLanguage (language);

    mQContact.saveDetail (&languageDetail);
#endif
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
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail mileageDetail = mQContact.detail <GContactCustomDetail>();
    mileageDetail.setMileage (mileage);

    mQContact.saveDetail (&mileageDetail);
#endif
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
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail occupationDetail = mQContact.detail <GContactCustomDetail>();
    occupationDetail.setOccupation (occupation);

    mQContact.saveDetail (&occupationDetail);
#endif
}

void
GContactEntry::setPriority (const QString priority)
{
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail priorityDetail;
    priorityDetail.setPriority (priority);

    mQContact.saveDetail (&priorityDetail);
#endif
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
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail sensitivityDetail = mQContact.detail <GContactCustomDetail>();
    sensitivityDetail.setSensitivity (sensitivity);

    mQContact.saveDetail (&sensitivityDetail);
#endif
}

void
GContactEntry::setShortname (const QString shortname)
{
    // TODO: Custom field
}

void
GContactEntry::setSubject (const QString subject)
{
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail subjectDetail = mQContact.detail <GContactCustomDetail>();
    subjectDetail.setSubject (subject);

    mQContact.saveDetail (&subjectDetail);
#endif
}

void
GContactEntry::setSystemGroup (const QString systemGroup)
{
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail systemGroupDetail = mQContact.detail <GContactCustomDetail>();
    systemGroupDetail.setSystemGroup (systemGroup);

    mQContact.saveDetail (&systemGroupDetail);
#endif
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
    QContactUrl url = mQContact.detail <QContactUrl> ();
    url.setUrl (website);

    mQContact.saveDetail (&url);
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
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail countryDetail = mQContact.detail <GContactCustomDetail>();
    countryDetail.setCountry (country);

    mQContact.saveDetail (&countryDetail);
#endif
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
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail feedlinkDetail = mQContact.detail <GContactCustomDetail>();
    feedlinkDetail.setFeedLink (feedLink);

    mQContact.saveDetail (&feedlinkDetail);
#endif
}

void
GContactEntry::setIm (const QString address, const QString rel,
                      const QString protocol, const QString primary)
{
    QContactOnlineAccount imAccount;
    imAccount.setAccountUri (address);
    //imAccount.setProtocol (protocol);
    //imAccount.setServiceProvider (protocol);  // FIXME: How is protocol different from service provider?

    mQContact.saveDetail (&imAccount);
}

void
GContactEntry::setMoney (const QString amount, const QString currencyCode)
{
#ifdef CUSTOM_DETAIL_IS_SUPPORTED
    GContactCustomDetail moneyDetail = mQContact.detail <GContactCustomDetail>();
    moneyDetail.setMoney (amount);

    mQContact.saveDetail (&moneyDetail);
#endif
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
    //number.setSubTypes (rel);

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
    // TODO: Custom field
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
    QContactAddress address = mQContact.detail <QContactAddress>();
    address.setLocality (neighborhood);

    mQContact.saveDetail (&address);
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
    target.setSyncTarget ("buteo");

    mQContact.saveDetail (&target);
}

void
GContactEntry::setBatchResponseStatusCode (const int code)
{
    this->mResponseCode = code;
}

void
GContactEntry::setBatchResponseReason (const QString reason)
{
    this->mReason = reason;
}

void
GContactEntry::setBatchResponseReasonText (const QString reasonText)
{
    this->mReasonText = reasonText;
}

void
GContactEntry::GContactEntry::setBatchResponseOpsType (const QString opsType)
{
}

QContact
GContactEntry::qContact()
{
    return mQContact;
}

void
GContactEntry::setError (const bool errorFlag)
{
    mError = errorFlag;
}

bool
GContactEntry::error ()
{
    return mError;
}

void
GContactEntry::setHasPhoto (const bool hasPhotoFlag)
{
    mHasPhoto = hasPhotoFlag;
}

bool
GContactEntry::hasPhoto ()
{
    return mHasPhoto;
}

void
GContactEntry::setPhotoUrl (const QString photoUrl)
{
    mPhotoUrl = photoUrl;

    QContactAvatar avatar = mQContact.detail <QContactAvatar> ();
    QUrl url (photoUrl);
    avatar.setImageUrl (url);
    mQContact.saveDetail (&avatar);
}

QString
GContactEntry::photoUrl ()
{
    return mPhotoUrl;
}
