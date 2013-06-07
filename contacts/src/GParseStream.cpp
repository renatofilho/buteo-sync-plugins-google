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

#include "GParseStream.h"
#include "GContactEntry.h"
#include "GAtom.h"
#include <LogMacros.h>

GParseStream::GParseStream (bool response, QObject* parent) :
                            QObject (parent), mXml (NULL), mAtom (NULL)
{
    FUNCTION_CALL_TRACE;

    if (response == true)
        initResponseFunctionMap ();
    else
        initFunctionMap ();
}

GParseStream::GParseStream(QByteArray xmlStream, QObject *parent) :
    QObject(parent)
{
    FUNCTION_CALL_TRACE;

    mXml = new QXmlStreamReader (xmlStream);
    mAtom = new GAtom ();

    initFunctionMap ();
}

GParseStream::~GParseStream ()
{
    FUNCTION_CALL_TRACE;
}

void
GParseStream::initAtomFunctionMap ()
{
    // Initialize the function map
    mAtomFunctionMap.insert ("updated", &GParseStream::handleAtomUpdated);
    mAtomFunctionMap.insert ("category", &GParseStream::handleAtomCategory);
    mAtomFunctionMap.insert ("author", &GParseStream::handleAtomAuthor);
    mAtomFunctionMap.insert ("totalResults", &GParseStream::handleAtomOpenSearch);
    mAtomFunctionMap.insert ("startIndex", &GParseStream::handleAtomOpenSearch);
    mAtomFunctionMap.insert ("itemsPerPage", &GParseStream::handleAtomOpenSearch);
    mAtomFunctionMap.insert ("link", &GParseStream::handleAtomLink);
    mAtomFunctionMap.insert ("entry", &GParseStream::handleAtomEntry);
}

void
GParseStream::initResponseFunctionMap ()
{
    initAtomFunctionMap ();

    mContactFunctionMap.insert ("id", &GParseStream::handleEntryId);
    mContactFunctionMap.insert ("title", &GParseStream::handleEntryTitle);

    mContactFunctionMap.insert ("gContact:userDefinedField", &GParseStream::handleEntryUserDefinedField);

    mContactFunctionMap.insert ("batch:status", &GParseStream::handleEntryBatchStatus);
    mContactFunctionMap.insert ("batch:operation", &GParseStream::handleEntryBatchOperation);
}

