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
    QTextStream in(&file);
    QByteArray ba = in.readAll ().toAscii ();
    mPs = new GParseStream(ba);
    mPs->parse ();
}

void
GParseStreamTest::testParse ()
{
    QVERIFY (mPs != NULL);
}

void
GParseStreamTest::testAtom ()
{
    GAtom* atom = mPs->atom ();
    QVERIFY (atom->getAuthorName () == "nemo sailfish");
    QVERIFY (atom->getAuthorEmail () == "nemosailfish@gmail.com");
    QVERIFY (atom->getItemsPerPage () == 25);
    QVERIFY (atom->getStartIndex () == 1);
    QVERIFY (atom->getTotalResults () == 2);
    QVERIFY (atom->getTotalResults () == atom->entries ()->size ());
}
