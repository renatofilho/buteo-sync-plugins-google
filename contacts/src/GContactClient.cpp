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
#include "GParseStream.h"
#include "GContactsBackend.h"
#include "GAuth.h"
#include "GContactEntry.h"
#include "GWriteStream.h"
#include "GConfig.h"

#include <QLibrary>
#include <QtNetwork>
#include <QDateTime>
#include <QContactGuid>
#include <QContactDetailFilter>
#include <QContactAvatar>

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
    ClientPlugin(aPluginName, aProfile, aCbInterface), mSlowSync (true),
    mTransport(0), mCommittedItems(0), mStartIndex (1), mHasMoreContactsToStore (false), mHasPhotosToStore (false)
{
    FUNCTION_CALL_TRACE;
}

GContactClient::~GContactClient()
{
    FUNCTION_CALL_TRACE;
}

bool
GContactClient::init()
{
    FUNCTION_CALL_TRACE;

    if (lastSyncTime ().isNull ())
        mSlowSync = true;
    else
        mSlowSync = false;

    mContactBackend = new GContactsBackend ();
    if (initConfig () && initTransport ())
    {
        return true;
    } else
    {
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

    if (!mContactBackend || !mGoogleAuth || !mTransport)
        return false;

    LOG_DEBUG ("Init done. Continuing with sync");

    // syncStateChanged to signal changes from CONNECTING, RECEIVING
    // SENDING, DISCONNECTING, CLOSED
    connect(this, SIGNAL(stateChanged(Sync::SyncProgressDetail)),
            this, SLOT(receiveStateChanged(Sync::SyncProgressDetail)));

    // Take necessary action when sync is finished
    connect(this, SIGNAL(syncFinished(Sync::SyncStatus)),
            this, SLOT(receiveSyncFinished(Sync::SyncStatus)));

    QTimer::singleShot (0, this, SLOT (start ()));
    return true;
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
    FUNCTION_CALL_TRACE;

    switch (mSyncDirection)
    {
    case Buteo::SyncProfile::SYNC_DIRECTION_TWO_WAY:
        if (mSlowSync)
            allLocalContactIds ();
        else
            changedLocalContactIds ();

        fetchRemoteContacts (mStartIndex);

        break;
    case Buteo::SyncProfile::SYNC_DIRECTION_FROM_REMOTE:
        // Not required
        break;
    case Buteo::SyncProfile::SYNC_DIRECTION_TO_REMOTE:
        // Not required
        break;
    case Buteo::SyncProfile::SYNC_DIRECTION_UNDEFINED:
        // Not required
    default:
        // throw configuration error
        break;
    };

    return true;
}

bool
GContactClient::abort (Sync::SyncStatus status)
{
    emit syncFinished (Sync::SYNC_ABORTED);
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
    //LOG_DEBUG("***********  Sync Status has Changed to:" << toText(aState)
    //        << "****************");
#endif  //  QT_NO_DEBUG
}

void
GContactClient::receiveSyncFinished(Sync::SyncStatus aState)
{
    FUNCTION_CALL_TRACE;

#ifndef QT_NO_DEBUG
    //LOG_DEBUG("***********  Sync has finished with:" << toText(aState)
     //       << "****************");
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
    FUNCTION_CALL_TRACE;

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

    LOG_DEBUG("Creating HTTP transport");

    mRemoteURI = iProfile.key(Buteo::KEY_REMOTE_DATABASE);
    if (mRemoteURI.isEmpty ())
    {
        // Set to the default value
        mRemoteURI = "https://www.google.com/m8/feeds/contacts/default/full/";
    }

    mTransport = new GTransport ();
    Q_CHECK_PTR (mTransport);

    connect (mTransport, SIGNAL (finishedRequest ()),
             this, SLOT (networkRequestFinished ()));

    connect (mTransport, SIGNAL (error (int)),
             this, SLOT (networkError (int)));

    LOG_DEBUG("Setting remote URI to" << mRemoteURI);
    mTransport->setUrl (mRemoteURI);

    QString proxyHost = iProfile.key(Buteo::KEY_HTTP_PROXY_HOST);
    // Set proxy, if available
    if (!proxyHost.isEmpty()) {

        QString proxyPort = iProfile.key(Buteo::KEY_HTTP_PROXY_PORT);

        mTransport->setProxy (proxyHost, proxyPort);

        LOG_DEBUG("Proxy host:" << proxyHost);
        LOG_DEBUG("Proxy port:" << proxyPort);
    } else {
        LOG_DEBUG("Not using proxy");
    }

    return true;
}

void
GContactClient::closeTransport()
{

    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Closing transport...");

    mTransport->deleteLater ();
    mTransport = NULL;

    LOG_DEBUG("Transport closed");
}

bool
GContactClient::initConfig ()
{

    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Initiating config...");

    mGoogleAuth = new GAuth ();

    mSyncDirection = iProfile.syncDirection();

    mConflictResPolicy = iProfile.conflictResolutionPolicy();

    return true;
}

void
GContactClient::closeConfig()
{

    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Closing config...");
}

Buteo::SyncResults
GContactClient::getSyncResults() const
{
    FUNCTION_CALL_TRACE;

    return mResults;
}

void
GContactClient::connectivityStateChanged(Sync::ConnectivityType aType,
        bool aState)
{
    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Received connectivity change event:" << aType << " changed to "
            << aState);
}

