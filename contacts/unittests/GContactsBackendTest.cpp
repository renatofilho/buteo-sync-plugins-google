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

#include "GContactsBackendTest.h"
#include "GContactsBackend.h"
#include <QDateTime>
#include <QHash>
#include <QDebug>
#include <iostream>

GContactsBackendTest::GContactsBackendTest(QObject *parent) :
    QObject(parent)
{
}

void
GContactsBackendTest::testAllLocalContactIds ()
{
    GContactsBackend* contactsBackend = new GContactsBackend ();
    contactsBackend->init();
    QList<QContactId> localIds = contactsBackend->getAllContactIds ();
    std::cout << "What is this" << localIds.size () << std::endl;
    for (int i=0; i<localIds.size (); i++)
        std::cout << localIds.at(i).toString().toStdString() << "\n";
}

void
GContactsBackendTest::testLocalIdsGivenGuids ()
{
    QStringList guids;
    guids << "4bd9135e89e4e925" << "6ba8cd628dea9945";
    GContactsBackend cb;
    QStringList localIds = cb.localIds (guids);
    foreach (QString localId, localIds) {
        std::cout << localId.toStdString() << "\n";
    }
}

void GContactsBackendTest::testgetAllNewContactIds() {
    GContactsBackend cb;
    QDateTime datetime = QDateTime::fromString("2013-09-16T10:00:19", Qt::ISODate);
    QHash<QString, QContactId> newContactHash = cb.getAllNewContactIds(datetime);
    std::cout << "Total Contacts added newly = " << newContactHash.count() << "   Search Time : " << datetime.toString(Qt::ISODate).toStdString() << "\n";
    foreach (QContactId id , newContactHash.values()) {
        std::cout << id.toString().toStdString() << "\n";
    }
}

void GContactsBackendTest::testgetAllDeletedContactIds() {
    GContactsBackend cb;
    QDateTime datetime = QDateTime::fromString("2013-09-16T10:00:19", Qt::ISODate);
    QHash<QString, QContactId> newContactHash = cb.getAllDeletedContactIds(datetime);
    std::cout << "Total Contacts Deleted = " << newContactHash.count() << "   Search Time : " << datetime.toString(Qt::ISODate).toStdString() << "\n";
    foreach (QContactId id , newContactHash.values()) {
        std::cout << id.toString().toStdString() << "\n";
    }
}

void GContactsBackendTest::testgetAllModifiedContactIds() {
    GContactsBackend cb;
    QDateTime datetime = QDateTime::fromString("2013-09-16T10:00:19", Qt::ISODate);
    QHash<QString, QContactId> newContactHash = cb.getAllModifiedContactIds(datetime);
    std::cout << "Total Contacts Modified = " << newContactHash.count() << "   Search Time : " << datetime.toString(Qt::ISODate).toStdString() << "\n";
    foreach (QContactId id , newContactHash.values()) {
        std::cout << id.toString().toStdString() << "\n";
    }
}
