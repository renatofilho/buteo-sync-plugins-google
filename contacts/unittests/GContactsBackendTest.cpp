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
    QList<QContactLocalId> localIds = contactsBackend->getAllContactIds ();
    std::cout << "What is this" << localIds.size ();
    for (int i=0; i<localIds.size (); i++)
        std::cout << localIds.at(i) << "\n";
}

void
GContactsBackendTest::testLocalIdsGivenGuids ()
{
    QStringList guids;
    guids << "4bd9135e89e4e925" << "6ba8cd628dea9945";
    GContactsBackend cb;
    QStringList localIds = cb.localIds (guids);
    foreach (QString localId, localIds) {
        std::cout << localId.toStdString () << "\n";
    }
}