Buteo::SyncProfile::SyncDirection
GContactClient::syncDirection ()
{
    FUNCTION_CALL_TRACE;

    return mSyncDirection;
}

Buteo::SyncProfile::ConflictResolutionPolicy
GContactClient::conflictResolutionPolicy ()
{
    FUNCTION_CALL_TRACE;

    return mConflictResPolicy;
}

void
GContactClient::generateResults( bool aSuccessful )
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

void
GContactClient::fetchRemoteContacts (const int startIndex)
{
    FUNCTION_CALL_TRACE;

    /**
     o Get last sync time
     o Get etag value from local file system (this is a soft etag)
     o Connect finishedRequest to parseResults & network error slots
     o Use mTransport to perform network fetch
    */
    QDateTime syncTime = lastSyncTime ();
    if (!syncTime.isNull ())
        mTransport->setUpdatedMin (syncTime);

    if (startIndex != 1)
    {
        mTransport->setStartIndex (startIndex);
    }
    mTransport->setMaxResults (GConfig::MAX_RESULTS);
    if (mSlowSync == false)
        mTransport->setShowDeleted ();

    // FIXME: Fetching contacts using etag value as described in Google
    // data API does not seem to work
    // https://developers.google.com/gdata/docs/2.0/reference

    QString token = authToken ();
    if (token.isNull () || token.isEmpty ())
    {
        LOG_CRITICAL ("Auth token is null");
        // Better error would be SYNC_CONFIG_ERROR
        emit syncFinished (Sync::SYNC_ERROR);
        return;
    }
    mTransport->setGDataVersionHeader ();
    mTransport->addHeader (QByteArray ("Authorization"),
                           QByteArray (QString ("Bearer " + token).toAscii ()));

    mTransport->request (GTransport::GET);
}

void
GContactClient::changedLocalContactIds ()
{
    FUNCTION_CALL_TRACE;

    /**
     o Get last sync time
     o Fetch added/modified/deleted contact ids
     */

    QDateTime syncTime = lastSyncTime ();

    mAddedContactIds = mContactBackend->getAllNewContactIds (syncTime);
    mModifiedContactIds = mContactBackend->getAllModifiedContactIds (syncTime);
    mDeletedContactIds = mContactBackend->getAllDeletedContactIds (syncTime);
}

void
GContactClient::allLocalContactIds ()
{
    FUNCTION_CALL_TRACE;

    mAllLocalContactIds = mContactBackend->getAllContactIds ();
    LOG_DEBUG ("Number of contacts:" << mAllLocalContactIds.size ());
}

const QString
GContactClient::authToken ()
{
    FUNCTION_CALL_TRACE;

    QString token = mGoogleAuth->token ();

    return token;
}

const QDateTime
GContactClient::lastSyncTime ()
{
    FUNCTION_CALL_TRACE;

    Buteo::ProfileManager pm;
    Buteo::SyncProfile* sp = pm.syncProfile (iProfile.name ());
    // Without the hack (adding 5 secs), the qttracker engine contact storage
    // time is greater than the sync finish time
    // Because of this, the already added contacts are being sync'd again
    // for consecutive sync's
    if (!sp->lastSuccessfulSyncTime ().isNull ())
        return sp->lastSuccessfulSyncTime ().addSecs (5);
    else
        return sp->lastSuccessfulSyncTime ();
}

