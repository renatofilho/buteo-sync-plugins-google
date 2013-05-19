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
#include "GAtom.h"
#include <LogMacros.h>

GAtom::GAtom()
{
    FUNCTION_CALL_TRACE;
}

void
GAtom::setAuthorEmail (QString authorEmail)
{
    FUNCTION_CALL_TRACE;

    mAuthorEmail = authorEmail;
}

QString
GAtom::getAuthorEmail()
{
    return mAuthorEmail;
}

void
GAtom::setAuthorName (QString authorName)
{
    mAuthorName = authorName;
}

QString
GAtom::getAuthorName()
{
    return mAuthorName;
}

void
GAtom::setId(const QString id)
{
    mId = id;
}

QString
GAtom::getId()
{
    return mId;
}

void
GAtom::setUpdated(QString updated)
{
    mUpdated = updated;
}

QString
GAtom::getUpdated()
{
    return mUpdated;
}

void
GAtom::setCategory (const QString schema, const QString term)
{
}

void
GAtom::setTitle(QString title)
{
    mTitle = title;
}

QString
GAtom::getTitle()
{
    return mTitle;
}

void
GAtom::setGenerator(QString name,
                  QString version,
                  QString uri)
{
    mGeneratorName = name;
    mGeneratorVersion = version;
    mGeneratorUri = uri;
}

void
GAtom::setContent (const QString note, const QString type)
{
}

QString
GAtom::getGeneratorName()
{
    return mGeneratorName;
}

QString
GAtom::getGeneratorVersion()
{
    return mGeneratorVersion;
}

QString
GAtom::getGeneratorUri()
{
    return mGeneratorUri;
}

void
GAtom::setTotalResults(int totalResults)
{
    mTotalResults = totalResults;
}

int
GAtom::getTotalResults()
{
    return mTotalResults;
}

void
GAtom::setStartIndex(int startIndex)
{
    mStartIndex = startIndex;
}

int
GAtom::getStartIndex()
{
    return mStartIndex;
}

void
GAtom::setItemsPerPage(int itemsPerPage)
{
    mItemsPerPage = itemsPerPage;
}

int
GAtom::getItemsPerPage()
{
    return mItemsPerPage;
}

void
GAtom::addEntry (GContactEntry *entry)
{
    mContactList.append (entry);
}

QList<GContactEntry*>
GAtom::entries()
{
    return mContactList;
}
