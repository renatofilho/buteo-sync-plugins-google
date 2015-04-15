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

#ifndef GCONTACTCLIENT_H
#define GCONTACTCLIENT_H

#include "buteosyncfw_p.h"
#include "buteo-gcontact-plugin_global.h"

#include <QNetworkReply>
#include <QContact>
#include <QList>
#include <QPair>

#include "GConfig.h"
#include "GTransport.h"

QTCONTACTS_USE_NAMESPACE

class GContactEntry;
class GTransport;
class GContactsBackend;
class GParseStream;
class GAuth;
class GWriteStream;

class BUTEOGCONTACTPLUGINSHARED_EXPORT GContactClient : public Buteo::ClientPlugin
{
public:
    Q_OBJECT
public:

    /*! \brief Constructor
     *
     * @param aPluginName Name of this client plugin
     * @param aProfile Sync profile
     * @param aCbInterface Pointer to the callback interface
     */
    GContactClient(const QString& aPluginName,
                   const Buteo::SyncProfile &aProfile,
                   Buteo::PluginCbInterface *aCbInterface);

    /*! \brief Destructor
     *
     * Call uninit before destroying the object.
     */
    virtual ~GContactClient();

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

signals:

    void stateChanged (Sync::SyncProgressDetail progress);

    void itemProcessed (Sync::TransferType type,
                        Sync::TransferDatabase db,
                        int committedItems);

    void syncFinished (Sync::SyncStatus);

protected slots:

    void receiveStateChanged (Sync::SyncProgressDetail);

    void receiveItemProcessed (Sync::TransferType,
                               Sync::TransferDatabase transferDb,
                               QString database,
                               int committedItems);

    void receiveSyncFinished (Sync::SyncStatus);

    void networkRequestFinished ();

    void networkError (int errorCode);

    bool start ();

    void authenticationError();

private:

    bool initConfig ();

    void closeConfig ();

    bool initTransport();

    void closeTransport();

    bool abort (Sync::SyncStatus status);

    Buteo::SyncProfile::SyncDirection syncDirection();

    Buteo::SyncProfile::ConflictResolutionPolicy conflictResolutionPolicy();

    void addProcessedItem (Sync::TransferType modificationType,
                           Sync::TransferDatabase database,
                           QString modifiedDatabase);

    void generateResults( bool aSuccessful );

    void fetchRemoteContacts (const int startIndex);

    void changedLocalContactIds ();

    void allLocalContactIds ();

    const QString authToken ();

    /**
     * \brief Method to determine when this session should
     *        be slow-sync or fast-sync
     */
    const QDateTime lastSyncTime ();

    void filterRemoteAddedModifiedDeletedContacts (const QList<GContactEntry*> remoteContacts,
                                             QList<GContactEntry*>& remoteAddedContacts,
                                             QList<GContactEntry*>& remoteModifiedContacts,
                                             QList<GContactEntry*>& remoteDeletedContacts);

    bool storeToRemote ();

    bool storeToLocal (const QList<GContactEntry*> remoteContacts);

    QList<QContact> toQContacts (const QList<GContactEntry*> gContactList);

    void resolveConflicts (QList<GContactEntry*>& modifiedRemoteContacts,
                           QList<GContactEntry*>& deletedRemoteContacts);

    void updateIdsToLocal (const QList<GContactEntry*> responseEntries);

    void postAvatar (const QContactId avatarUrl);

    GAuth*                      mGoogleAuth;

    bool                        mSlowSync;

    bool                        mHasMoreContactsToStore;

    GContactsBackend*           mContactBackend;

    GTransport*                 mTransport;

    Buteo::SyncResults          mResults;

    quint32                     mCommittedItems;

    quint32                     mAccountId;

    QList<QContactId>          mAllLocalContactIds;

    QHash<QString, QContactId> mAddedContactIds;
    QHash<QString, QContactId> mModifiedContactIds;
    QHash<QString, QContactId> mDeletedContactIds;

    QString             mSyncTarget;

    int mStartIndex;

    QString mRemoteURI;

    Sync::SyncStatus mSyncStatus;

    QList<QContactId> mContactsWithAvatars;

    bool mHasPhotosToStore;

    QMap<QString, Buteo::DatabaseResults> mItemResults;

    Buteo::SyncProfile::SyncDirection mSyncDirection;

    Buteo::SyncProfile::ConflictResolutionPolicy mConflictResPolicy;

#ifndef QT_NO_DEBUG
    friend class GContactClientTest;
#endif

};

/*! \brief Creates Google client plugin
 *
 * @param aPluginName Name of this client plugin
 * @param aProfile Profile to use
 * @param aCbInterface Pointer to the callback interface
 * @return Client plugin on success, otherwise NULL
 */
extern "C" GContactClient* createPlugin( const QString& aPluginName,
                                       const Buteo::SyncProfile& aProfile,
                                       Buteo::PluginCbInterface *aCbInterface );

/*! \brief Destroys Google client plugin
 *
 * @param aServer Google client plugin instance to destroy
 */
extern "C" void destroyPlugin( GContactClient *aClient );

#endif // GCONTACTCLIENT_H