/**
  * The state machine is pretty much maintained in this method.
  * Maybe it is better to create a separate class that can handle
  * the sync state. It can act of Qt signals that will be emitted
  * by this method
  */
void
GContactClient::networkRequestFinished ()
{
    FUNCTION_CALL_TRACE;

    // o Error - if network error, set the sync results with the code
    // o Call uninit
    // o Stop sync
    // o If success, invoke the mParser->parse ()

    const QNetworkReply* reply = mTransport->reply ();
    GTransport::HTTP_REQUEST_TYPE requestType = mTransport->requestType ();
    if (reply)
    {
        QByteArray data = mTransport->replyBody ();
        LOG_DEBUG (data);
        if (data.isNull ())
        {
            LOG_DEBUG ("Nothing returned from server");
            return;
        }

        GParseStream parser (false);
        GAtom* atom = parser.parse (data);

        if (!atom)
        {
            LOG_CRITICAL ("NULL atom object. Something wrong with parsing");
            emit syncFinished (Sync::SYNC_ERROR);
        }

        if ((requestType == GTransport::POST) ||
            (requestType == GTransport::PUT))
        {
            LOG_DEBUG ("@@@PREVIOUS REQUEST TYPE=POST");
            // Check if there are any errors in the returned XML
            // buffer
            if (atom->title () == "Error")
            {
                LOG_CRITICAL ("Error in the request message. Check the data format");
                LOG_CRITICAL (data);
                // No need to signal error for the entire sync.
                // It might be a problem in a specific batch POST request
            }

            updateIdsToLocal (atom->entries ());

            // Since we are POSTing a batch of MAX_RESULTS to
            // the server, we will continue with the next batch
            // if there are any
            if (mHasMoreContactsToStore == true)
            {
                mSyncStatus = Sync::SYNC_PROGRESS;
                storeToRemote ();
            } else
            {
                LOG_DEBUG ("***Avatar list size=" << mContactsWithAvatars.size ());
                // Good time to store photos
                if (mContactsWithAvatars.size () > 0)
                {
                    // If any of the contacts have photos,
                    // spawn a new thread to store the photos
                    postAvatar (mContactsWithAvatars.takeFirst ());
                    mSyncStatus = Sync::SYNC_PROGRESS;
                } else
                    mSyncStatus = Sync::SYNC_DONE;
            }
        } else if (requestType == GTransport::GET)
        {
            LOG_DEBUG ("@@@PREVIOUS REQUEST TYPE=GET");
            QList<GContactEntry*> remoteContacts = atom->entries ();

            if (remoteContacts.size () > 0)
                storeToLocal (remoteContacts);

            if ((!atom->nextEntriesUrl ().isNull () ||
                !atom->nextEntriesUrl ().isEmpty ()))
            {
                // Request for the next batch
                // This condition will make this slot to be
                // called again and again until there are no more
                // entries left to be fetched from the server
                mStartIndex += GConfig::MAX_RESULTS;
                //mTransport->setUrl (atom->nextEntriesUrl ());
                fetchRemoteContacts (mStartIndex);
                mSyncStatus = Sync::SYNC_PROGRESS;
            } else
            {
                // There are no more entries to be fetched from
                // server. Continue with saving the local contacts to
                // server
                if ((storeToRemote () == true) ||
                    (mHasPhotosToStore == true))
                    mSyncStatus = Sync::SYNC_PROGRESS;
                else
                {
                    mSyncStatus = Sync::SYNC_DONE;
                }
            }
        }
        delete atom;
    } else
    {
        mSyncStatus = Sync::SYNC_ERROR;
    }

    if (mSyncStatus == Sync::SYNC_DONE ||
        mSyncStatus == Sync::SYNC_ERROR)
    {
        emit syncFinished (mSyncStatus);
    }
}

void
GContactClient::networkError (int errorCode)
{
    FUNCTION_CALL_TRACE;

    // TODO: If interested, check the value of error. But
    // it is enough to say that it is a SYNC_CONNECTION_ERROR
    //emit syncFinished (Sync::SYNC_CONNECTION_ERROR);
    switch (errorCode)
    {
    case 400:
        // Bad request. Better to bail out, since it could be a problem with the
        // data format of the request/response
        mSyncStatus = Sync::SYNC_BAD_REQUEST;
        break;
    case 401:
        mSyncStatus = Sync::SYNC_AUTHENTICATION_FAILURE;
        break;
    case 403:
    case 408:
        mSyncStatus = Sync::SYNC_ERROR;
        break;
    case 500:
    case 503:
    case 504:
        // Server failures
        mSyncStatus = Sync::SYNC_SERVER_FAILURE;
        break;
    default:
        break;
    };

    emit syncFinished (mSyncStatus);
}

