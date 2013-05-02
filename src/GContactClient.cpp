/*
 * This file is part of buteo-gcontact-plugins package
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

#include "GContactClient.h"
#include "GTransport.h"

#include <QLibrary>
#include <QtNetwork>

#include <buteosyncfw/SyncCommonDefs.h>
#include <buteosyncfw/PluginCbInterface.h>
#include <buteosyncfw/LogMacros.h>
#include <buteosyncfw/ProfileEngineDefs.h>
#include <buteosyncfw/ProfileManager.h>

extern "C" GContactClient* createPlugin(const QString& aPluginName,
        const Buteo::SyncProfile& aProfile,
        Buteo::PluginCbInterface *aCbInterface) {
    return new GContactClient(aPluginName, aProfile, aCbInterface);
}

extern "C" void destroyPlugin(GContactClient *aClient) {
    delete aClient;
}

GContactClient::GContactClient(const QString& aPluginName,
        const Buteo::SyncProfile& aProfile,
        Buteo::PluginCbInterface *aCbInterface) :
    ClientPlugin(aPluginName, aProfile, aCbInterface), mParser(0),
            mTransport(0), mCommittedItems(0) {
    FUNCTION_CALL_TRACE;
}

GContactClient::~GContactClient() {
    FUNCTION_CALL_TRACE;
}

bool
GContactClient::init() {
    FUNCTION_CALL_TRACE;

    //TODO: Initialize mParser

    if (initTransport()) {
        return true;
    } else {
        // Uninitialize everything that was initialized before failure.
        uninit();

        return false;
    }
}

bool
GContactClient::uninit()
{
    FUNCTION_CALL_TRACE;

    closeTransport();

    return true;
}

bool
GContactClient::startSync()
{
    /*
      1. If no previous sync, go for slow-sync. Fetch all contacts
         from server
      2. Check if previous sync happened (from SyncLog). If yes,
         fetch the time of last sync
      3. Using the last sync time, retrieve all contacts from server
         that were added/modified/deleted
      4. Fetch all added/modified/deleted items from device
      5. Check for conflicts. Take the default policy as "server-wins"
      6. Save the list from the server to device
      7. Push "client changes" - "conflicting items" to the server
      8. Save the sync log
     */

    FUNCTION_CALL_TRACE;

    if (mParser == 0 || mTransport == 0)
    {
        return false;
    }

    // syncStateChanged to signal changes from CONNECTING, RECEIVING
    // SENDING, DISCONNECTING, CLOSED
    connect(this, SIGNAL(stateChanged(Sync::SyncProgressDetail)),
            this, SLOT(receiveStateChanged(Sync::SyncProgressDetail)));

    // Take necessary action when sync is finished
    // FIXME: SyncStatus should reflect the status of the
    // transaction. More status fields need to be added
    connect(this, SIGNAL(syncFinished(Sync::SyncStatus)),
            this, SLOT(receiveSyncFinished(Sync::SyncStatus)));

    // Keep track of the changes as they go along
    connect(mParser, SIGNAL(itemProcessed (Sync::TransferType, Sync::TransferDatabase, int)),
            this, SLOT(receiveItemProcessed (Sync::TransferType, Sync::TransferDatabase, int)));

    return this->start ();
}

void
GContactClient::abortSync (Sync::SyncStatus aStatus)
{
    FUNCTION_CALL_TRACE;
    Sync::SyncStatus state = Sync::SYNC_ABORTED;

    if (aStatus == Sync::SYNC_ERROR) {
        state = Sync::SYNC_CONNECTION_ERROR;
    }

    if( !this->abort (state) )
    {
        LOG_DEBUG( "Agent not active, aborting immediately" );
        syncFinished(Sync::SYNC_ABORTED);

    }
    else
    {
        LOG_DEBUG( "Agent active, abort event posted" );
    }
}

bool
GContactClient::start ()
{
    return true;
}

bool
GContactClient::abort (Sync::SyncStatus status)
{
    // TODO: Implement
    return true;
}

bool
GContactClient::cleanUp() {
    FUNCTION_CALL_TRACE;

    // TODO: Implement
    return true;
}

