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
#include <QVariantMap>
#include <LogMacros.h>

#include <QTextStream>
#include <QFile>
#include <QStringList>
#include <QDebug>

#include <Accounts/Manager>

#include <oauth2data.h>

#include <buteosyncfw/ProfileEngineDefs.h>

using namespace Accounts;
using namespace SignOn;

/* Values obtained after registering with Google */
const QString CLIENT_ID	            ("340286938476.apps.googleusercontent.com");
const QString CLIENT_SECRET			("cE6huV6DyPQCKXo5AOg5Balm");

const QString RESPONSE_TYPE         ("ResponseType");
const QString SCOPE             	("Scope");
const QString AUTH_PATH				("AuthPath");
const QString TOKEN_PATH			("TokenPath");
const QString REDIRECT_URI          ("RedirectUri");
const QString HOST                  ("Host");
const QString SLASH                 ("/");
const QString AUTH                  ("auth");
const QString AUTH_METHOD           ("method");
const QString MECHANISM             ("mechanism");

GAuth::GAuth(const quint32 accountId, const QString scope, QObject *parent) :
    QObject(parent)
{
    Manager *manager = new Manager();
    mAccount = manager->account(accountId);
    mScope = scope;
}

bool GAuth::init() {
    if (mAccount == NULL) {
        LOG_DEBUG("Account is not created... Cannot authenticate");
        return false;
    }

    QVariant val = QVariant::String;
    mAccount->value(AUTH + SLASH + AUTH_METHOD, val);
    QString method = val.toString();
    mAccount->value(AUTH + SLASH + MECHANISM, val);
    QString mechanism = val.toString();

    qint32 cId = mAccount->credentialsId();
    //LOG_DEBUG("Got Credentials ID = " + cId);
    if (cId == 0) {
        QMap<MethodName,MechanismsList> methods;
        methods.insert(method, QStringList()  << mechanism);
        IdentityInfo *info = new IdentityInfo(mAccount->displayName(), "", methods);
        info->setRealms(QStringList() << HOST);
        info->setType(IdentityInfo::Application);

        mIdentity = Identity::newIdentity(*info);
        connect(mIdentity, SIGNAL(credentialsStored(const quint32)),
                this, SLOT(credentialsStored(const quint32)));
        connect(mIdentity, SIGNAL(error(const SignOn::Error &)),
                this, SLOT(error(const SignOn::Error &)));

        mIdentity->storeCredentials();
    } else {
        mIdentity = Identity::existingIdentity(cId);
    }

    mSession = mIdentity->createSession(QLatin1String("oauth2"));

    connect(mSession, SIGNAL(response(const SignOn::SessionData &)),
            this, SLOT(sessionResponse(const SignOn::SessionData &)));

    connect(mSession, SIGNAL(error(const SignOn::Error &)),
            this, SLOT(error(const SignOn::Error &)));

    return true;
}

void GAuth::sessionResponse(const SessionData &sessionData) {
    OAuth2PluginNS::OAuth2PluginTokenData response = sessionData.data<OAuth2PluginNS::OAuth2PluginTokenData>();
    mToken = response.AccessToken();
    LOG_DEBUG("Authenticated !!!");

    emit success();
}

const QString GAuth::token()
{
    //LOG_DEBUG("Returning token.........." + mToken);
    if (mToken.isEmpty()) {
        authenticate();
    }

    return mToken;
}

void GAuth::authenticate()
{
    QVariant val = QVariant::String;
    mAccount->value(AUTH + SLASH + AUTH_METHOD, val);
    QString method = val.toString();
    mAccount->value(AUTH + SLASH + MECHANISM, val);
    QString mechanism = val.toString();

    mAccount->value(AUTH + SLASH + method + SLASH + mechanism + SLASH + HOST, val);
    QString host = val.toString();
    mAccount->value(AUTH + SLASH + method + SLASH + mechanism + SLASH + AUTH_PATH, val);
    QString auth_url = val.toString();
    mAccount->value(AUTH + SLASH + method + SLASH + mechanism + SLASH + TOKEN_PATH, val);
    QString token_url = val.toString();
    mAccount->value(AUTH + SLASH + method + SLASH + mechanism + SLASH + REDIRECT_URI, val);
    QString redirect_uri = val.toString();
    mAccount->value(AUTH + SLASH + method + SLASH + mechanism + SLASH + RESPONSE_TYPE, val);
    QString response_type = val.toString();

    QStringList scope;
    if (mScope.isEmpty()) {
        QVariant val1 = QVariant::StringList;
        mAccount->value(AUTH + SLASH + method + SLASH + mechanism + SLASH + SCOPE, val1);
        scope.append(val1.toStringList());
    } else {
        scope.append(mScope);
    }

    OAuth2PluginNS::OAuth2PluginData data;
    data.setClientId(CLIENT_ID);
    data.setClientSecret(CLIENT_SECRET);
    data.setHost(host);
    data.setAuthPath(auth_url);
    data.setTokenPath(token_url);
    data.setRedirectUri(redirect_uri);
    data.setResponseType(QStringList() << response_type);
    data.setScope(scope);

    mSession->process(data, mechanism);
}

void GAuth::credentialsStored(const quint32 id) {
    mAccount->setCredentialsId(id);
    mAccount->sync();
    //LOG_DEBUG("Credentials Stored.........." + id);
}

void GAuth::error(const SignOn::Error & error) {
    printf(error.message().toStdString().c_str());
    qDebug() << error.message();

    emit failed();
}