void
GParseStream::initFunctionMap ()
{
    FUNCTION_CALL_TRACE;

    initAtomFunctionMap ();

    mContactFunctionMap.insert ("id", &GParseStream::handleEntryId);
    mContactFunctionMap.insert ("content", &GParseStream::handleEntryContent);
    mContactFunctionMap.insert ("title", &GParseStream::handleEntryTitle);
    mContactFunctionMap.insert ("link", &GParseStream::handleEntryLink);
    mContactFunctionMap.insert ("gContact:billingInformation", &GParseStream::handleEntryBillingInformation);
    mContactFunctionMap.insert ("gContact:birthday", &GParseStream::handleEntryBirthday);
    mContactFunctionMap.insert ("gContact:calendarLink", &GParseStream::handleEntryCalendarLink);
    mContactFunctionMap.insert ("gContact:directoryServer", &GParseStream::handleEntryDirectoryServer);
    mContactFunctionMap.insert ("gContact:event", &GParseStream::handleEntryEvent);
    mContactFunctionMap.insert ("gContact:externalId", &GParseStream::handleEntryExternalId);
    mContactFunctionMap.insert ("gcontact::gender", &GParseStream::handleEntryGender);
    mContactFunctionMap.insert ("gcontact:groupMembershipInfo", &GParseStream::handleEntryGroupMembershipInfo);
    mContactFunctionMap.insert ("gContact:hobby", &GParseStream::handleEntryHobby);
    mContactFunctionMap.insert ("gContact:initials", &GParseStream::handleEntryInitials);
    mContactFunctionMap.insert ("gContact:jot", &GParseStream::handleEntryJot);
    mContactFunctionMap.insert ("gContact:language", &GParseStream::handleEntryLanguage);
    mContactFunctionMap.insert ("gContact:maidenName", &GParseStream::handleEntryMaidenName);
    mContactFunctionMap.insert ("gContact:mileage", &GParseStream::handleEntryMileage);
    mContactFunctionMap.insert ("gContact:nickname", &GParseStream::handleEntryNickname);
    mContactFunctionMap.insert ("gContact:occupation", &GParseStream::handleEntryOccupation);
    mContactFunctionMap.insert ("gContact:priority", &GParseStream::handleEntryPriority);
    mContactFunctionMap.insert ("gContact:relation", &GParseStream::handleEntryRelation);
    mContactFunctionMap.insert ("gContact:sensitivity", &GParseStream::handleEntrySensitivity);
    mContactFunctionMap.insert ("gContact:shortName", &GParseStream::handleEntryShortname);
    mContactFunctionMap.insert ("gContact:subject", &GParseStream::handleEntrySubject);
    mContactFunctionMap.insert ("gContact:systemGroup", &GParseStream::handleEntrySystemGroup);
    mContactFunctionMap.insert ("gContact:userDefinedField", &GParseStream::handleEntryUserDefinedField);
    mContactFunctionMap.insert ("gContact:website", &GParseStream::handleEntryWebsite);

    mContactFunctionMap.insert ("gd:additionalName", &GParseStream::handleEntryAdditionalName);
    mContactFunctionMap.insert ("gd:comments", &GParseStream::handleEntryComments);
    mContactFunctionMap.insert ("gd:country", &GParseStream::handleEntryCountry);
    mContactFunctionMap.insert ("gd:deleted", &GParseStream::handleEntryDeleted);
    mContactFunctionMap.insert ("gd:email", &GParseStream::handleEntryEmail);
    mContactFunctionMap.insert ("gd:extendedProperty", &GParseStream::handleEntryExtendedProperty);
    mContactFunctionMap.insert ("gd:feedLink", &GParseStream::handleEntryFeedLink);
    mContactFunctionMap.insert ("gd:im", &GParseStream::handleEntryIm);
    mContactFunctionMap.insert ("gd:money", &GParseStream::handleEntryMoney);
    mContactFunctionMap.insert ("gd:name", &GParseStream::handleEntryName);
    mContactFunctionMap.insert ("gd:organization", &GParseStream::handleEntryOrganization);
    mContactFunctionMap.insert ("gd:originalEvent", &GParseStream::handleEntryOriginalEvent);
    mContactFunctionMap.insert ("gd:phoneNumber", &GParseStream::handleEntryPhoneNumber);
    mContactFunctionMap.insert ("gd:rating", &GParseStream::handleEntryRating);
    mContactFunctionMap.insert ("gd:structuredPostalAddress", &GParseStream::handleEntryStructuredPostalAddress);

    mContactFunctionMap.insert ("batch:status", &GParseStream::handleEntryBatchStatus);
    mContactFunctionMap.insert ("batch:operation", &GParseStream::handleEntryBatchOperation);
}

GAtom*
GParseStream::parse (const QByteArray xmlBuffer)
{
    FUNCTION_CALL_TRACE;

    mXml = new QXmlStreamReader (xmlBuffer);
    mAtom = new GAtom ();

    Q_CHECK_PTR (mXml);
    Q_CHECK_PTR (mAtom);

    while (!mXml->atEnd () && !mXml->hasError ())
    {
        if (mXml->readNextStartElement ())
        {
            Handler handler = mAtomFunctionMap.value (mXml->name ().toString ());
            if (handler)
                (*this.*handler) ();
        }
    }
    delete mXml;

    return mAtom;
}

void
GParseStream::handleAtomUpdated ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->name () == "updated");

    mAtom->setUpdated (mXml->readElementText ());
}

void
GParseStream::handleAtomCategory ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->name () == "category");

    QXmlStreamAttributes attributes = mXml->attributes ();
    QString schema, term;
    if (attributes.hasAttribute ("scheme"))
        schema = attributes.value ("scheme").toString ();
    else if (attributes.hasAttribute ("term"))
        term = attributes.value ("term").toString ();

    mAtom->setCategory (schema, term);
}