void
GContactClient::filterRemoteAddedModifiedDeletedContacts (const QList<GContactEntry *> remoteContacts,
                                                          QList<GContactEntry*>& remoteAddedContacts,
                                                          QList<GContactEntry*>& remoteModifiedContacts,
                                                          QList<GContactEntry*>& remoteDeletedContacts)
{
    FUNCTION_CALL_TRACE;

    foreach (GContactEntry* entry, remoteContacts)
    {
        QContactLocalId localId = mContactBackend->entryExists (entry->guid ());
        if (localId != 0)
            entry->setLocalId (QString::number (localId));

        if (localId != 0)
        {
            if (entry->deleted () == true)
                remoteDeletedContacts.append (entry);
            else
                remoteModifiedContacts.append (entry);
        } else
            remoteAddedContacts.append (entry);
    }
}

bool
GContactClient::storeToRemote ()
{
    FUNCTION_CALL_TRACE;

    QByteArray encodedContacts;
    if (mSlowSync == true)
    {
        LOG_DEBUG ("TOTAL LOCAL CONTACTS FOR REMOTE STORAGE:" << mAllLocalContactIds.size ());
        if (!mAllLocalContactIds.isEmpty ())
        {
            GWriteStream ws;
            ws.encodeContacts (mAllLocalContactIds.mid (0, GConfig::MAX_RESULTS),
                           GConfig::ADD);
            encodedContacts = ws.encodedStream ();
            mContactsWithAvatars.append (ws.contactsWithAvatars ());

            // Once the contacts have been encoded, remove them
            // from mAllLocalContactIds
            for (int i=0; i<GConfig::MAX_RESULTS; i++)
                mAllLocalContactIds.removeAt (i);

            if (mAllLocalContactIds.size () > 0)
                mHasMoreContactsToStore = true;
            else
                mHasMoreContactsToStore = false;
        } else
        {
            return false; // Function exit here, if there are no contacts to be sync'd
        }

    } else
    {
        QHash<QContactLocalId, GConfig::TRANSACTION_TYPE> allChangedContactIds;

        /*
         * Create a new list of contact ids that are added/modified/deleted
         * The ids added to this list need to be deleted from the
         * member attributes so that the sync'd entries would nto be present
         * the next time, this method is invoked
         */
        int totalCount = 0;
        QHash<QString, QContactLocalId>::iterator iter = mAddedContactIds.begin ();

        while (iter != mAddedContactIds.end ())
        {
            allChangedContactIds.insert (iter.value (), GConfig::ADD);
            iter = mAddedContactIds.erase (iter);
            totalCount++;
            if ((iter == mAddedContactIds.end ()) ||
                (totalCount > GConfig::MAX_RESULTS))
                break;
            ++iter;
        }

        iter = mModifiedContactIds.begin ();
        while (iter != mModifiedContactIds.end ())
        {
            allChangedContactIds.insert (iter.value (), GConfig::UPDATE);
            iter = mModifiedContactIds.erase (iter);
            totalCount++;
            if ((iter == mModifiedContactIds.end ()) ||
                (totalCount > GConfig::MAX_RESULTS))
                break;
            ++iter;
        }

        iter = mDeletedContactIds.begin ();
        while (iter != mDeletedContactIds.end ())
        {
            allChangedContactIds.insert (iter.value (), GConfig::DELETE);
            iter = mDeletedContactIds.erase (iter);
            totalCount++;
            if ((iter == mDeletedContactIds.end ()) ||
                (totalCount > GConfig::MAX_RESULTS))
                break;
            ++iter;
        }

        if (allChangedContactIds.size () > 0)
        {
            GWriteStream ws;

            encodedContacts = ws.encodeContact (allChangedContactIds);
            mContactsWithAvatars.append (ws.contactsWithAvatars ());
        } else
        {
            return false; // Function exit here, if there are no contacts to be sync'd
        }

        if ((mAddedContactIds.size () +
             mModifiedContactIds.size () +
             mDeletedContactIds.size ()) > 0)
            mHasMoreContactsToStore = true;
        else
            mHasMoreContactsToStore = false;

    }

    if (mContactsWithAvatars.size () > 0)
        mHasPhotosToStore = true;

    mTransport->reset ();
    mTransport->setUrl (mRemoteURI + "/batch");
    mTransport->setGDataVersionHeader ();
    mTransport->setAuthToken (mGoogleAuth->token ());
    mTransport->setData (encodedContacts);
    mTransport->addHeader ("Content-Type", "application/atom+xml; charset=UTF-8; type=feed");

    LOG_DEBUG ("POST DATA:" << encodedContacts);

    mTransport->request (GTransport::POST);

    return mHasMoreContactsToStore;
}

