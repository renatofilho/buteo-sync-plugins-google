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

#include <QFile>
#include <QTextStream>
#include <QtTest/QtTest>
#include <QDebug>

#include <QContactName>
#include <QContactNote>
#include <QContactNickname>
#include <QContactEmailAddress>
#include <QContactBirthday>

#include <LogMacros.h>

#include <iostream>
#include "GParseStreamTest.h"
#include "GParseStream.h"
#include "GAtom.h"

GParseStreamTest::GParseStreamTest(QObject *parent) :
    QObject(parent)
{
}

void
GParseStreamTest::initTestCase()
{
    QFile file("./contact_v3.xml");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in (&file);
    QByteArray ba = in.readAll ().toAscii ();
    mPs = new GParseStream (ba);
    mPs->parse ();
    file.close ();
}

void
GParseStreamTest::testParse ()
{
    QVERIFY (mPs != NULL);
}

void
GParseStreamTest::testAtom ()
{
    mAtom = mPs->atom ();
    QVERIFY (mAtom != NULL);
    QVERIFY (mAtom->getAuthorName () == "nemo sailfish");
    QVERIFY (mAtom->getAuthorEmail () == "nemosailfish@gmail.com");
    QVERIFY (mAtom->getItemsPerPage () == 25);
    QVERIFY (mAtom->getStartIndex () == 1);
    QVERIFY (mAtom->getTotalResults () == 2);
    QVERIFY (mAtom->getTotalResults () == mAtom->entries ().size ());
}

void
GParseStreamTest::testContactEntry ()
{
    QList<GContactEntry*> gContacts = mAtom->entries ();
    QVERIFY (gContacts.size () == 2);

    QContact qContact = gContacts.at (0)->qContact ();
    std::cout << qContact.detail<QContactName> ().firstName ().toStdString() << "\n";
    std::cout << qContact.detail<QContactName> ().lastName ().toStdString() << "\n";
    LOG_DEBUG (qContact.detail<QContactName> ().firstName ());
    LOG_DEBUG (qContact.detail<QContactName> ().lastName ());
    LOG_DEBUG (qContact.detail<QContactNickname> ().nickname ());
    LOG_DEBUG (qContact.detail<QContactEmailAddress> ().emailAddress ());
    LOG_DEBUG (qContact.detail<QContactNote> ().note ());
    LOG_DEBUG (qContact.detail<QContactBirthday> ().date ().toString ());
    std::cout << qContact.detail<QContactBirthday> ().date ().toString().toStdString() << "\n";

    /*
    QVERIFY (qContact.detail<QContactName> ().firstName () == "Tarzan");
    // FIXME: For some weird reason, lastname comes up as empty
    //QVERIFY (qContact.detail<QContactName> ().lastName () == "Hello");
    QVERIFY (qContact.detail<QContactNickname> ().nickname () == "Taru");
    QVERIFY (qContact.detail<QContactEmailAddress> ().emailAddress () == "tarzan.forest@forest.com");
    QVERIFY (qContact.detail<QContactNote> ().note () == "This is a note");
    //QVERIFY (qContact.detail<QContactBirthday> ().date ().toString () == "1967-12-02");
    */

    qContact = gContacts.at (1)->qContact ();
    std::cout << qContact.detail<QContactName> ().firstName ().toStdString() << "\n";
    std::cout << qContact.detail<QContactName> ().lastName ().toStdString() << "\n";
    LOG_DEBUG (qContact.detail<QContactName> ().firstName ());
    LOG_DEBUG (qContact.detail<QContactName> ().lastName ());
    LOG_DEBUG (qContact.detail<QContactNickname> ().nickname ());
    LOG_DEBUG (qContact.detail<QContactEmailAddress> ().emailAddress ());
    LOG_DEBUG (qContact.detail<QContactNote> ().note ());
    LOG_DEBUG (qContact.detail<QContactBirthday> ().date ().toString ());
    std::cout << qContact.detail<QContactBirthday> ().date ().toString().toStdString() << "\n";
}
