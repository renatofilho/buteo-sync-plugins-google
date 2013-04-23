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

#include "ParseStream.h"

#include "Atom.h"
#include <LogMacros.h>

ParseStream::ParseStream(QObject *parent) :
    QObject(parent)
{
    initFunctionMap ();
}

ParseStream::ParseStream(QByteArray xmlStream)
{
    mXml = new QXmlStreamReader (xmlStream);
    mAtom = new Atom (false);

    initFunctionMap ();
}

void
ParseStream::initFunctionMap ()
{
    // Initialize the function map
    mFunctionMap.insert ("updated", &ParseStream::handleAtomUpdated);
    mFunctionMap.insert ("category", &ParseStream::handleAtomCategory);
    mFunctionMap.insert ("author", &ParseStream::handleAtomAuthor);
    mFunctionMap.insert ("totalResults", &ParseStream::handleOpenSearch);
    mFunctionMap.insert ("startIndex", &ParseStream::handleOpenSearch);
    mFunctionMap.insert ("itemsPerPage", &ParseStream::handleOpenSearch);
    mFunctionMap.insert ("entry", &ParseStream::parseEntry);

    mFunctionMap.insert ("gContact:billingInformation", NULL);
    mFunctionMap.insert ("gContact:birthday", NULL);
    mFunctionMap.insert ("gContact:calendarLink", NULL);
    mFunctionMap.insert ("gContact:directoryServer", NULL);
    mFunctionMap.insert ("gContact:event", NULL);
    mFunctionMap.insert ("gContact:externalId", NULL);
    mFunctionMap.insert ("gContact:gender", NULL);
    mFunctionMap.insert ("gContact:groupMembershipInfo", NULL);
    mFunctionMap.insert ("gContact:hobby", NULL);
    mFunctionMap.insert ("gContact:initials", NULL);
    mFunctionMap.insert ("gContact:jot", NULL);
    mFunctionMap.insert ("gContact:language", NULL);
    mFunctionMap.insert ("gContact:maidenName", NULL);
    mFunctionMap.insert ("gContact:mileage", NULL);
    mFunctionMap.insert ("gContact:nickname", NULL);
    mFunctionMap.insert ("gContact:occupation", NULL);
    mFunctionMap.insert ("gContact:priority", NULL);
    mFunctionMap.insert ("gContact:relation", NULL);
    mFunctionMap.insert ("gContact:sensitivity", NULL);
    mFunctionMap.insert ("gContact:shortName", NULL);
    mFunctionMap.insert ("gContact:subject", NULL);
    mFunctionMap.insert ("gContact:systemGroup", NULL);
    mFunctionMap.insert ("gContact:userDefinedField", NULL);
    mFunctionMap.insert ("gContact:website", NULL);

    mFunctionMap.insert ("gd:additionalName", NULL);
    mFunctionMap.insert ("gd:comments", NULL);
    mFunctionMap.insert ("gd:country", NULL);
    mFunctionMap.insert ("gd:deleted", NULL);
    mFunctionMap.insert ("gd:email", NULL);
    mFunctionMap.insert ("gd:entryLink", NULL);
    mFunctionMap.insert ("gd:extendedProperty", NULL);
    mFunctionMap.insert ("gd:familyName", NULL);
    mFunctionMap.insert ("gd:feedLink", NULL);
    mFunctionMap.insert ("gd:geoPt", NULL);
    mFunctionMap.insert ("gd:givenName", NULL);
    mFunctionMap.insert ("gd:im", NULL);
    mFunctionMap.insert ("gd:money", NULL);
    mFunctionMap.insert ("gd:name", NULL);
    mFunctionMap.insert ("gd:organization", NULL);
    mFunctionMap.insert ("gd:orgDepartment", NULL);
    mFunctionMap.insert ("gd:orgJobDescription", NULL);
    mFunctionMap.insert ("gd:orgName", NULL);
    mFunctionMap.insert ("gd:orgSymbol", NULL);
    mFunctionMap.insert ("gd:orgTitle", NULL);
    mFunctionMap.insert ("gd:originalEvent", NULL);
    mFunctionMap.insert ("gd:phoneNumber", NULL);
    mFunctionMap.insert ("gd:postalAddress", NULL);
    mFunctionMap.insert ("gd:rating", NULL);
    mFunctionMap.insert ("gd:resourceId", NULL);
    mFunctionMap.insert ("gd:structuredPostalAddress", NULL);

}

Atom*
ParseStream::atom ()
{
    return mAtom;
}

void
ParseStream::parse ()
{
    while (!mXml->atEnd () && !mXml->hasError ())
    {
        if (mXml->readNextStartElement ())
        {
            Handler handler = mFunctionMap.value (mXml->name ().toString ());
            if (handler)
                (*this.*handler) ();

            if (mXml->isEndElement () && mXml->name () == "feed")
            {
                LOG_DEBUG("End of reading feed");
            }
        }
    }
}

void
ParseStream::handleAtomUpdated ()
{
    mXml->readNext ();
    mAtom->setUpdated (mXml->text ().toString ());
}

void
ParseStream::handleAtomCategory ()
{
    QXmlStreamAttributes attributes = mXml->attributes ();
    for (int i=0; i<attributes.size (); i++)
    {
        QXmlStreamAttribute attr = mXml->attributes ().at (i);
        if (attr.name () == "schema")
            mAtom->setCategorySchema (attr.value ().toString ());
        else if (attr.name () == "term")
            mAtom->setCategoryTerm (attr.value ().toString ());
    }
}

void
ParseStream::handleAtomAuthor ()
{
    mXml->readNextStartElement ();
    if (mXml->name () == "name")
    {
        mAtom->setAuthorName (mXml->readElementText ());
    }
    mXml->readNextStartElement ();
    mXml->readNextStartElement ();
    if (mXml->name () == "email")
    {
        mAtom->setAuthorEmail (mXml->readElementText ());
    }

}

void
ParseStream::handleOpenSearch ()
{
    if (mXml->name() == "totalResults")
        mAtom->setTotalResults (mXml->readElementText ().toInt ());
    else if (mXml->name() == "startIndex")
        mAtom->setStartIndex (mXml->readElementText ().toInt ());
    else if (mXml->name () == "itemsPerPage")
        mAtom->setItemsPerPage (mXml->readElementText ().toInt ());
}

void
ParseStream::parseEntry ()
{
    Q_ASSERT(mXml->isStartElement () && mXml->name () == "entry");

    GContactEntry* contact = new GContactEntry(false);
}