void
GContactClient::receiveStateChanged(Sync::SyncProgressDetail aState)
{
    FUNCTION_CALL_TRACE;

    switch(aState) {
    case Sync::SYNC_PROGRESS_SENDING_ITEMS: {
        emit syncProgressDetail (getProfileName(), Sync::SYNC_PROGRESS_SENDING_ITEMS);
        break;
    }
    case Sync::SYNC_PROGRESS_RECEIVING_ITEMS: {
        emit syncProgressDetail (getProfileName(), Sync::SYNC_PROGRESS_RECEIVING_ITEMS);
        break;
    }
    case Sync::SYNC_PROGRESS_FINALISING: {
        emit syncProgressDetail (getProfileName(),Sync::SYNC_PROGRESS_FINALISING);
        break;
    }
    default:
        //do nothing
        break;
    };

#ifndef QT_NO_DEBUG
    LOG_DEBUG("***********  Sync Status has Changed to:" << toText(aState)
            << "****************");
#endif  //  QT_NO_DEBUG
}

void GContactClient::receiveSyncFinished(Sync::SyncStatus aState) {

    FUNCTION_CALL_TRACE;

#ifndef QT_NO_DEBUG
    LOG_DEBUG("***********  Sync has finished with:" << toText(aState)
            << "****************");
#endif  //  QT_NO_DEBUG
    switch(aState)
    {
        case Sync::SYNC_ERROR:
        case Sync::SYNC_AUTHENTICATION_FAILURE:
        case Sync::SYNC_DATABASE_FAILURE:
        case Sync::SYNC_CONNECTION_ERROR:
        case Sync::SYNC_NOTPOSSIBLE:
        {
            generateResults( false );
            emit error( getProfileName(), "", aState);
            break;
        }
        case Sync::SYNC_ABORTED:
        case Sync::SYNC_DONE:
        {
            generateResults( true );
            emit success( getProfileName(), QString::number(aState));
            break;
        }
        case Sync::SYNC_QUEUED:
        case Sync::SYNC_STARTED:
        case Sync::SYNC_PROGRESS:
        default:
        {
            generateResults( false );
            emit error( getProfileName(), "", aState);
            break;
        }
    }
}

void
GContactClient::receiveItemProcessed (Sync::TransferType aModificationType,
                                      Sync::TransferDatabase transferDb,
                                      QString database,
                                      int aCommittedItems)
{

    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Modification Type " << aModificationType);
    LOG_DEBUG("Committed items" << aCommittedItems);

    ++mCommittedItems;
    if(!receivedItems.contains(database))
    {
        ReceivedItemDetails details;
        details.added = details.modified = details.deleted = details.error = 0;
        //details.mime = aMimeType;
        receivedItems[database] = details;
    }

    switch (aModificationType) {
    case Sync::ITEM_ADDED: {
        ++receivedItems[database].added;
        break;
    }
    case Sync::ITEM_MODIFIED: {
        ++receivedItems[database].modified;
        break;
    }
    case Sync::ITEM_DELETED: {
        ++receivedItems[database].deleted;
        break;
    }
    case Sync::ITEM_ERROR: {
        ++receivedItems[database].error;
        break;
    }
    default: {
        Q_ASSERT(0);
        break;

    }
    }

    if( mCommittedItems == aCommittedItems )
    {
        QMapIterator<QString,ReceivedItemDetails> itr(receivedItems);
        while( itr.hasNext() )
        {
            itr.next();
            if( itr.value().added )
                emit transferProgress(getProfileName(), transferDb, Sync::ITEM_ADDED, itr.value().mime, itr.value().added);

            if( itr.value().modified )
                emit transferProgress(getProfileName(), transferDb, Sync::ITEM_MODIFIED, itr.value().mime, itr.value().modified);

            if( itr.value().deleted )
                emit transferProgress(getProfileName(), transferDb, Sync::ITEM_DELETED, itr.value().mime, itr.value().deleted);

            if( itr.value().error )
                emit transferProgress(getProfileName(), transferDb, Sync::ITEM_ERROR, itr.value().mime, itr.value().error);
        }
        mCommittedItems = 0;
        receivedItems.clear();
    }
}

void
GContactClient::addProcessedItem (Sync::TransferType modificationType,
                                  Sync::TransferDatabase database,
                                  QString modifiedDatabase)
{
    Buteo::DatabaseResults& results = mItemResults[modifiedDatabase];
    if( database == Sync::LOCAL_DATABASE ) {

        if( modificationType == Sync::ITEM_ADDED ) {
            ++results.iLocalItemsAdded;
        }
        else if( modificationType == Sync::ITEM_MODIFIED ) {
            ++results.iLocalItemsModified;
        }
        else if( modificationType == Sync::ITEM_DELETED ) {
            ++results.iLocalItemsDeleted;
        }
    }
    else if( database == Sync::REMOTE_DATABASE ) {

        if( modificationType == Sync::ITEM_ADDED ) {
            ++results.iRemoteItemsAdded;
        }
        else if( modificationType == Sync::ITEM_MODIFIED ) {
            ++results.iRemoteItemsModified;
        }
        else if( modificationType == Sync::ITEM_DELETED ) {
            ++results.iRemoteItemsDeleted;
        }
    }
}

