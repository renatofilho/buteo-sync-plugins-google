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

GAuth::GAuth(const Buteo::SyncProfile &aProfile, QObject *parent) :
    QObject(parent)
{
    m_syncProfile = aProfile.clone();
    quint32 accountId = 1;
    QStringList accountList = aProfile.keyValues(Buteo::KEY_ACCOUNT_ID);
    if (!accountList.isEmpty()) {
        QString aId = accountList.first();
        if (aId != NULL) {
            accountId = aId.toInt();
        }
    }

    Manager *manager = new Manager();
    m_account = manager->account(accountId);
    if (m_account == NULL) {
        qDebug() << "Account is not created... Cannot authenticate";
        return;
    }

    QVariant val = QVariant::String;
    m_account->value(AUTH + SLASH + AUTH_METHOD, val);
    QString method = val.toString();
    m_account->value(AUTH + SLASH + MECHANISM, val);
    QString mechanism = val.toString();

    qint32 cId = m_account->credentialsId();
    qDebug() << "Got Credentials ID = " << cId;
    if (cId == 0) {
        QMap<MethodName,MechanismsList> methods;
        methods.insert(method, QStringList()  << mechanism);
        IdentityInfo *info = new IdentityInfo(m_syncProfile->displayname(), "", methods);
        info->setRealms(QStringList() << HOST);
        info->setType(IdentityInfo::Application);

        m_identity = Identity::newIdentity(*info);

        connect(m_identity, SIGNAL(credentialsStored(const quint32)),
                this, SLOT(credentialsStored(const quint32)));
        connect(m_identity, SIGNAL(error(const SignOn::Error &)),
                this, SLOT(error(const SignOn::Error &)));

        m_identity->storeCredentials();
    } else {
        m_identity = Identity::existingIdentity(cId);
    }

    m_session = m_identity->createSession(QLatin1String("oauth2"));

    connect(m_session, SIGNAL(response(const SignOn::SessionData &)),
            this, SLOT(sessionResponse(const SignOn::SessionData &)));

    connect(m_session, SIGNAL(error(const SignOn::Error &)),
            this, SLOT(error(const SignOn::Error &)));
}

void GAuth::sessionResponse(const SessionData &sessionData) {
    OAuth2PluginNS::OAuth2PluginTokenData response = sessionData.data<OAuth2PluginNS::OAuth2PluginTokenData>();
    m_token = response.AccessToken();
    qDebug() << "Authenticated !!!";

    emit success();
}

const QString GAuth::token()
{
    qDebug() << "Returning token.........." << m_token;
    if (m_token.isEmpty()) {
        authenticate();
    }

    return m_token;
}

void GAuth::authenticate()
{
    QVariant val = QVariant::String;
    m_account->value(AUTH + SLASH + AUTH_METHOD, val);
    QString method = val.toString();
    m_account->value(AUTH + SLASH + MECHANISM, val);
    QString mechanism = val.toString();

    m_account->value(AUTH + SLASH + method + SLASH + mechanism + SLASH + HOST, val);
    QString host = val.toString();
    m_account->value(AUTH + SLASH + method + SLASH + mechanism + SLASH + AUTH_PATH, val);
    QString auth_url = val.toString();
    m_account->value(AUTH + SLASH + method + SLASH + mechanism + SLASH + TOKEN_PATH, val);
    QString token_url = val.toString();
    m_account->value(AUTH + SLASH + method + SLASH + mechanism + SLASH + REDIRECT_URI, val);
    QString redirect_uri = val.toString();
    m_account->value(AUTH + SLASH + method + SLASH + mechanism + SLASH + RESPONSE_TYPE, val);
    QString response_type = val.toString();

    QVariant val1 = QVariant::StringList;
    m_account->value(AUTH + SLASH + method + SLASH + mechanism + SLASH + SCOPE, val1);
    QStringList scope = val1.toStringList();

    OAuth2PluginNS::OAuth2PluginData data;
    data.setClientId(CLIENT_ID);
    data.setClientSecret(CLIENT_SECRET);
    data.setHost(host);
    data.setAuthPath(auth_url);
    data.setTokenPath(token_url);
    data.setRedirectUri(redirect_uri);
    data.setResponseType(QStringList() << response_type);
    data.setScope(scope);

    m_session->process(data, mechanism);
}

void GAuth::credentialsStored(const quint32 id) {
    m_account->setCredentialsId(id);
    m_account->sync();
    qDebug() << "Credentials Stored.........." + id;
}

void GAuth::error(const SignOn::Error & error) {
    printf(error.message().toStdString().c_str());
    qDebug() << error.message();

    emit failed();
}
