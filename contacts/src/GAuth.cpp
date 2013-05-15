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

#include "GAuth.h"
#include "GTransport.h"
#include <qjson/parser.h>
#include <QVariantMap>
#include <LogMacros.h>

#include <QTextStream>
#include <QFile>

/* Values obtained after registering with Google */
const QString CODE_TAG            	("code");
const QString CLIENT_ID_TAG	        ("client_id");
const QString CLIENT_ID	            ("340286938476.apps.googleusercontent.com");
const QString SCOPE_TAG             ("scope");
const QString CONTACTS_SCOPE_URL	("https://www.google.com/m8/feeds/");
const QString CLIENT_SECRET_TAG		("client_secret");
const QString CLIENT_SECRET			("cE6huV6DyPQCKXo5AOg5Balm");
const QString GRANT_TYPE_TAG		("grant_type");
const QString GRANT_TYPE			("http://oauth.net/grant_type/device/1.0");
const QString AUTH_URL				("https://accounts.google.com/o/oauth2/auth");
const QString TOKEN_URI				("https://accounts.google.com/o/oauth2/token");
const QString OAUTH_DEVICE_CODE_URL ("https://accounts.google.com/o/oauth2/device/code");
const QString USER_INFO_PROFILE		("https://www.googleapis.com/auth/userinfo.profile");
const QString AND					("&");
const QString EQUALS				("=");

GAuth::GAuth(QObject *parent) :
    QObject(parent)
{
}

const QString GAuth::token()
{
    LOG_DEBUG("Token:" << iToken);
    // FIXME: Read the token from file until accounts&sso
    // integration is done
    QFile file ("/tmp/access_token.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in (&file);
    iToken = in.readAll ();
    file.close ();
    return iToken;
}

void GAuth::authenticate()
{
    deviceAuth();
    getToken();
}

void GAuth::getToken()
{
    if (iDeviceCode == "")
    {
        // FIXME: Error condition, emit an error signal
    }
    const QString data = CLIENT_ID_TAG + EQUALS + CLIENT_ID + AND +
                         CLIENT_SECRET_TAG + EQUALS + CLIENT_SECRET + AND +
                         CODE_TAG + EQUALS + iDeviceCode + AND +
                         GRANT_TYPE_TAG + EQUALS + GRANT_TYPE;

    if (iTransport != NULL)
    {
        delete iTransport;
        iTransport = NULL;
    }
    iTransport = new GTransport(TOKEN_URI, data.toAscii(), NULL);
    QObject::connect(iTransport, SIGNAL(finishedRequest()),
                     this, SLOT(tokenResponse()));
    iTransport->request(GTransport::POST);
}

void GAuth::tokenResponse()
{
    if (iTransport != NULL)
    {
        const QNetworkReply* reply = iTransport->reply();
        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray body = iTransport->replyBody();
            processTokenResponse(body);
        }
    }
}

void GAuth::processTokenResponse(const QByteArray tokenJSON)
{
    /*
    {
      "access_token" : "ya29.AHES6ZRwtsotfOhe3Vu21XM6kCi7R4r-P1Fq19uvtVIaQ50", => this is used for auth header
      "token_type" : "Bearer",
      "expires_in" : 3600,
      "id_token" : "eyJhbGciOiJSUzI1NiIsImtpZCI6ImQ3NzljOTIzZGJkNGNmN2Y4NTc3Y2EyMGIwODFkODEwOTQ1YzMxZDEifQ.eyJpc3MiOiJhY2NvdW50cy5nb29nbGUuY29tIiwiYXVkIjoiMzQwMjg2OTM4NDc2LmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwidG9rZW5faGFzaCI6IlJJcC03b21rZnRCUFZVU0tTbnAyTVEiLCJhdF9oYXNoIjoiUklwLTdvbWtmdEJQVlVTS1NucDJNUSIsImNpZCI6IjM0MDI4NjkzODQ3Ni5hcHBzLmdvb2dsZXVzZXJjb250ZW50LmNvbSIsImF6cCI6IjM0MDI4NjkzODQ3Ni5hcHBzLmdvb2dsZXVzZXJjb250ZW50LmNvbSIsImlkIjoiMTEwMzQ0MzcxNzQyMDg3OTk4MjIwIiwic3ViIjoiMTEwMzQ0MzcxNzQyMDg3OTk4MjIwIiwiaWF0IjoxMzY1NzAwODIzLCJleHAiOjEzNjU3MDQ3MjN9.Z3Bk4W-9q1aHOxQLKxdAjWf9jTYLAcD9uGEmW73FAjdWKNLufw0LCji3SPtSyJg3uMQTueievTp3xjPcVjfKzfokEq3fjUFn5bets7MLZ7Vr8Le6Yb0mhGcO0kefImfcufuVEXt9I-W-1OddCzyj4YfrYxtJ-eYRwXGOOA6dzZM",
      "refresh_token" : "1/I-IcRJHdgy16BZZxSyPYF31OUGIfjLSLSBksU3eJT3Q"
    }
    */

    /*
    QJson::Parser parser;
    bool ok;
    QVariantMap result = parser.parse(tokenJSON, &ok).toMap();
    if (ok)
        iToken = result["access_token"].toString();
    else
        iToken = QString("");
        */
}

void GAuth::deviceAuth()
{
    const QString data = CLIENT_ID_TAG + EQUALS + CLIENT_ID + AND +
                         SCOPE_TAG + EQUALS + CONTACTS_SCOPE_URL + " " + USER_INFO_PROFILE;

    // FIXME: Create a proper GTransport object
    iTransport = new GTransport(OAUTH_DEVICE_CODE_URL, data.toAscii(), NULL);

    QObject::connect(iTransport, SIGNAL(finishedRequest()),
                     this, SLOT(deviceCodeResponse()));

    iTransport->request(GTransport::POST);
}

void GAuth::deviceCodeResponse()
{
    if (iTransport != NULL)
    {
        const QNetworkReply* reply = iTransport->reply();
        if (reply->error() == QNetworkReply::NoError)
        {
            QString body = iTransport->replyBody();
            processDeviceCode(body.toAscii());
        }
    }
}

void GAuth::processDeviceCode(const QByteArray deviceCodeJSON)
{
    /*
     Parse the json output and store the values
     {
       "device_code" : "4/OPkob1JigTUheZ72hm4fEmcHXy3E", => this will be used to get the token
       "user_code" : "wkwnjjvj",
       "verification_url" : "http://www.google.com/device",
       "expires_in" : 1800,
       "interval" : 5
     }
    */

    /*
    QJson::Parser parser;
    bool ok;

    QVariantMap result = parser.parse(deviceCodeJSON, &ok).toMap();
    if (ok)
        iDeviceCode = result["device_code"].toString();
    else
        iDeviceCode = QString("").toAscii();

    LOG_DEBUG("Verification url:" << result["verification_url"].toString() <<
              ", user_code:" << result["user_code"].toString() <<
              ",device_code" << result["device_code"].toString());
    */
}
