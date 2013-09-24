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

#include "GWriteStream.h"
#include "GContactCustomDetail.h"

#include <QXmlStreamWriter>

#include <QVariant>
#include <QHash>

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
#include <QContactAvatar>
#include <QContactFamily>
#include <QContactTimestamp>
#include <QContactManager>
#include <QContactOriginMetadata>

#include <Accounts/Manager>
#include <Accounts/Account>

GWriteStream::GWriteStream(quint32 accountId)
                : mXmlWriter (&mXmlBuffer), mAccountId(accountId)
{
}

GWriteStream::~GWriteStream ()
{
}

void
GWriteStream::encodeAllContacts ()
{
    QList<QContact> allContacts;
    QList<QContactId> allContactIds = mContactsBackend.getAllContactIds ();

    if (allContactIds.size () <= 1)
        return;

    // QtContacts has a fictious first contact that has nothing
    // in it. Removing this unwanted element
    allContactIds.removeFirst ();

    QHash<QContactId, GConfig::TRANSACTION_TYPE> qContactMap;
    foreach (QContactId localId, allContactIds)
    {
        qContactMap.insert (localId, GConfig::ADD);
    }

    encodeContact (qContactMap);
}

void
GWriteStream::encodeContacts (const QList<QContactId> idList, GConfig::TRANSACTION_TYPE type)
{
    QHash<QContactId, GConfig::TRANSACTION_TYPE> qContactMap;
    foreach (QContactId localId, idList)
    {
        qContactMap.insert (localId, type);
    }

    encodeContact (qContactMap);
}

QByteArray
GWriteStream::encodeContact (QHash<QContactId, GConfig::TRANSACTION_TYPE> qContactMap)
{
    if (qContactMap.size () <= 0)
        return mXmlBuffer;

    mXmlWriter.writeStartDocument ();
    // Encode everything as a batch request
    bool batch = true;
    startBatchFeed ();

    QHash<QContactId, GConfig::TRANSACTION_TYPE>::iterator contactPair;
    for (contactPair = qContactMap.begin (); contactPair != qContactMap.end (); ++contactPair)
    {
        QContact contact;
        mContactsBackend.getContact (contactPair.key (), contact);
        if (!contact.isEmpty ())
            encodeContact (contact, contactPair.value (), batch);
    }

    if (batch == true)
        endBatchFeed ();

    mXmlWriter.writeEndDocument ();

    return mXmlBuffer;
}

QByteArray
GWriteStream::encodedStream ()
{
    return mXmlBuffer;
}

QByteArray
GWriteStream::encodeContact(const QContact qContact,
                            const GConfig::TRANSACTION_TYPE updateType,
                            const bool batch)
{
    QList<QContactDetail> allDetails = qContact.details ();

    // Etag encoding has to immediately succeed writeStartElement ("atom:entry"),
    // since etag is an attribute of this element
    if (batch == true) {
        mXmlWriter.writeStartElement ("atom:entry");
        encodeEtag (qContact);
        encodeBatchTag (updateType, qContact.id().toString());
    } else {
        mXmlWriter.writeStartElement ("atom:entry");
        mXmlWriter.writeAttribute ("xmlns:atom", "http://www.w3.org/2005/Atom");
        mXmlWriter.writeAttribute ("xmlns:gd", "http://schemas.google.com/g/2005");
        mXmlWriter.writeAttribute (" xmlns:gContact", "http://schemas.google.com/contact/2008");
    }

    if (updateType == GConfig::UPDATE) {
        encodeId (qContact);
        encodeUpdated (qContact);
    }

    if (updateType == GConfig::DELETE) {
        encodeId (qContact);
        mXmlWriter.writeEndElement ();
        return mXmlBuffer;
    }

    // Category is not required for deleted entries. So place it
    // here
    encodeCategory ();

    foreach (const QContactDetail& detail, allDetails)
    {
        if (detail.type() == QContactName::Type)
            encodeName (detail);
        else if (detail.type() == QContactPhoneNumber::Type)
            encodePhoneNumber (detail);
        else if (detail.type() == QContactEmailAddress::Type)
            encodeEmail (detail);
        else if (detail.type() == QContactAddress::Type)
            encodeAddress (detail);
        else if (detail.type() == QContactUrl::Type)
            encodeUrl (detail);
        else if (detail.type () == QContactBirthday::Type)
            encodeBirthDay (detail);
        else if (detail.type () == QContactNote::Type)
            encodeNote (detail);
        else if (detail.type () == QContactHobby::Type)
            encodeHobby (detail);
        else if (detail.type () == QContactOrganization::Type)
            encodeOrganization (detail);
        else if (detail.type () == QContactAvatar::Type)
            encodeAvatar (detail, qContact);
        else if (detail.type () == QContactAnniversary::Type)
            encodeAnniversary (detail);
        else if (detail.type () == QContactNickname::Type)
            encodeNickname (detail);
        else if (detail.type () == QContactGender::Type)
            encodeGender (detail);
        else if (detail.type () == QContactOnlineAccount::Type)
            encodeOnlineAccount(detail);
        else if (detail.type () == QContactFamily::Type)
            encodeFamily (detail);
        // TODO: handle the custom detail fields. If sailfish UI
        // does not handle these contact details, then they
        // need not be pushed to the server
    }
    mXmlWriter.writeEndElement ();

    return mXmlBuffer;
}

