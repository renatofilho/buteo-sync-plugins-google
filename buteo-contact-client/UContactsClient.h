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

#ifndef UCONTACSTCLIENT_H
#define UCONTACSTCLIENT_H

#include "UContactInfo.h"

#include <QNetworkReply>
#include <QContact>
#include <QList>
#include <QPair>

#include <ClientPlugin.h>

//#include "GConfig.h"
//#include "GTransport.h"

QTCONTACTS_USE_NAMESPACE

class UContactsClientPrivate;
class UAuth;
class UAbstractRemoteSource;
class UContactsBackend;

class UContactsClient : public Buteo::ClientPlugin
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(UContactsClient)

public:

    /*! \brief Constructor
     *
     * @param aPluginName Name of this client plugin
     * @param aProfile Sync profile
     * @param aCbInterface Pointer to the callback interface
     * @param authenticator a instance of UAuth class to be used during the authentication
     */
    UContactsClient(const QString& aPluginName,
                    const Buteo::SyncProfile &aProfile,
                    Buteo::PluginCbInterface *aCbInterface,
                    const QString& serviceName);

    /*! \brief Destructor
     *
     * Call uninit before destroying the object.
     */
    virtual ~UContactsClient();

    //! @see SyncPluginBase::init
    virtual bool init();

    //! @see SyncPluginBase::uninit
    virtual bool uninit();

    //! @see ClientPlugin::startSync
    virtual bool startSync();

    //! @see SyncPluginBase::abortSync
    virtual void abortSync(Sync::SyncStatus aStatus = Sync::SYNC_ABORTED);

    //! @see SyncPluginBase::getSyncResults
    virtual Buteo::SyncResults getSyncResults() const;

    //! @see SyncPluginBase::cleanUp
    virtual bool cleanUp();

public slots:
    //! @see SyncPluginBase::connectivityStateChanged
    virtual void connectivityStateChanged( Sync::ConnectivityType aType,
                                           bool aState );

protected:
    QString authToken() const;
    QString syncTargetId() const;

    // Must be implemented by the plugins
    virtual UAbstractRemoteSource* createRemoteSource(QObject *parent) const = 0;
    virtual QVariantMap remoteSourceProperties() const = 0;

    virtual UContactsBackend* createContactsBackend(QObject *parent) const;
    virtual UAuth* crateAuthenticator(QObject *parent) const;

    virtual bool isReadyToSync() const;

signals:
    void stateChanged(Sync::SyncProgressDetail progress);
    void itemProcessed(Sync::TransferType type,
                       Sync::TransferDatabase db,
                       int committedItems);
    void syncFinished(Sync::SyncStatus);


private:
    QScopedPointer<UContactsClientPrivate> d_ptr;

    void loadLocalContacts(const QDateTime &since);
    const QDateTime lastSyncTime() const;
    QList<QContact> toQContacts (const QList<UContactInfo> &uContactList) const;
    bool initConfig();
    void generateResults(bool aSuccessful);
    bool storeToLocal(const QList<UContactInfo> &remoteContacts);
    void updateIdsToLocal(const QList<UContactInfo> remoteContacts);
    void filterRemoteAddedModifiedDeletedContacts(const QList<UContactInfo> remoteContacts,
                                                  QList<UContactInfo> &remoteAddedContacts,
                                                  QList<UContactInfo> &remoteModifiedContacts,
                                                  QList<UContactInfo> &remoteDeletedContacts);
    void resolveConflicts(QList<UContactInfo> &modifiedRemoteContacts,
                          QList<UContactInfo> &deletedRemoteContacts);
    void addProcessedItem (Sync::TransferType modificationType,
                           Sync::TransferDatabase database,
                           const QString &modifiedDatabase);

private slots:
    bool start();
    void onAuthenticationError();
    void onStateChanged(Sync::SyncProgressDetail progress);
    void onSyncFinished(Sync::SyncStatus status);

    void onRemoteContactsFetchedForSlowSync(const QList<UContactInfo> contacts,
                                            Sync::SyncStatus status);
    void onContactsSavedForSlowSync(const QList<UContactInfo> contacts,
                                    Sync::SyncStatus status);
};

#endif // UCONTACTCLIENT_H
