/*
 * This file is part of buteo-gcontact-plugins package
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

#include "UContactsClient.h"
#include "UContactsBackend.h"
#include "UAbstractRemoteSource.h"
#include "UAuth.h"

//Buteo
#include <LogMacros.h>
#include <ProfileEngineDefs.h>
#include <ProfileManager.h>
#if 0
#include <Profile.h>
#include <SyncCommonDefs.h>
#include <SyncPluginBase.h>
#include <ClientPlugin.h>
#include <SyncResults.h>
#include <SyncProfile.h>
#include <Profile.h>
#include <PluginCbInterface.h>
#include <LogMacros.h>
#endif

#include <QLibrary>
#include <QtNetwork>
#include <QDateTime>
#include <QContactGuid>
#include <QContactDetailFilter>
#include <QContactAvatar>

class UContactsClientPrivate
{
public:
    UContactsClientPrivate(const QString &serviceName)
        : mAuth(0),
          mRemoteSource(0),
          mContactBackend(0),
          mServiceName(serviceName)
    {
    }

    UAuth*                      mAuth;
    UContactsBackend*           mContactBackend;
    UAbstractRemoteSource*      mRemoteSource;
    bool                        mSlowSync;
    QString                     mServiceName;
    // local database information
    QList<QContactId>           mAllLocalContactIds;
    QHash<QString, QContactId>  mAddedContactIds;
    QHash<QString, QContactId>  mModifiedContactIds;
    QHash<QString, QContactId>  mDeletedContactIds;
    // sync report
    QMap<QString, Buteo::DatabaseResults> mItemResults;
    Buteo::SyncResults          mResults;
    // sync profile
    QString mSyncTarget;
    qint32 mAccountId;
    Buteo::SyncProfile::SyncDirection mSyncDirection;
    Buteo::SyncProfile::ConflictResolutionPolicy mConflictResPolicy;
};

UContactsClient::UContactsClient(const QString& aPluginName,
                                 const Buteo::SyncProfile& aProfile,
                                 Buteo::PluginCbInterface *aCbInterface, const QString &serviceName)
    : ClientPlugin(aPluginName, aProfile, aCbInterface),
      d_ptr(new UContactsClientPrivate(serviceName))
{
    FUNCTION_CALL_TRACE;
}

UContactsClient::~UContactsClient()
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    delete d->mAuth;
    delete d->mRemoteSource;
}

bool
UContactsClient::init()
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    if (lastSyncTime().isNull()) {
        d->mSlowSync = true;
    } else {
        d->mSlowSync = false;
    }

    LOG_DEBUG ("Last sync date:" << lastSyncTime() << "Using slow sync?" << d->mSlowSync);
    if (!initConfig()) {
        return false;
    }

    d->mRemoteSource = createRemoteSource(this);
    // remote source will be initialized after authentication
    if (!d->mRemoteSource) {
        goto init_fail;
    }

    d->mContactBackend = createContactsBackend(this);
    if (!d->mContactBackend || !d->mContactBackend->init(d->mSyncTarget)) {
        goto init_fail;
    }

    d->mAuth = crateAuthenticator(this);
    if (!d->mAuth || !d->mAuth->init(d->mAccountId, d->mServiceName)) {
        goto init_fail;
    }

    connect(d->mAuth, SIGNAL(success()), SLOT(start()));
    connect(d->mAuth, SIGNAL(failed()), SLOT(onAuthenticationError()));

    // syncStateChanged to signal changes from CONNECTING, RECEIVING
    // SENDING, DISCONNECTING, CLOSED
    connect(this,
            SIGNAL(stateChanged(Sync::SyncProgressDetail)),
            SLOT(onStateChanged(Sync::SyncProgressDetail)));

    // Take necessary action when sync is finished
    connect(this,
            SIGNAL(syncFinished(Sync::SyncStatus)),
            SLOT(onSyncFinished(Sync::SyncStatus)));

    return true;

init_fail:
    delete d->mRemoteSource;
    delete d->mContactBackend;
    delete d->mAuth;
    d->mRemoteSource = 0;
    d->mContactBackend = 0;
    d->mAuth = 0;
    return false;
}

bool
UContactsClient::uninit()
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    delete d->mRemoteSource;
    delete d->mContactBackend;
    delete d->mAuth;
    d->mRemoteSource = 0;
    d->mContactBackend = 0;
    d->mAuth = 0;

    return true;
}

bool
UContactsClient::isReadyToSync() const
{
    const Q_D(UContactsClient);
    return (d->mContactBackend && d->mRemoteSource && d->mAuth);
}

UContactsBackend *UContactsClient::createContactsBackend(QObject *parent) const
{
    return new UContactsBackend(parent);
}

UAuth *UContactsClient::crateAuthenticator(QObject *parent) const
{
    return new UAuth(parent);
}

bool
UContactsClient::startSync()
{
    FUNCTION_CALL_TRACE;

    if (!isReadyToSync()) {
        LOG_WARNING ("Ubuntu plugin is not ready to sync.");
        return false;
    }
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

    Q_D(UContactsClient);
    LOG_DEBUG ("Init done. Continuing with sync");

    d->mAuth->authenticate();
    return true;
}

void
UContactsClient::abortSync(Sync::SyncStatus aStatus)
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    d->mRemoteSource->abort();
    emit syncFinished(Sync::SYNC_ABORTED);
}

bool
UContactsClient::initConfig()
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    //TODO: support multiple remote databases "scopes"
    QStringList accountList = iProfile.keyValues(Buteo::KEY_ACCOUNT_ID);
    if (!accountList.isEmpty()) {
        QString aId = accountList.first();
        if (aId != NULL) {
            d->mAccountId = aId.toInt();
        }
    } else {
        d->mAccountId = 0;
        return false;
    }

    QStringList databaseName = iProfile.keyValues(Buteo::KEY_LOCAL_URI);
    if (databaseName.isEmpty()) {
        LOG_WARNING("\"Loal URI\" is missing on configuration file");
        return false;
    }
    d->mSyncTarget = databaseName.first();
    d->mSyncDirection = iProfile.syncDirection();
    d->mConflictResPolicy = iProfile.conflictResolutionPolicy();
    d->mItemResults.insert(d->mSyncTarget, Buteo::DatabaseResults());

    return true;
}

void
UContactsClient::onAuthenticationError()
{
    emit syncFinished (Sync::SYNC_AUTHENTICATION_FAILURE);
}

bool
UContactsClient::start()
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    // Remote source will be create after authentication since it needs some information
    // about the authentication (auth-token, etc..)

    if (!d->mRemoteSource->init(remoteSourceProperties())) {
        LOG_WARNING("Fail to init remote source");
        return false;
    }

    switch (d->mSyncDirection)
    {
    case Buteo::SyncProfile::SYNC_DIRECTION_TWO_WAY:
    {
        QDateTime sinceDate = d->mSlowSync ? QDateTime() : lastSyncTime();

        loadLocalContacts(sinceDate);

        // load remote contacts
        if (d->mSlowSync) {
            connect(d->mRemoteSource,
                    SIGNAL(contactsFetched(QList<UContactInfo>,Sync::SyncStatus)),
                    SLOT(onRemoteContactsFetchedForSlowSync(QList<UContactInfo>,Sync::SyncStatus)));
        } else {
            qWarning() << "FAST SYNC NOT IMPLEMENTED";
            return false;
        }
        d->mRemoteSource->fetchContacts(sinceDate, !d->mSlowSync);
        break;
    }
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

void
UContactsClient::onRemoteContactsFetchedForSlowSync(const QList<UContactInfo> contacts,
                                                   Sync::SyncStatus status)
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);
    if ((status != Sync::SYNC_PROGRESS) && (status != Sync::SYNC_STARTED)) {
        disconnect(d->mRemoteSource);
    }

    if ((status == Sync::SYNC_PROGRESS) || (status == Sync::SYNC_DONE)) {
        // save remote contacts locally
        storeToLocal(contacts);

        if (status == Sync::SYNC_DONE) {
            // upload local contacts
            QList<UContactInfo> contacts;

            foreach(const QContactId &id, d->mAllLocalContactIds) {
                QContact contact;
                d->mContactBackend->getContact(id, contact);
                if (!contact.isEmpty()) {
                    QString remoteId = UContactsBackend::getRemoteId(contact);
                    contacts.append(UContactInfo(contact, id.toString(), remoteId, false));
                }
            }
            connect(d->mRemoteSource,
                    SIGNAL(contactsSaved(QList<UContactInfo>,Sync::SyncStatus)),
                    SLOT(onContactsSavedForSlowSync(QList<UContactInfo>, Sync::SyncStatus)));

            d->mRemoteSource->saveContacts(contacts);
        }
        // sync still running
        return;
    }

    emit syncFinished(status);
}

void
UContactsClient::onContactsSavedForSlowSync(const QList<UContactInfo> contacts,
                                            Sync::SyncStatus status)
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    if ((status != Sync::SYNC_PROGRESS) && (status != Sync::SYNC_STARTED)) {
        disconnect(d->mRemoteSource);
    }

    if ((status == Sync::SYNC_PROGRESS) || (status == Sync::SYNC_DONE)) {
        updateIdsToLocal(contacts);

        // sync report
        for (int i=0; i < contacts.size(); i++) {
            addProcessedItem(Sync::ITEM_ADDED,
                             Sync::REMOTE_DATABASE,
                             d->mAuth->accountDisplayName());
        }

        if (status == Sync::SYNC_PROGRESS) {
            // sync still in progress
            return;
        }
    }

    emit syncFinished(status);
}


bool
UContactsClient::storeToLocal(const QList<UContactInfo> &remoteContacts)
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    bool syncSuccess = false;

    if (d->mSlowSync) {
        LOG_DEBUG ("@@@storeToLocal#SLOW SYNC");
        // Since we request for all the deleted contacts, if
        // slow sync is performed many times, even deleted contacts
        // will appear in *remoteContacts. Filter them out while
        // saving them to device
        LOG_DEBUG ("TOTAL REMOTE CONTACTS:" << remoteContacts.size());

        if (!remoteContacts.isEmpty()) {
            QList<QContact> remoteQContacts = toQContacts(remoteContacts);
            QMap<int, UContactsStatus> statusMap;
            if (d->mContactBackend->addContacts(remoteQContacts, statusMap)) {
                // TODO: Saving succeeded. Update sync results
                syncSuccess = true;

                // sync report
                addProcessedItem(Sync::ITEM_ADDED, Sync::LOCAL_DATABASE, syncTargetId());

                // Remove remote contacts from localContacts to avoid uploade it
                foreach (const UContactInfo &c, remoteContacts) {
                    d->mAllLocalContactIds.removeOne(QContactId::fromString(c.localId()));
                }
            } else {
                // TODO: Saving failed. Update sync results and probably stop sync
                syncSuccess = false;
            }
        }
    } else {
        //TODO: FAST SYNC
    }
    return syncSuccess;
}


bool
UContactsClient::cleanUp()
{
    FUNCTION_CALL_TRACE;
    //TODO
    return true;
}

void UContactsClient::connectivityStateChanged(Sync::ConnectivityType aType, bool aState)
{
    FUNCTION_CALL_TRACE;
    LOG_DEBUG("Received connectivity change event:" << aType << " changed to " << aState);
}

void
UContactsClient::loadLocalContacts(const QDateTime &since)
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    if (since.isValid()) {
        d->mAllLocalContactIds = d->mContactBackend->getAllContactIds();

        LOG_DEBUG ("Number of contacts:" << d->mAllLocalContactIds.size ());
    } else {
        d->mAddedContactIds = d->mContactBackend->getAllNewContactIds(since);
        d->mModifiedContactIds = d->mContactBackend->getAllModifiedContactIds(since);
        d->mDeletedContactIds = d->mContactBackend->getAllDeletedContactIds(since);

        LOG_DEBUG ("Number of local added contacts:" << d->mAddedContactIds.size());
        LOG_DEBUG ("Number of local modified contacts:" << d->mModifiedContactIds.size());
        LOG_DEBUG ("Number of local removed contacts:" << d->mDeletedContactIds.size());
    }
}

void
UContactsClient::onStateChanged(Sync::SyncProgressDetail aState)
{
    FUNCTION_CALL_TRACE;

    switch(aState) {
    case Sync::SYNC_PROGRESS_SENDING_ITEMS: {
        emit syncProgressDetail(getProfileName(), Sync::SYNC_PROGRESS_SENDING_ITEMS);
        break;
    }
    case Sync::SYNC_PROGRESS_RECEIVING_ITEMS: {
        emit syncProgressDetail(getProfileName(), Sync::SYNC_PROGRESS_RECEIVING_ITEMS);
        break;
    }
    case Sync::SYNC_PROGRESS_FINALISING: {
        emit syncProgressDetail(getProfileName(),Sync::SYNC_PROGRESS_FINALISING);
        break;
    }
    default:
        //do nothing
        break;
    };
}

void
UContactsClient::onSyncFinished(Sync::SyncStatus aState)
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    switch(aState)
    {
        case Sync::SYNC_ERROR:
        case Sync::SYNC_AUTHENTICATION_FAILURE:
        case Sync::SYNC_DATABASE_FAILURE:
        case Sync::SYNC_CONNECTION_ERROR:
        case Sync::SYNC_NOTPOSSIBLE:
        {
            generateResults(false);
            emit error(getProfileName(), "", aState);
            break;
        }
        case Sync::SYNC_DONE:
            // purge all deleted contacts
            d->mContactBackend->purgecontacts();
        case Sync::SYNC_ABORTED:
        {
            generateResults(true);
            emit success(getProfileName(), QString::number(aState));
            break;
        }
        case Sync::SYNC_QUEUED:
        case Sync::SYNC_STARTED:
        case Sync::SYNC_PROGRESS:
        default:
        {
            generateResults(false);
            emit error(getProfileName(), "", aState);
            break;
        }
    }
}


Buteo::SyncResults
UContactsClient::getSyncResults() const
{
    return d_ptr->mResults;
}

QString
UContactsClient::authToken() const
{
    return d_ptr->mAuth->token();
}

QString
UContactsClient::syncTargetId() const
{
    return d_ptr->mContactBackend->syncTargetId();
}

const QDateTime
UContactsClient::lastSyncTime() const
{
    FUNCTION_CALL_TRACE;

    Buteo::ProfileManager pm;
    Buteo::SyncProfile* sp = pm.syncProfile (iProfile.name ());
    // Without the hack (adding 5 secs), the qttracker engine contact storage
    // time is greater than the sync finish time
    // Because of this, the already added contacts are being sync'd again
    // for consecutive sync's
    QDateTime lastTime = sp->lastSuccessfulSyncTime();
    if (!lastTime.isNull()) {
        // return UTC time used by google
        return lastTime.addSecs(30).toUTC();
    } else {
        return lastTime;
    }
}


void
UContactsClient::filterRemoteAddedModifiedDeletedContacts(const QList<UContactInfo> remoteContacts,
                                                          QList<UContactInfo>& remoteAddedContacts,
                                                          QList<UContactInfo>& remoteModifiedContacts,
                                                          QList<UContactInfo>& remoteDeletedContacts)
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    foreach (const UContactInfo &uContact, remoteContacts) {
        if (uContact.deleted()) {
            remoteDeletedContacts.append(uContact);
            continue;
        }

        QContactId localId = d->mContactBackend->entryExists(uContact.remoteId());
        if (localId.isNull()) {
            remoteAddedContacts.append(uContact);
        } else {
            remoteModifiedContacts.append(uContact);
        }
    }
}

QList<QContact>
UContactsClient::toQContacts (const QList<UContactInfo> &uContactList) const
{
    FUNCTION_CALL_TRACE;

    QList<QContact> qContactList;

    foreach (const UContactInfo &uContact, uContactList) {
        qContactList << uContact.contact();
    }

    return qContactList;
}

void
UContactsClient::resolveConflicts(QList<UContactInfo> &modifiedRemoteContacts,
                                  QList<UContactInfo> &deletedRemoteContacts)
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    // TODO: Handle conflicts. The steps:
    // o Compare the list of local modified/deleted contacts with
    //   the list of remote modified/deleted contacts
    // o Create a new list (a map maybe) that has the contacts to
    //   be modified/deleted using the conflict resolution policy
    //   (server-wins, client-wins, add-new)
    // o Return the list

    //QListIterator<GContactEntry*> iter (modifiedRemoteContacts);
    QList<UContactInfo>::iterator iter;
    for (iter = modifiedRemoteContacts.begin (); iter != modifiedRemoteContacts.end (); ++iter) {
        UContactInfo uContact = *iter;
        if (d->mModifiedContactIds.contains(uContact.remoteId())) {
            if (d->mConflictResPolicy == Buteo::SyncProfile::CR_POLICY_PREFER_LOCAL_CHANGES) {
                modifiedRemoteContacts.erase(iter);
            } else {
                d->mModifiedContactIds.remove(uContact.remoteId());
            }
        }

        if (d->mDeletedContactIds.contains(uContact.remoteId())) {
            if (d->mConflictResPolicy == Buteo::SyncProfile::CR_POLICY_PREFER_LOCAL_CHANGES) {
                modifiedRemoteContacts.erase (iter);
            } else {
                d->mDeletedContactIds.remove (uContact.remoteId());
            }
        }
    }

    for (iter = deletedRemoteContacts.begin (); iter != deletedRemoteContacts.end (); ++iter) {
        UContactInfo uContact = *iter;
        if (d->mModifiedContactIds.contains(uContact.remoteId())) {
            if (d->mConflictResPolicy == Buteo::SyncProfile::CR_POLICY_PREFER_LOCAL_CHANGES) {
                deletedRemoteContacts.erase(iter);
            } else {
                d->mModifiedContactIds.remove(uContact.remoteId());
            }
        }

        if (d->mDeletedContactIds.contains(uContact.remoteId())) {
            // If the entry is deleted both at the server and
            // locally, then just remove it from the lists
            // so that no further action need to be taken
            deletedRemoteContacts.erase(iter);
            d->mDeletedContactIds.remove(uContact.remoteId());
        }
    }
}

void
UContactsClient::updateIdsToLocal (const QList<UContactInfo> remoteContacts)
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    QStringList localIdList;
    foreach (const UContactInfo &uContact, remoteContacts) {
        localIdList << uContact.localId();
    }

    QList<QContact> contactsList = toQContacts(remoteContacts);
    d->mContactBackend->modifyContacts(contactsList, localIdList);
}

void
UContactsClient::addProcessedItem (Sync::TransferType modificationType,
                                   Sync::TransferDatabase database,
                                   const QString &modifiedDatabase)
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    Buteo::DatabaseResults& results = d->mItemResults[modifiedDatabase];
    if (database == Sync::LOCAL_DATABASE) {
        if (modificationType == Sync::ITEM_ADDED) {
            ++results.iLocalItemsAdded;
        } else if (modificationType == Sync::ITEM_MODIFIED) {
            ++results.iLocalItemsModified;
        } else if (modificationType == Sync::ITEM_DELETED) {
            ++results.iLocalItemsDeleted;
        }
    } else if (database == Sync::REMOTE_DATABASE) {
        if( modificationType == Sync::ITEM_ADDED) {
            ++results.iRemoteItemsAdded;
        } else if (modificationType == Sync::ITEM_MODIFIED) {
            ++results.iRemoteItemsModified;
        } else if (modificationType == Sync::ITEM_DELETED) {
            ++results.iRemoteItemsDeleted;
        }
    }
}

void
UContactsClient::generateResults(bool aSuccessful)
{
    FUNCTION_CALL_TRACE;
    Q_D(UContactsClient);

    d->mResults.setMajorCode(aSuccessful ? Buteo::SyncResults::SYNC_RESULT_SUCCESS :
                                           Buteo::SyncResults::SYNC_RESULT_FAILED );
    d->mResults.setTargetId(iProfile.name());
    if (d->mItemResults.isEmpty()) {
        LOG_DEBUG("No items transferred");
    } else {
        QMapIterator<QString, Buteo::DatabaseResults> i(d->mItemResults);
        while (i.hasNext())
        {
            i.next();
            const Buteo::DatabaseResults &r = i.value();
            Buteo::TargetResults targetResults(i.key(), // Target name
                                               Buteo::ItemCounts(r.iLocalItemsAdded,
                                                                 r.iLocalItemsDeleted,
                                                                 r.iLocalItemsModified),
                                               Buteo::ItemCounts(r.iRemoteItemsAdded,
                                                                 r.iRemoteItemsDeleted,
                                                                 r.iRemoteItemsModified));
            d->mResults.addTargetResults(targetResults);
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
