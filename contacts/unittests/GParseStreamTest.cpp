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
#include "iostream"

#include <QContactName>
#include <QContactNote>
#include <QContactNickname>
#include <QContactEmailAddress>
#include <QContactBirthday>
#include <QDebug>

#include "GParseStreamTest.h"
#include "GParseStream.h"
#include "GAtom.h"
#include "GContactEntry.h"

QTM_USE_NAMESPACE
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
    mPs = new GParseStream (false);
    mAtom = mPs->parse (ba);
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
    QVERIFY (mAtom != NULL);
    QVERIFY (mAtom->authorName () == "nemo sailfish");
    QVERIFY (mAtom->authorEmail () == "nemosailfish@gmail.com");
    QVERIFY (mAtom->itemsPerPage () == 25);
    QVERIFY (mAtom->startIndex () == 1);
    QVERIFY (mAtom->totalResults () == 2);
    QVERIFY (mAtom->totalResults () == mAtom->entries ().size ());
}

void
GParseStreamTest::testContactEntry ()
{
    QList<GContactEntry*> gContacts = mAtom->entries ();
    QVERIFY (gContacts.size () == 2);

    QContact qContact = gContacts.at (0)->qContact ();
    qDebug () << qContact.detail<QContactName> ().firstName ();
    qDebug () << qContact.detail<QContactName> ().lastName ();
    qDebug () << qContact.detail<QContactNickname> ().nickname ();
    qDebug () << qContact.detail<QContactEmailAddress> ().emailAddress ();
    qDebug () << qContact.detail<QContactNote> ().note ();
    qDebug () << qContact.detail<QContactBirthday> ().date ().toString ();

    std::cout << qContact.detail<QContactName> ().firstName ().toStdString () << "\n";
    std::cout << qContact.detail<QContactName> ().lastName ().toStdString () << "\n";
    std::cout << qContact.detail<QContactNickname> ().nickname ().toStdString () << "\n";
    std::cout << qContact.detail<QContactEmailAddress> ().emailAddress ().toStdString () << "\n";
    std::cout << qContact.detail<QContactNote> ().note ().toStdString () << "\n";
    std::cout << qContact.detail<QContactBirthday> ().date ().toString ("yyyy-MM-dd").toStdString () << "\n";

    QVERIFY (qContact.detail<QContactName> ().firstName () == "Tarzan");
    QVERIFY (qContact.detail<QContactName> ().lastName () == "Hello");
    QVERIFY (qContact.detail<QContactNickname> ().nickname () == "Taru");
    QVERIFY (qContact.detail<QContactEmailAddress> ().emailAddress () == "tarzan.forest@forest.com");
    QVERIFY (qContact.detail<QContactNote> ().note () == "This is a note");
    QVERIFY (qContact.detail<QContactBirthday> ().date ().toString ("yyyy-MM-dd") == "1967-12-02");

    std::cout << "---------" << "\n";
    qContact = gContacts.at (1)->qContact ();
    /*
    qDebug () << qContact.detail<QContactName> ().firstName ();
    qDebug () << qContact.detail<QContactName> ().lastName ();
    qDebug () << qContact.detail<QContactNickname> ().nickname ();
    qDebug () << qContact.detail<QContactEmailAddress> ().emailAddress ();
    qDebug () << qContact.detail<QContactNote> ().note ();
    qDebug () << qContact.detail<QContactBirthday> ().date ().toString ();
    */
    std::cout << qContact.detail<QContactName> ().firstName ().toStdString () << "\n";
    std::cout << qContact.detail<QContactName> ().lastName ().toStdString () << "\n";
    std::cout << qContact.detail<QContactNickname> ().nickname ().toStdString () << "\n";
    std::cout << qContact.detail<QContactEmailAddress> ().emailAddress ().toStdString () << "\n";
    std::cout << qContact.detail<QContactNote> ().note ().toStdString () << "\n";
}
