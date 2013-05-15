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

#include "GTransportTest.h"

#include "GTransport.h"
#include <QDebug>
#include <QPair>
#include <QList>

GTransportTest::GTransportTest(QObject *parent) :
    QObject(parent)
{
}

void
GTransportTest::initTestCase ()
{
}

void
GTransportTest::testGET()
{
    QByteArray data = "client_id=340286938476.apps.googleusercontent.com&scope=https://www.google.com/m8/feeds/ https://www.googleapis.com/auth/userinfo.profile";
    QString url = "https://accounts.google.com/o/oauth2/device/code";

    mTransport = new GTransport (url, data, NULL);

    connect(mTransport, SIGNAL(finishedRequest()),
            this,  SLOT(processResponse()));
    mTransport->request (GTransport::POST);
}

void
GTransportTest::testGetToken()
{
    QByteArray data = "client_id=340286938476.apps.googleusercontent.com&client_secret=cE6huV6DyPQCKXo5AOg5Balm&grant_type=http://oauth.net/grant_type/device/1.0&code=4/uEQy2Ew1ar8x0v3w_EYdMF8_JKlB";
    QString url = "https://accounts.google.com/o/oauth2/token";

    mTransport = new GTransport (url, data, NULL);

    connect(mTransport, SIGNAL(finishedRequest()),
            this,  SLOT(processResponse()));
    mTransport->request (GTransport::POST);
}

void
GTransportTest::testGetContacts ()
{
    QList<QPair<QByteArray, QByteArray> > headers;
    QByteArray gdataVersion("GData-Version");
    QByteArray gdataVersionNo("3.0");
    headers.append (qMakePair(gdataVersion, gdataVersionNo));
    QByteArray authFirst("Authorization");
    QByteArray authSecond("Bearer ya29.AHES6ZQSBqQ4XZmVGbGEn0xVMe4783Sr0WlRDX7ZDrLEL7kRn9UmmA");
    headers.append (qMakePair(authFirst, authSecond));
    QString url = "https://www.google.com/m8/feeds/contacts/default/full";

    mTransport = new GTransport (url, &headers);
    connect(mTransport, SIGNAL(finishedRequest()),
            this,  SLOT(processResponse()));
    mTransport->request (GTransport::GET);
}

void
GTransportTest::processResponse ()
{
    if (mTransport)
    {
        const QNetworkReply* reply = mTransport->reply ();
        if (reply)
        {
            const QList<QByteArray> headerList = reply->rawHeaderList ();
            for (int i=0; i<headerList.size (); i++)
            {
                qDebug() << headerList[i] << ":" << reply->rawHeader (headerList[i]);
            }
            qDebug() << mTransport->replyBody ();
        }
    }
}

void
GTransportTest::cleanupTestCase ()
{
}