void
GParseStream::handleAtomAuthor ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->name () == "author");

    while (!(mXml->tokenType () == QXmlStreamReader::EndElement &&
             mXml->name () == "author"))
    {
        if (mXml->tokenType () == QXmlStreamReader::StartElement)
        {
            if (mXml->name () == "name")
                mAtom->setAuthorName (mXml->readElementText ());
            else if (mXml->name () == "email")
                mAtom->setAuthorEmail (mXml->readElementText ());
        }
        mXml->readNextStartElement ();
    }
}

void
GParseStream::handleAtomOpenSearch ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->prefix () == "openSearch");

    if (mXml->name() == "totalResults")
        mAtom->setTotalResults (mXml->readElementText ().toInt ());
    else if (mXml->name() == "startIndex")
        mAtom->setStartIndex (mXml->readElementText ().toInt ());
    else if (mXml->name () == "itemsPerPage")
        mAtom->setItemsPerPage (mXml->readElementText ().toInt ());
}

void
GParseStream::handleAtomLink ()
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT(mXml->isStartElement () && mXml->name () == "link");
    if (mXml->attributes ().hasAttribute ("rel") &&
        (mXml->attributes ().value ("rel") == "next"))
    {
        mAtom->setNextEntriesUrl (mXml->attributes ().value ("href").toString ());
    }
}

void
GParseStream::handleEntryBatchStatus ()
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT(mXml->isStartElement () && (mXml->name () == "status"));

    mContactEntry->setBatchResponseStatusCode (mXml->attributes ().value ("code").toString ().toInt ());
    mContactEntry->setBatchResponseReason (mXml->attributes ().value ("reason").toString ());
    mContactEntry->setBatchResponseReasonText (mXml->readElementText ());
}

void
GParseStream::handleEntryBatchOperation ()
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT(mXml->isStartElement () && (mXml->name () == "operation"));

    mContactEntry->setBatchResponseOpsType (mXml->readElementText ());
}

void
GParseStream::handleAtomEntry ()
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT(mXml->isStartElement () && mXml->name () == "entry");

    mContactEntry = new GContactEntry ();
    Q_CHECK_PTR (mContactEntry);

    // TODO: Optimize the parsing of the entries for GET and POST
    // responses. For POST response, there is no need for parsing the complete
    // response, but just the id is enough

    // Store the etag value of the entry
    mContactEntry->setEtag (mXml->attributes ().value ("gd:etag").toString ());

    while (!((mXml->tokenType () == QXmlStreamReader::EndElement) &&
             (mXml->name () == "entry")))
    {
        if (mXml->tokenType () == QXmlStreamReader::StartElement)
        {
            Handler handler = mContactFunctionMap.value (mXml->qualifiedName ().toString ());
            if (handler)
                (*this.*handler) ();
        }
        mXml->readNextStartElement ();
    }
    mContactEntry->setSyncTarget ();
    mAtom->addEntry (mContactEntry);
}

void
GParseStream::handleEntryId ()
{
    QString idUrl = mXml->readElementText ();
    mContactEntry->setGuid (idUrl.remove (0, idUrl.lastIndexOf ('/') + 1));
}

void
GParseStream::handleEntryContent ()
{
    QString content = mXml->readElementText ();
    mContactEntry->setComments (content);
}

void
GParseStream::handleEntryTitle ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->name () == "title");

    QString title = mXml->readElementText ();
    mContactEntry->setFullName (mXml->readElementText ());

    if (title.contains ("Error"))
        mContactEntry->setError (true);
}

void
GParseStream::handleEntryLink ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->name () == "link");

    if (mXml->attributes ().hasAttribute ("gd:etag") &&
        (mXml->attributes ().value ("rel") == "http://schemas.google.com/contacts/2008/rel#photo"))
    {
        // the contact entry has a photo
        mContactEntry->setHasPhoto (true);
        mContactEntry->setPhotoUrl (mXml->attributes ().value ("href").toString ());
    }
}

void
GParseStream::handleEntryBillingInformation ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:billingInformation");

    mContactEntry->setBillingInformation (mXml->readElementText ());
}

