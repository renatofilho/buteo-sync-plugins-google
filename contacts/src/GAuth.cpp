/*
 * This file is part of buteo-gcontact-plugin package
 *
 * Copyright (C) 2013 Jolla Ltd. and/or its subsidiary(-ies).
 *
 * Contributors: Sateesh Kavuri <sateesh.kavuri@gmail.com>
 *               Mani Chandrasekar <maninc@gmail.com>
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

#include "config.h"
#include "GAuth.h"

#include <QVariantMap>
#include <QTextStream>
#include <QFile>
#include <QStringList>
#include <QDebug>

#include <Accounts/AccountService>

#include <ProfileEngineDefs.h>
#include <LogMacros.h>

using namespace Accounts;
using namespace SignOn;

const QString SYNC_SERVICE          ("google-contacts2");

GAuth::GAuth(const quint32 accountId, const QString scope, QObject *parent) :
    QObject(parent),
    mAccountId(accountId),
    mScope(scope)
{
}

bool GAuth::init()
{
    if (mAccountManager) {
        LOG_DEBUG("GAuth already initialized");
        return false;
    }

    mAccountManager = new Accounts::Manager();
    if (mAccountManager == NULL) {
        LOG_DEBUG("Account manager is not created... Cannot authenticate");
        return false;
    }

    mAccount = Accounts::Account::fromId(mAccountManager.data(), mAccountId, this);
    if (mAccount == NULL) {
        LOG_DEBUG("Account is not created... Cannot authenticate");
        return false;
    }

    return true;
}

QString GAuth::accountDisplayName() const
{
    if (mAccount) {
        return mAccount->displayName();
    } else {
        return QString();
    }
}

void GAuth::sessionResponse(const SessionData &sessionData) {
    SignOn::AuthSession *session = qobject_cast<SignOn::AuthSession*>(sender());
    session->disconnect(this);

    mToken = sessionData.getProperty(QStringLiteral("AccessToken")).toString();
    LOG_DEBUG("Authenticated !!!");

    emit success();
}

const QString GAuth::token()
{
    if (mToken.isEmpty()) {
        authenticate();
    }

    return mToken;
}

void GAuth::authenticate()
{
    if (mSession) {
        LOG_WARNING(QString(QLatin1String("error: Google account %1 Authenticate already requested"))
                .arg(mAccount->displayName()));
        return;
    }

    Accounts::Service srv(mAccountManager->service(SYNC_SERVICE));
    mAccount->selectService(srv);

    Accounts::AccountService *accSrv = new Accounts::AccountService(mAccount, srv);
    if (!accSrv) {
        LOG_WARNING(QString(QLatin1String("error: Google account %1 has no valid account service"))
                .arg(mAccount->displayName()));
        return;
    }

    AuthData authData = accSrv->authData();
    mIdentity = SignOn::Identity::existingIdentity(authData.credentialsId());
    if (!mIdentity) {
        LOG_WARNING(QString(QLatin1String("error: Google account %1 has no valid credentials"))
                .arg(mAccount->displayName()));
        return;
    }

    mSession = mIdentity->createSession(authData.method());
    if (!mSession) {
        LOG_WARNING(QString(QLatin1String("error: could not create signon session for Google account %1"))
                .arg(mAccount->displayName()));
        accSrv->deleteLater();
        return;
    }
    connect(mSession.data(), SIGNAL(response(SignOn::SessionData)),
            SLOT(sessionResponse(SignOn::SessionData)), Qt::QueuedConnection);
    connect(mSession.data(), SIGNAL(error(SignOn::Error)),
            SLOT(error(SignOn::Error)), Qt::QueuedConnection);
    accSrv->deleteLater();

    QVariantMap signonSessionData = authData.parameters();
    signonSessionData.insert("ClientId", GOOGLE_CONTACTS_CLIENT_ID);
    signonSessionData.insert("ClientSecret", GOOGLE_CONTACTS_CLIENT_SECRET);
    //signonSessionData.insert("UiPolicy", SignOn::NoUserInteractionPolicy);
    //mSession->setProperty("accountId", mAccount->id());
    //mSession->setProperty("mechanism", mechanism);
    //mSession->setProperty("signonSessionData", signonSessionData);

    mSession->process(signonSessionData, authData.mechanism());
    qDebug() << "INITIIIIIIIIIIIIIIII+++" << __LINE__;

}

void GAuth::credentialsStored(const quint32 id)
{
    mAccount->setCredentialsId(id);
    mAccount->sync();
}

void GAuth::error(const SignOn::Error & error)
{
    qDebug() << "LOGIN ERROR:" << error.message();
    emit failed();
}