void
GWriteStream::startBatchFeed ()
{
    mXmlWriter.writeStartElement ("atom:feed");
    mXmlWriter.writeAttribute ("xmlns:atom", "http://www.w3.org/2005/Atom");
    mXmlWriter.writeAttribute ("xmlns:gContact", "http://schemas.google.com/contact/2008");
    mXmlWriter.writeAttribute ("xmlns:gd", "http://schemas.google.com/g/2005");
    mXmlWriter.writeAttribute ("xmlns:batch", "http://schemas.google.com/gdata/batch");
}

void
GWriteStream::endBatchFeed ()
{
    mXmlWriter.writeEndElement ();
}

void
GWriteStream::encodeBatchTag (const GConfig::TRANSACTION_TYPE type, const QString batchId)
{
    mXmlWriter.writeTextElement ("batch:id", batchId);
    if (type == GConfig::ADD)
    {
        mXmlWriter.writeEmptyElement ("batch:operation");
        mXmlWriter.writeAttribute ("type", "insert");
    } else if (type == GConfig::UPDATE)
    {
        mXmlWriter.writeEmptyElement ("batch:operation");
        mXmlWriter.writeAttribute ("type", "update");
    } else if (type == GConfig::DELETE)
    {
        mXmlWriter.writeEmptyElement ("batch:operation");
        mXmlWriter.writeAttribute ("type", "delete");
    }
}

void
GWriteStream::encodeId (const QContact qContact)
{
    QString guid = qContact.detail (
                QContactGuid::Type).value (QContactGuid::FieldGuid).toString();

    if (!guid.isNull ()) {
        QString uname = "default";
        Accounts::Manager mgr;
        Accounts::Account *account = mgr.account(mAccountId);
        if (account != NULL) {
            QVariant val = QVariant::String;
            account->value("name", val);
            uname = val.toString();
        } else {
            LOG_DEBUG(mgr.lastError().message());
        }

        mXmlWriter.writeTextElement ("atom:id", "http://www.google.com/m8/feeds/contacts/" + uname + "/full/" + guid);
    }
}

void
GWriteStream::encodeUpdated (const QContact qContact)
{
    QString updated = qContact.detail (
                QContactTimestamp::Type).value (QContactTimestamp::FieldModificationTimestamp).toString();

    if (!updated.isNull ())
        mXmlWriter.writeTextElement ("updated", updated);
}

void
GWriteStream::encodeEtag (const QContact qContact)
{
    QtContacts::QContactOriginMetadata etagDetail = qContact.detail<QtContacts::QContactOriginMetadata>();
    QString etag = etagDetail.id();

    if (!etag.isNull ())
        mXmlWriter.writeAttribute ("gd:etag", etag);
}