void
GParseStream::handleEntryBirthday ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:birthday");

    mContactEntry->setBirthday (mXml->attributes ().value ("when").toString ());
}

void
GParseStream::handleEntryCalendarLink ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:calendarLink");
    QXmlStreamAttributes attrs = mXml->attributes ();
    QString label, rel, href, primary;
    if (attrs.hasAttribute ("label"))
    {
        label = attrs.value ("label").toString ();
    } else if (attrs.hasAttribute ("rel"))
    {
        rel = attrs.value ("rel").toString ();
    }

    if (attrs.hasAttribute ("primary"))
    {
        primary = attrs.value ("primary").toString ();
    }

    href = attrs.value ("href").toString ();

    mContactEntry->setCalendarLink (href, rel, primary);
}

void
GParseStream::handleEntryDirectoryServer ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:directoryServer");

    mContactEntry->setDirectoryServer (mXml->readElementText ());
}

void
GParseStream::handleEntryEvent ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:event");

    QXmlStreamAttributes attrs = mXml->attributes ();
    QString eventType;
    if (attrs.hasAttribute ("label"))
        eventType = attrs.value ("label").toString ();
    else if (attrs.hasAttribute ("rel"))
        eventType = attrs.value ("rel").toString ();

    QString when = attrs.value ("when").toString ();

    mContactEntry->setEvent (eventType, when);
}

void
GParseStream::handleEntryExternalId ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:externalId");

    QXmlStreamAttributes attrs = mXml->attributes ();
    QString label, rel, value;
    if (attrs.hasAttribute ("label"))
        label == attrs.value ("label").toString ();

    rel = attrs.value ("rel").toString ();
    value = attrs.value ("value").toString ();
    mContactEntry->setExternalId (value, rel);
}

void
GParseStream::handleEntryGender ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:gender");

    mContactEntry->setGender (mXml->attributes ().value ("value").toString ());
}

void
GParseStream::handleEntryGroupMembershipInfo ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:groupMembershipInfo");

    QXmlStreamAttributes attrs = mXml->attributes ();

    QString deleted, groupId = attrs.value ("href").toString ();
    if (attrs.hasAttribute ("deleted"))
        deleted = attrs.value ("deleted").toString ();

    mContactEntry->setGroupMembershipInfo (groupId, deleted);
}

void
GParseStream::handleEntryHobby ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:hobby");

    mContactEntry->setHobby (mXml->readElementText ());
}

void
GParseStream::handleEntryInitials ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:initials");

    mContactEntry->setInitials (mXml->readElementText ());
}

void
GParseStream::handleEntryJot ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:jot");

    mContactEntry->setJot (mXml->readElementText (), mXml->attributes ().value ("rel").toString ());
}

void
GParseStream::handleEntryLanguage ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:language");

    QXmlStreamAttributes attrs = mXml->attributes ();
    if (attrs.hasAttribute ("code"))
       mContactEntry->setLanguage (attrs.value ("code").toString ());
    else if (attrs.hasAttribute ("label"))
       mContactEntry->setLanguage (attrs.value ("label").toString ());
}

void
GParseStream::handleEntryMaidenName ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:maidenName");

    mContactEntry->setMaidenName (mXml->readElementText ());
}

void
GParseStream::handleEntryMileage ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:mileage");

    mContactEntry->setMileage (mXml->readElementText ());
}

void
GParseStream::handleEntryNickname ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:nickname");

    mContactEntry->setNickname (mXml->readElementText ());
}

void
GParseStream::handleEntryOccupation ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:occupation");

    mContactEntry->setOccupation (mXml->readElementText ());
}

void
GParseStream::handleEntryPriority ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:priority");

    mContactEntry->setPriority (mXml->attributes ().value ("rel").toString ());
}

void
GParseStream::handleEntryRelation ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:relation");

    QString relation;
    if (mXml->attributes ().hasAttribute ("rel"))
        relation = mXml->attributes ().value ("rel").toString ();

    mContactEntry->setRelation (mXml->readElementText (), relation);
}

