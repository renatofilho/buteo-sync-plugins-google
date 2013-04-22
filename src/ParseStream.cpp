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
    initFunctionMap ();
    mAtom = new Atom ();
}

void
ParseStream::initFunctionMap ()
{
    // Initialize the function map
    mFunctionMap.insert (Atom::updated, &ParseStream::handleAtomUpdated);
    mFunctionMap.insert (Atom::category, &ParseStream::handleAtomCategory);
    mFunctionMap.insert (Atom::author, &ParseStream::handleAtomAuthor);
    mFunctionMap.insert (Atom::totalResults, &ParseStream::handleOpenSearch);
    mFunctionMap.insert (Atom::startIndex, &ParseStream::handleOpenSearch);
    mFunctionMap.insert (Atom::itemsPerPage, &ParseStream::handleOpenSearch);
    mFunctionMap.insert (Atom::entry, &ParseStream::handleEntry);
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
            Handler handler = mFunctionMap.value (
                    mAtom->stringToEnum ( mXml->name ().toString ()));
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
    QString schema, term;
    for (int i=0; i<attributes.size (); i++)
    {
        QXmlStreamAttribute attr = mXml->attributes ().at (i);
        if (attr.name () == "schema")
           schema = attr.value ().toString ();
        else if (attr.name () == "term")
            term = attr.value ().toString ();
    }
    mAtom->setCategory (schema, term);
}

void
ParseStream::handleAtomAuthor ()
{
    mXml->readNextStartElement ();
    QString authorName, authorEmail;
    if (mXml->name () == "name")
    {
        authorName = mXml->readElementText ();
    }
    mXml->readNextStartElement ();
    mXml->readNextStartElement ();
    if (mXml->name () == "email")
    {
        authorEmail = mXml->readElementText ();
    }

    mAtom->setAuthor (authorName, authorEmail);
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
ParseStream::handleEntry ()
{
}