void
GWriteStream::encodeCategory ()
{
    mXmlWriter.writeEmptyElement ("atom:category");
    mXmlWriter.writeAttribute ("schema", "http://schemas.google.com/g/2005#kind");
    mXmlWriter.writeAttribute ("term", "http://schemas.google.com/contact/2008#contact");
}

void
GWriteStream::encodeLink ()
{
}

void
GWriteStream::encodeName (const QContactDetail& detail)
{
    QContactName contactName = static_cast<QContactName>(detail);
    mXmlWriter.writeStartElement ("gd:name");

    if (!contactName.firstName ().isEmpty ())
        mXmlWriter.writeTextElement ("gd:givenName", contactName.firstName ());
    if (!contactName.middleName().isEmpty())
        mXmlWriter.writeTextElement ("gd:additionalName", contactName.middleName ());
    if (!contactName.lastName().isEmpty())
        mXmlWriter.writeTextElement ("gd:familyName", contactName.lastName ());
    if (!contactName.prefix().isEmpty())
        mXmlWriter.writeTextElement ("gd:namePrefix", contactName.prefix ());
    if (!contactName.suffix().isEmpty())
        mXmlWriter.writeTextElement ("gd:nameSuffix", contactName.suffix ());

    mXmlWriter.writeEndElement ();
}

/*!
 * Encode Phone Number Field Information into the Google contact XML Document
 */
void
GWriteStream::encodePhoneNumber (const QContactDetail& detail)
{
    QContactPhoneNumber phoneNumber = static_cast<QContactPhoneNumber>(detail);
    QList<int> subTypes = phoneNumber.subTypes();
    if (phoneNumber.number().isEmpty ())
        return;

    mXmlWriter.writeStartElement ("gd:phoneNumber");

    QString rel = "http://schemas.google.com/g/2005#";

    QString type;
    if (subTypes.isEmpty()) {
        type = "mobile";
    } else {
        switch(subTypes.first()) {
            case QContactPhoneNumber::SubTypeLandline:
            type = "landline";
            break;
            case QContactPhoneNumber::SubTypeMobile:
            type = "mobile";
            break;
            case QContactPhoneNumber::SubTypeFax:
            type = "fax";
            break;
            case QContactPhoneNumber::SubTypePager:
            type = "pager";
            break;
            case QContactPhoneNumber::SubTypeModem:
            type = "tty_tdd";
            break;
            case QContactPhoneNumber::SubTypeCar:
            type = "car";
            break;
            case QContactPhoneNumber::SubTypeBulletinBoardSystem:
            type = "telex";
            break;
            case QContactPhoneNumber::SubTypeAssistant:
            type = "assistant";
            break;
        }
    }

    mXmlWriter.writeAttribute ("rel", rel + type);
    mXmlWriter.writeCharacters (phoneNumber.number());
    mXmlWriter.writeEndElement ();
}

