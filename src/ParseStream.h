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

#ifndef PARSESTREAM_H
#define PARSESTREAM_H

#include <QObject>

#include <QXmlStreamReader>
#include <QMap>

#include "Atom.h"

class ParseStream : public QObject
{
    Q_OBJECT
public:
    explicit ParseStream(QObject *parent = 0);

    explicit ParseStream(QByteArray xmlStream);

    void parse();

    Atom* atom();

signals:

public slots:

private:

    void initFunctionMap();

    void handleAtomUpdated();

    void handleAtomCategory();

    void handleAtomAuthor();

    void handleOpenSearch();

    void parseEntry();

    void handleEntryId();

    void handleEntryName();

    void handleEntryNickName();

    void handleEntryBirthday();

    void handleEntryOrganization();

    void handleEntryEmail();

    void handleEntryIm();

    void handleEntryPhoneNumber();

    void handleEntryPostalAddress();

    void handleEntryRelation();

    void handleEntryWebsite();

    void handleEntryBillingInformation();

    void handleEntryCalendarLink();

    QXmlStreamReader* mXml;

    typedef void (ParseStream::*Handler)();

    QMap<QString, ParseStream::Handler> mFunctionMap;

    Atom* mAtom;
};

#endif // PARSESTREAM_H