bool
GContactClient::storeToLocal (const QList<GContactEntry*> remoteContacts)
{
    FUNCTION_CALL_TRACE;

    bool syncSuccess = false;
    if (mSlowSync == true)
    {
        LOG_DEBUG ("@@@storeToLocal#SLOW SYNC");
        // Since we request for all the deleted contacts, if
        // slow sync is performed many times, even deleted contacts
        // will appear in *remoteContacts. Filter them out while
        // saving them to device
        LOG_DEBUG ("TOTAL REMOTE CONTACTS:" << remoteContacts.size ());
        if (remoteContacts.size () > 0)
        {
            QList<QContact> remoteQContacts = toQContacts (remoteContacts);
            QMap<int, GContactsStatus> statusMap;

            if (mContactBackend->addContacts (remoteQContacts, statusMap))
            {
                // TODO: Saving succeeded. Update sync results
                syncSuccess = true;
            } else
            {
                // TODO: Saving failed. Update sync results and probably stop sync
                syncSuccess = false;
            }
        }
    } else if (mSlowSync == false)
    {
        LOG_DEBUG ("@@@storeToLocal#FAST SYNC");
        QList<GContactEntry*> remoteAddedContacts, remoteModifiedContacts, remoteDeletedContacts;
        filterRemoteAddedModifiedDeletedContacts (remoteContacts,
                                                  remoteAddedContacts,
                                                  remoteModifiedContacts,
                                                  remoteDeletedContacts);

        resolveConflicts (remoteModifiedContacts, remoteDeletedContacts);

        LOG_DEBUG ("###REMOTED ADDED=" << remoteAddedContacts.size ());
        LOG_DEBUG ("###REMOTED MODIFIED=" << remoteModifiedContacts.size ());
        LOG_DEBUG ("###REMOTED DELETED=" << remoteDeletedContacts.size ());
        if (remoteAddedContacts.size () > 0)
        {
            LOG_DEBUG ("***Adding " << remoteAddedContacts.size () << " contacts");
            QList<QContact> addedContacts = toQContacts (remoteAddedContacts);
            QMap<int, GContactsStatus> addedStatusMap;
            if (mContactBackend->addContacts (addedContacts, addedStatusMap))
                syncSuccess = true;
            else
                syncSuccess = false;
        }

        if (remoteModifiedContacts.size () > 0)
        {
            LOG_DEBUG ("***Modifying " << remoteModifiedContacts.size () << " contacts");
            QList<QContact> modifiedContacts = toQContacts (remoteModifiedContacts);

            QStringList modifiedIdsList;
            for (int i=0; i<modifiedContacts.size (); i++)
                modifiedIdsList << QString (modifiedContacts.at (i).localId ());

            QMap<int, GContactsStatus> modifiedStatusMap =
            mContactBackend->modifyContacts (modifiedContacts, modifiedIdsList);

            if (modifiedStatusMap.size () > 0)
            {
                syncSuccess = true;
            } else
            {
                syncSuccess = false;
            }
        }

        if (remoteDeletedContacts.size () > 0)
        {
            LOG_DEBUG ("***Deleting " << remoteModifiedContacts.size () << " contacts");
            QStringList guidList;
            for (int i=0; i<remoteDeletedContacts.size (); i++)
                guidList << remoteDeletedContacts.at (i)->guid ();

            QStringList localIdList = mContactBackend->localIds (guidList);

            QMap<int, GContactsStatus> deletedStatusMap =
                    mContactBackend->deleteContacts (localIdList);
            if (deletedStatusMap.size () > 0)
            {
                syncSuccess = true;
            } else
            {
                syncSuccess = false;
            }
        }
    }
    return syncSuccess;
}