void
GParseStream::handleEntrySensitivity ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:sensitivity");

    mContactEntry->setSensitivity (mXml->attributes ().value ("rel").toString ());
}

void
GParseStream::handleEntryShortname ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:shortName");

    mContactEntry->setShortname (mXml->readElementText ());
}

void
GParseStream::handleEntrySubject ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:subject");

    mContactEntry->setSubject (mXml->readElementText ());
}

void
GParseStream::handleEntrySystemGroup ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:systemGroup");

    mContactEntry->setSystemGroup (mXml->attributes ().value ("id").toString ());
}

void
GParseStream::handleEntryUserDefinedField ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:userDefinedField");

    QString key = mXml->attributes ().value ("key").toString ();
    QString value = mXml->attributes ().value ("value").toString ();
    if (key == "ButeoLocalId")
    {
        mContactEntry->setLocalId (value);
    }
}

void
GParseStream::handleEntryWebsite ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gContact:website");

    mContactEntry->setWebsite (mXml->attributes ().value ("href").toString (),
                               mXml->attributes ().value ("rel").toString ());
}

void
GParseStream::handleEntryAdditionalName ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:additionalName");

    mContactEntry->setAdditionalName (mXml->readElementText ());
}

void
GParseStream::handleEntryComments ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:comments");

    // Should the comment feed be supported? Think it is not required to
    // perform additional parsing. Store the complete feed in db. Let the interested
    // apps handle the making use of the feed part
    mContactEntry->setComments (mXml->readElementText ());
}

void
GParseStream::handleEntryDeleted ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:deleted");

    mContactEntry->setDeleted (true);
}

void
GParseStream::handleEntryCountry ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:country");

    mContactEntry->setCountry (mXml->readElementText ());
}

void
GParseStream::handleEntryEmail ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:email");

    mContactEntry->setEmail (mXml->attributes ().value ("address").toString (),
                             mXml->attributes ().value ("rel").toString (),
                             mXml->attributes ().value ("primary").toString ());
}

void
GParseStream::handleEntryExtendedProperty ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:extendedProperty");

    mContactEntry->setExtendedProperty (mXml->attributes ().value ("name").toString (),
                                        mXml->readElementText ());
}

void
GParseStream::handleEntryFeedLink ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:feedLink");

    // Store the complete feed link. Apps interested in using this field, will
    // have to handle the entry link feed content
    mContactEntry->setFeedLink (mXml->readElementText ());
}

void
GParseStream::handleEntryIm ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:im");

    QString rel, protocol, primary;
    if (mXml->attributes ().hasAttribute ("rel"))
        rel = mXml->attributes ().value ("rel").toString ();
    if (mXml->attributes ().hasAttribute ("protocol"))
    {
        QString protocolUrl = mXml->attributes ().value ("protocol").toString ();
        protocol = protocolUrl.right (protocolUrl.lastIndexOf ("#"));
    }
    if (mXml->attributes ().hasAttribute ("primary"))
        primary = mXml->attributes ().value ("primary").toString ();
    mContactEntry->setIm (mXml->attributes ().value ("address").toString (),
                          rel,
                          protocol,
                          primary);
}

void
GParseStream::handleEntryMoney ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:money");

    mContactEntry->setMoney (mXml->attributes ().value ("amount").toString (),
                             mXml->attributes ().value ("currencyCode").toString ());
}