bool
GContactClient::initTransport()
{
    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Initiating transport...");

    return initHttpTransport ();
}

void GContactClient::closeTransport() {

    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Closing transport...");

    delete mTransport;
    mTransport = NULL;

    LOG_DEBUG("Transport closed");

}

bool
GContactClient::initConfig () {

    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Initiating config...");

    QStringList storageNames = iProfile.subProfileNames(
            Buteo::Profile::TYPE_STORAGE);

    if (storageNames.isEmpty()) {
        LOG_CRITICAL("No storages defined for profile, nothing to sync");
        return false;
    }

    mSyncDirection = syncDirection ();

    mConflictResPolicy = conflictResolutionPolicy ();

    return true;

}

void GContactClient::closeConfig() {

    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Closing config...");
}

Buteo::SyncResults GContactClient::getSyncResults() const {
    FUNCTION_CALL_TRACE;

    return mResults;
}

void GContactClient::connectivityStateChanged(Sync::ConnectivityType aType,
        bool aState) {
    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Received connectivity change event:" << aType << " changed to "
            << aState);
}

bool
GContactClient::initHttpTransport() {
    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Creating HTTP transport");

    QString remoteURI = iProfile.key(Buteo::KEY_REMOTE_DATABASE);
    bool success = false;

    if (!remoteURI.isEmpty()) {

        mTransport = new GTransport ();

        LOG_DEBUG("Setting remote URI to" << remoteURI);
        mTransport->setUrl (remoteURI);

        QString proxyHost = iProfile.key(Buteo::KEY_HTTP_PROXY_HOST);
        if (!proxyHost.isEmpty()) {

            QString proxyPort = iProfile.key(Buteo::KEY_HTTP_PROXY_PORT);

            mTransport->setProxy (proxyHost, proxyPort);

            LOG_DEBUG("Proxy host:" << proxyHost);
            LOG_DEBUG("Proxy port:" << proxyPort);
        } else {
            LOG_DEBUG("Not using proxy");
        }

        success = true;
    } else {
        LOG_DEBUG("Could not find 'Remote database' property");
    }

    return success;
}

Buteo::SyncProfile::SyncDirection
GContactClient::syncDirection () {
    FUNCTION_CALL_TRACE;

    Buteo::SyncProfile::SyncDirection direction =
            iProfile.syncDirection();

    return direction;
}

Buteo::SyncProfile::ConflictResolutionPolicy
GContactClient::conflictResolutionPolicy () {
    FUNCTION_CALL_TRACE;

    Buteo::SyncProfile::ConflictResolutionPolicy crPolicy =
            iProfile.conflictResolutionPolicy();

    return crPolicy;
}

void GContactClient::generateResults( bool aSuccessful )
{
    FUNCTION_CALL_TRACE;

    mResults.setMajorCode( aSuccessful ? Buteo::SyncResults::SYNC_RESULT_SUCCESS : Buteo::SyncResults::SYNC_RESULT_FAILED );

    mResults.setTargetId(iProfile.name());

    if (mItemResults.isEmpty())
    {
        LOG_DEBUG("No items transferred");
    }
    else
    {
        QMapIterator<QString, Buteo::DatabaseResults> i( mItemResults );
        while ( i.hasNext() )
        {
            i.next();
            const Buteo::DatabaseResults& r = i.value();
            Buteo::TargetResults targetResults(
                    i.key(), // Target name
                    Buteo::ItemCounts( r.iLocalItemsAdded,
                                       r.iLocalItemsDeleted,
                                       r.iLocalItemsModified ),
                    Buteo::ItemCounts( r.iRemoteItemsAdded,
                                       r.iRemoteItemsDeleted,
                                       r.iRemoteItemsModified ));
            mResults.addTargetResults( targetResults );

            LOG_DEBUG("Items for" << targetResults.targetName() << ":");
            LOG_DEBUG("LA:" << targetResults.localItems().added <<
                      "LD:" << targetResults.localItems().deleted <<
                      "LM:" << targetResults.localItems().modified <<
                      "RA:" << targetResults.remoteItems().added <<
                      "RD:" << targetResults.remoteItems().deleted <<
                      "RM:" << targetResults.remoteItems().modified);
        }
    }
}

QDateTime
GContactClient::lastSyncTime ()
{
    Buteo::ProfileManager pm;
    Buteo::SyncProfile* sp = pm.syncProfile (iProfile.name ());
    return sp->lastSyncTime ();
}