QList<QContact>
GContactClient::toQContacts (const QList<GContactEntry*> gContactList)
{
    FUNCTION_CALL_TRACE;

    QList<QContact> qContactList;

    foreach (GContactEntry* gContact, gContactList)
        qContactList.append (gContact->qContact ());

    return qContactList;
}

void
GContactClient::resolveConflicts (QList<GContactEntry*>& modifiedRemoteContacts,
                                  QList<GContactEntry*>& deletedRemoteContacts)
{
    FUNCTION_CALL_TRACE;

    // TODO: Handle conflicts. The steps:
    // o Compare the list of local modified/deleted contacts with
    //   the list of remote modified/deleted contacts
    // o Create a new list (a map maybe) that has the contacts to
    //   be modified/deleted using the conflict resolution policy
    //   (server-wins, client-wins, add-new)
    // o Return the list

    //QListIterator<GContactEntry*> iter (modifiedRemoteContacts);
    QList<GContactEntry*>::iterator iter;
    for (iter = modifiedRemoteContacts.begin (); iter != modifiedRemoteContacts.end (); ++iter)
    {
        GContactEntry* entry = *iter;
        if (mModifiedContactIds.contains (entry->guid ()))
        {
            if (mConflictResPolicy == Buteo::SyncProfile::CR_POLICY_PREFER_LOCAL_CHANGES)
                modifiedRemoteContacts.erase (iter);
            else
                mModifiedContactIds.remove (entry->guid ());
        }

        if (mDeletedContactIds.contains (entry->guid ()))
        {
            if (mConflictResPolicy == Buteo::SyncProfile::CR_POLICY_PREFER_LOCAL_CHANGES)
                modifiedRemoteContacts.erase (iter);
            else
                mDeletedContactIds.remove (entry->guid ());
        }
    }

    for (iter = deletedRemoteContacts.begin (); iter != deletedRemoteContacts.end (); ++iter)
    {
        GContactEntry* entry = *iter;
        if (mModifiedContactIds.contains (entry->guid ()))
        {
            if (mConflictResPolicy == Buteo::SyncProfile::CR_POLICY_PREFER_LOCAL_CHANGES)
                deletedRemoteContacts.erase (iter);
            else
                mModifiedContactIds.remove (entry->guid ());
        }

        if (mDeletedContactIds.contains (entry->guid ()))
        {
            // If the entry is deleted both at the server and
            // locally, then just remove it from the lists
            // so that no further action need to be taken
            deletedRemoteContacts.erase (iter);
            mDeletedContactIds.remove (entry->guid ());
        }
    }
}

void
GContactClient::updateIdsToLocal (const QList<GContactEntry*> responseEntries)
{
    FUNCTION_CALL_TRACE;

    // Fetch only the ids from the contact entry list
    QStringList contactIdList;
    foreach (GContactEntry* entry, responseEntries)
    {
        // We will have to store the local id also to the server,
        // so that a mapping can be done while storing the remote id to
        // local
        contactIdList << entry->localId ();
    }
    QList<QContact> qContacts = toQContacts (responseEntries);
    mContactBackend->modifyContacts (qContacts, contactIdList);
}

/**
  * The avatarUrl is of the format:
  * file:////home/uesr/file.jpg#121460
  * The fragment in the url is the contact local id, which is used
  * to generate the PUT url
  */
void
GContactClient::postAvatar (const QContactLocalId contactId)
{
    FUNCTION_CALL_TRACE;

    mTransport->reset ();

    QContact contact;
    mContactBackend->getContact (contactId, contact);
    QUrl url = contact.detail<QContactAvatar> ().imageUrl ();
    qDebug () << "^^^^URL:" << url.toString ();
    QString imageUrl = contact.detail<QContactAvatar> ().imageUrl ().toString ();
    QString guid = contact.detail<QContactGuid> ().guid ();
    QFile file (imageUrl);
    file.open (QIODevice::ReadOnly);
    mTransport->setData (file.readAll ());
    file.close ();

    mTransport->setUrl (mRemoteURI + "/photos/media/default/" + guid);
    mTransport->setGDataVersionHeader ();
    mTransport->setAuthToken (mGoogleAuth->token ());
    mTransport->addHeader ("Content-Type", "image/*");
    mTransport->addHeader ("If-Match", "*"); // We just override other clients updates

    mTransport->request (GTransport::PUT);
}