/*!
 * Encode Email Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeEmail (const QContactDetail& detail)
{
    QContactEmailAddress emailAddress = static_cast<QContactEmailAddress>(detail);
    if (emailAddress.emailAddress ().isEmpty ())
        return;

    // TODO: Handle 'rel' attribute, which is subtype
    mXmlWriter.writeEmptyElement ("gd:email");
    mXmlWriter.writeAttribute ("rel", "http://schemas.google.com/g/2005#other");
    mXmlWriter.writeAttribute ("address", emailAddress.emailAddress ());
}

/*!
 * Encode Address Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeAddress (const QContactDetail& detail)
{
    QContactAddress address = static_cast<QContactAddress>(detail);

    mXmlWriter.writeStartElement ("gd:structuredPostalAddress");
    mXmlWriter.writeAttribute ("rel", "http://schemas.google.com/g/2005#other");

    if (!address.street ().isEmpty ())
        mXmlWriter.writeTextElement ("gd:street", address.street ());
    if (!address.locality ().isEmpty ())
        mXmlWriter.writeTextElement ("gd:neighborhood", address.locality ());
    if (!address.postOfficeBox ().isEmpty ())
        mXmlWriter.writeTextElement ("gd:pobox", address.postOfficeBox ());
    if (!address.region ().isEmpty ())
        mXmlWriter.writeTextElement ("gd:region", address.region ());
    if (!address.postcode ().isEmpty ())
        mXmlWriter.writeTextElement ("gd:postcode", address.postcode ());
    if (!address.country ().isEmpty ())
        mXmlWriter.writeTextElement ("gd:country", address.country ());

    mXmlWriter.writeEndElement ();
}

/*!
 * Encode URL Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeUrl (const QContactDetail& detail)
{
    QContactUrl contactUrl = static_cast<QContactUrl>(detail);
    if (contactUrl.url ().isEmpty ())
        return;
    mXmlWriter.writeEmptyElement ("gContact:website");
    mXmlWriter.writeAttribute ("rel", "home-page");
    mXmlWriter.writeAttribute ("href", contactUrl.url ());
}

/*!
 * Encode BirthDay Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeBirthDay (const QContactDetail& detail)
{
    QContactBirthday bday = static_cast<QContactBirthday>(detail);
    if (bday.date ().isNull ())
        return;

    QString value = bday.date().toString(Qt::ISODate);

    mXmlWriter.writeEmptyElement ("gContact:birthday");
    mXmlWriter.writeAttribute ("when", value);
}

/*!
 * Encode Comment i.e. Note Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeNote (const QContactDetail& detail)
{
    QContactNote contactNote = static_cast<QContactNote>(detail);
    if (contactNote.note ().isEmpty ())
        return;

    mXmlWriter.writeStartElement ("atom:content");
    mXmlWriter.writeAttribute ("type", "text");
    mXmlWriter.writeCharacters (contactNote.note ());
    mXmlWriter.writeEndElement ();
}

/*!
 * Encode Hobby i.e. Hobby Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeHobby (const QContactDetail& detail)
{
    QContactHobby contactHobby = static_cast<QContactHobby>(detail);
    if (contactHobby.hobby ().isEmpty ())
        return;
    mXmlWriter.writeTextElement ("gContact:hobby", contactHobby.hobby ());
}

/*!
 * Encode Geo Prpoperties Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeGeoLocation (const QContactDetail& detail)
{
    /* TODO: Need to check how to handle GeoRSS geo location
    QContactGeoLocation geoLocation = static_cast<QContactGeoLocation>(detail);
    property.setValue(QStringList() << QString::number(geoLocation.latitude())
                      << QString::number(geoLocation.longitude()));
    */
}

/*!
 * Encode organization properties to the versit document
 */
void
GWriteStream::encodeOrganization (const QContactDetail& detail)
{
    QContactOrganization organization = static_cast<QContactOrganization>(detail);
    mXmlWriter.writeStartElement ("gd:organization");
    // FIXME: The organization type should be obtained from DB
    mXmlWriter.writeAttribute ("rel", "http://schemas.google.com/g/2005#work");
    if (organization.title().length () > 0)
        mXmlWriter.writeTextElement ("gd:orgTitle", organization.title ());
    if (organization.name().length() > 0 )
        mXmlWriter.writeTextElement ("gd:orgName", organization.name ());
    if (organization.department ().length () > 0)
    {
        QString departmets;
        foreach (const QString dept, organization.department ())
            departmets.append (dept);

        mXmlWriter.writeTextElement ("gd:orgDepartment", departmets);
    }

    mXmlWriter.writeEndElement ();
}

/*!
 * Encode avatar URIs into the Google contact XML Document
 */
void
GWriteStream::encodeAvatar (const QContactDetail &detail, const QContact qContact)
{
    mContactsWithAvatars << qContact.id ();

    QContactAvatar contactAvatar = static_cast<QContactAvatar>(detail);
    QUrl imageUrl(contactAvatar.imageUrl());

    // If the url stored for the contact is the url from google,
    // then there is no need to sync it back to server
    // Only if it is a locally added avatar, it needs to be sync'd
    // to the server
    if ((imageUrl.host () != "www.google.com") &&
         imageUrl.isValid () &&
         !imageUrl.isEmpty ())
    {
        QContactGuid guid = qContact.detail<QContactGuid>();
        imageUrl.setFragment(guid.guid ());
    }
}

/*!
 * Encode gender property information into Google contact XML Document
 */
