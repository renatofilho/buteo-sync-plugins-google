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

#ifndef ATOM_H
#define ATOM_H

#include "atom_global.h"

#include <QMetaEnum>
#include <QMap>
#include <QList>
#include <QXmlStreamWriter>

class GContactEntry;

class GAtom
{

public:
    GAtom ();

    typedef enum
    {
        text,
        html,
        xhtml
    } TYPE;

    void setAuthorName(QString authorName);
    QString authorName();

    void setAuthorEmail(QString authorEmail);
    QString authorEmail();

    void setId(const QString id);
    QString id();

    void setUpdated(QString updated);
    QString updated();

    void setCategory (const QString schema = "http://schemas.google.com/g/2005#kind",
                      const QString term = "http://schemas.google.com/contact/2008#contact");

    void setTitle(QString title);
    QString title();

    void setContent (const QString note, const QString type="text");

    void setGenerator(QString name = "Contacts",
                      QString version = "1.0",
                      QString uri = "http://sailfish.org");
    QString generatorName();
    QString generatorVersion();
    QString generatorUri();

    void setTotalResults(int totalResults);
    int totalResults();

    void setStartIndex(int startIndex);
    int startIndex();

    void setItemsPerPage(int itemsPerPage);
    int itemsPerPage();

    void addEntry(GContactEntry* entry);

    QList<GContactEntry*> entries();

    void setNextEntriesUrl (const QString nextUrl);

    const QString nextEntriesUrl ();

    const QList<GContactEntry*> responseErrorEntries ();

private:

    QString mAuthorEmail;

    QString mAuthorName;

    QString mCategory;

    QString mCategoryTerm;

    QString mSchema;

    QString mContributor;

    QString mGeneratorName;

    QString mGeneratorVersion;

    QString mGeneratorUri;

    QString mIcon;

    QString mId;

    QString mLink;

    QString mLogo;

    QString mRights;

    QString mSubtitle;

    QString mTitle;

    QString mUpdated;

    int mTotalResults;

    int mStartIndex;

    int mItemsPerPage;

    QList<GContactEntry*> mContactList;

    QString mNextEntriesUrl;
};

#endif // ATOM_H