void
GParseStream::handleEntryName ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:name");

    while (!(mXml->tokenType () == QXmlStreamReader::EndElement &&
             mXml->qualifiedName () == "gd:name"))
    {
        if (mXml->tokenType () == QXmlStreamReader::StartElement)
        {
            if (mXml->qualifiedName () == "gd:givenName")
                mContactEntry->setGivenName (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:additionalName")
                mContactEntry->setAdditionalName (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:familyName")
                mContactEntry->setFamilyName (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:namePrefix")
                mContactEntry->setNamePrefix (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:nameSuffix")
                mContactEntry->setNameSuffix (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:fullName")
                mContactEntry->setFullName (mXml->readElementText ());
        }
        mXml->readNextStartElement ();
    }
}

void
GParseStream::handleEntryOrganization ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:organization");

    while (!(mXml->tokenType () == QXmlStreamReader::EndElement &&
             mXml->qualifiedName () == "gd:organization"))
    {
        if (mXml->tokenType () == QXmlStreamReader::StartElement)
        {
            if (mXml->qualifiedName () == "gd:orgDepartment")
                mContactEntry->setOrgDepartment (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:orgJobDescription")
                mContactEntry->setOrgJobDescription (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:orgName")
                mContactEntry->setOrgName (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:orgSymbol")
                mContactEntry->setOrgSymbol (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:orgTitle")
                mContactEntry->setOrgTitle (mXml->readElementText ());
        }
        mXml->readNextStartElement ();
    }
}

void
GParseStream::handleEntryOriginalEvent ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:originalEvent");

    mContactEntry->setOriginalEvent (mXml->readElementText ());
}

void
GParseStream::handleEntryPhoneNumber ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:phoneNumber");

    QString rel, uri, primary;
    if (mXml->attributes ().hasAttribute ("rel"))
        rel = mXml->attributes ().value ("rel").toString ();
    if (mXml->attributes ().hasAttribute ("uri"))
        uri = mXml->attributes ().value ("uri").toString ();
    if (mXml->attributes ().hasAttribute ("primary"))
        primary = mXml->attributes ().value ("primary").toString ();

    mContactEntry->setPhoneNumber (mXml->readElementText (),
                                   rel, uri, primary);
}

void
GParseStream::handleEntryRating ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:rating");

    QString average, numRaters, rel, value;
    if (mXml->attributes ().hasAttribute ("average"))
       average = mXml->attributes ().value ("average").toString ();
    if (mXml->attributes ().hasAttribute ("numRaters"))
       numRaters = mXml->attributes ().value ("numRaters").toString ();
    if (mXml->attributes ().hasAttribute ("rel"))
       rel = mXml->attributes ().value ("rel").toString ();
    if (mXml->attributes ().hasAttribute ("value"))
       value = mXml->attributes ().value ("value").toString ();

    mContactEntry->setRating (mXml->attributes ().value ("max").toString (),
                              mXml->attributes ().value ("min").toString (),
                              average,
                              numRaters,
                              rel,
                              value);
}

void
GParseStream::handleEntryStructuredPostalAddress ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->qualifiedName () == "gd:structuredPostalAddress");

    QString rel, mailClass, usage, primary;
    if (mXml->attributes ().hasAttribute ("rel"))
        rel = mXml->attributes ().value ("rel").toString ();
    if (mXml->attributes ().hasAttribute ("mailClass"))
        mailClass = mXml->attributes ().value ("mailClass").toString ();
    if (mXml->attributes ().hasAttribute ("usage"))
        usage = mXml->attributes ().value ("usage").toString ();
    if (mXml->attributes ().hasAttribute ("primary"))
        primary = mXml->attributes ().value ("primary").toString ();

    mContactEntry->setPostalAddrAttrs (rel, mailClass, usage, primary);

    while (!(mXml->tokenType () == QXmlStreamReader::EndElement &&
             mXml->qualifiedName () == "gd:structuredPostalAddress"))
    {
        if (mXml->tokenType () == QXmlStreamReader::StartElement)
        {
            if (mXml->qualifiedName () == "gd:agent")
                mContactEntry->setPostalAddrAgent (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:housename")
                mContactEntry->setPostalAddrHousename (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:street")
                mContactEntry->setPostalAddrStreet (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:pobox")
                mContactEntry->setPostalAddrPobox (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:neighborhood")
                mContactEntry->setPostalAddrNeighborhood (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:city")
                mContactEntry->setPostalAddrCity (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:subregion")
                mContactEntry->setPostalAddrSubregion (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:region")
                mContactEntry->setPostalAddrRegion (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:postcode")
                mContactEntry->setPostalAddrPostCode (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:country")
                mContactEntry->setPostalAddrCountry (mXml->readElementText ());
            else if (mXml->qualifiedName () == "gd:formattedAddress")
                mContactEntry->setPostalAddrFormatted (mXml->readElementText ());
        }
        mXml->readNextStartElement ();
    }
}