void
GWriteStream::encodeGender (const QContactDetail &detail)
{
    QContactGender gender = static_cast<QContactGender>(detail);
    if (gender.gender() == QContactGender::GenderUnspecified)
        return;

    mXmlWriter.writeEmptyElement ("gContact:gender");
    switch(gender.gender()) {
        case QContactGender::GenderMale:
        mXmlWriter.writeAttribute ("value", "male");
        break;
        case QContactGender::GenderFemale:
        mXmlWriter.writeAttribute ("value", "female");
        break;
    }

}

/*!
 * Encodes nickname property information into the Google contact XML Document
 */
void
GWriteStream::encodeNickname (const QContactDetail &detail)
{
    QContactNickname nicknameDetail = static_cast<QContactNickname>(detail);
    if (nicknameDetail.nickname ().isEmpty ())
        return;
    mXmlWriter.writeTextElement ("gContact:nickname", nicknameDetail.nickname ());
}

/*!
 * Encode anniversary information into Google contact XML Document
 */
void
GWriteStream::encodeAnniversary (const QContactDetail &detail)
{
    QContactAnniversary anniversary = static_cast<QContactAnniversary>(detail);
    if (!anniversary.event ().isEmpty () &&
        !anniversary.originalDate ().isNull ())
    {
        mXmlWriter.writeStartElement ("gContact:event");
        mXmlWriter.writeAttribute ("rel", "anniversary");
        mXmlWriter.writeEmptyElement ("gd:when");
        mXmlWriter.writeAttribute ("startTime", anniversary.originalDate ().toString (Qt::ISODate));
        mXmlWriter.writeEndElement ();
    }
}

/*!
 * Encode online account information into the Google contact XML Document
 */
void
GWriteStream::encodeOnlineAccount (const QContactDetail &detail)
{
    QContactOnlineAccount onlineAccount = static_cast<QContactOnlineAccount>(detail);
    if (onlineAccount.accountUri ().isEmpty ())
        return;

    QList<int> subTypes = onlineAccount.subTypes();
    QContactOnlineAccount::Protocol protocol = onlineAccount.protocol();

    QString propertyName;

    if (protocol == QContactOnlineAccount::ProtocolJabber) {
        propertyName = "JABBER";
    } else if (protocol == QContactOnlineAccount::ProtocolAim) {
        propertyName = "AIM";
    } else if (protocol == QContactOnlineAccount::ProtocolIcq) {
        propertyName = "ICQ";
    } else if (protocol == QContactOnlineAccount::ProtocolMsn) {
        propertyName = "MSN";
    } else if (protocol == QContactOnlineAccount::ProtocolQq) {
        propertyName = "QQ";
    } else if (protocol == QContactOnlineAccount::ProtocolYahoo) {
        propertyName = "YAHOO";
    } else if (protocol == QContactOnlineAccount::ProtocolSkype) {
        propertyName = "SKYPE";
    } else
        propertyName = protocol;

    mXmlWriter.writeEmptyElement ("gd:im");
    mXmlWriter.writeAttribute ("protocol", "http://schemas.google.com/g/2005#" + propertyName);
    // FIXME: The 'rel' value should be properly stored and retrieved
    mXmlWriter.writeAttribute ("rel", "http://schemas.google.com/g/2005#home");
    mXmlWriter.writeAttribute ("address", onlineAccount.accountUri ());
}

/*!
 * Encode family property if its supported in Google contact XML Document
 */
void
GWriteStream::encodeFamily (const QContactDetail &detail)
{
    QContactFamily family = static_cast<QContactFamily>(detail);

    if (family.spouse().length () > 0) {
        mXmlWriter.writeEmptyElement ("gContact:relation");
        mXmlWriter.writeAttribute ("rel", "spouse");
        mXmlWriter.writeCharacters (family.spouse ());
    }

    foreach (const QString member, family.children ())
    {
        mXmlWriter.writeEmptyElement ("gContact:relation");
        mXmlWriter.writeAttribute ("rel", "child");
        mXmlWriter.writeCharacters (member);
    }
}

QList<QContactId>&
GWriteStream::contactsWithAvatars ()
{
    return mContactsWithAvatars;
}
