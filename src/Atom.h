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

#include <QObject>
#include <QMetaObject>
#include <QMetaEnum>
#include <QMap>

#include "GContactEntry.h"

class ATOMSHARED_EXPORT Atom : public QObject
{
    Q_OBJECT

public:
    Atom();

    typedef enum
    {
        feed,
        id,
        updated,
        category,
        title,
        link,
        author,
        contributor,
        generator,
        logo,
        icon,
        rights,
        subtitel,
        entry,
        totalResults,
        startIndex,
        itemsPerPage
    } AtomFeed;

    typedef enum
    {
        text,
        html,
        xhtml
    } TYPE;

    Q_ENUMS (AtomFeed)

    void setAuthor(const QString authorName, const QString authorEmail);

    QString getAuthorName();
    QString getAuthorEmail();

    void setId(const QString id);

    QString getId();

    void setUpdated(QString updated);

    QString getUpdated();

    void setCategory(QString schema = "http://schemas.google.com/g/2005#kind",
                     QString term = "http://schemas.google.com/contact/2008#contact",
                     QString label = "");

    QString getCategorySchema();
    QString getCategoryTerm();
    QString getCategoryLabel();

    void setTitle(QString title, TYPE type = text);

    QString getTitle();

    void setGenerator(QString name = "Contacts",
                      QString version = "1.0",
                      QString uri = "http://sailfish.org");

    QString getGeneratorName();
    QString getGeneratorVersion();
    QString getGeneratorUri();

    QString getValue(AtomFeed key);

    void setTotalResults(int totalResults);
    int getTotalResults();

    void setStartIndex(int startIndex);
    int getStartIndex();

    void setItemsPerPage(int itemsPerPage);
    int getItemsPerPage();

    QString toString();

    void addEntry(GContactEntry* entry);

    QString enumToString(AtomFeed element);

    AtomFeed stringToEnum(QString element);

private:

    QMetaEnum mMetaEnum;

    QString mAuthor;

    QString mCategory;

    QString mContributor;

    QString mGenerator;

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

    GContactEntry* mContact;
};

#endif // ATOM_H
