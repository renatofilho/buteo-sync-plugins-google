#include "GRemoteSource.h"
#include "GTransport.h"
#include "GParseStream.h"
#include "GWriteStream.h"
#include "GAtom.h"
#include "GContactEntry.h"
#include "GConfig.h"

#include "buteosyncfw_p.h"
#include <ProfileEngineDefs.h>
#include <ProfileManager.h>

GRemoteSource::GRemoteSource(QObject *parent)
    : UAbstractRemoteSource(parent),
      mTransport(new GTransport),
      mState(GRemoteSource::STATE_IDLE),
      mStartIndex(0)
{
    connect(mTransport.data(),
            SIGNAL(finishedRequest()),
            SLOT(networkRequestFinished()));

    connect(mTransport.data(),
            SIGNAL(error(int)),
            SLOT(networkError(int)));
}

GRemoteSource::~GRemoteSource()
{
}

bool GRemoteSource::init(const QVariantMap &properties)
{
    FUNCTION_CALL_TRACE;
    LOG_DEBUG("Creating HTTP transport");

    if (mState != GRemoteSource::STATE_IDLE) {
        LOG_WARNING("Init called with wrong state" << mState);
        return false;
    }

    mAuthToken = properties.value("AUTH-TOKEN").toString();
    mSyncTarget = properties.value("SYNC-TARGET").toString();
    mRemoteUri = properties.value(Buteo::KEY_REMOTE_DATABASE).toString();
    mStartIndex = 0;
    mContactsToUpload.clear();
    mState = GRemoteSource::STATE_IDLE;

    if (mRemoteUri.isEmpty()) {
        // Set to the default value
        mRemoteUri = QStringLiteral("https://www.google.com/m8/feeds/contacts/default/full/");
    }

    LOG_DEBUG("Setting remote URI to" << mRemoteUri);
    mTransport->setUrl(mRemoteUri);

    QString proxyHost = properties.value(Buteo::KEY_HTTP_PROXY_HOST).toString();
    // Set proxy, if available
    if (!proxyHost.isEmpty()) {
        QString proxyPort = properties.value(Buteo::KEY_HTTP_PROXY_PORT).toString();
        mTransport->setProxy(proxyHost, proxyPort);
        LOG_DEBUG("Proxy host:" << proxyHost);
        LOG_DEBUG("Proxy port:" << proxyPort);
    } else {
        LOG_DEBUG("Not using proxy");
    }

    return true;
}

void GRemoteSource::abort()
{
    //TODO: Abort
}

void GRemoteSource::fetchContacts(const QDateTime &since, bool includeDeleted)
{
    FUNCTION_CALL_TRACE;
    if (mState != GRemoteSource::STATE_IDLE) {
        LOG_WARNING("GRemote source is not in idle state, current state is" << mState);
        return;
    }

    mState = GRemoteSource::STATE_FETCHING_CONTACTS;
    fetchRemoteContacts(since, includeDeleted, 1);
}

void GRemoteSource::saveContacts(const QList<UContactInfo> contacts)
{
    FUNCTION_CALL_TRACE;
    if (mState != GRemoteSource::STATE_IDLE) {
        LOG_WARNING("GRemote source is not in idle state, current state is" << mState);
        emit contactsSaved(contacts, Sync::SYNC_ERROR);
        return;
    }

    if (contacts.isEmpty()) {
        emit contactsSaved(contacts, Sync::SYNC_DONE);
        return;
    }

    mState = GRemoteSource::STATE_FETCHING_CONTACTS;
    mContactsToUpload = contacts;
    saveContactsContinue();
}

void
GRemoteSource::saveContactsContinue()
{
    FUNCTION_CALL_TRACE;
    QHash<UContactInfo, GConfig::TRANSACTION_TYPE> contacts;
    int limit = qMin(mContactsToUpload.size(), GConfig::MAX_RESULTS);

    for (int i=0; i < limit; i++) {
        UContactInfo uContact = mContactsToUpload.takeFirst();
        GConfig::TRANSACTION_TYPE op = GConfig::UPDATE;
        if (uContact.remoteId().isEmpty()) {
            op = GConfig::ADD;
        }
        contacts.insert(uContact, op);
    }

    GWriteStream ws(mSyncTarget);
    QByteArray encodedContacts = ws.encodeContacts(contacts);

    mTransport->reset();
    mTransport->setUrl(mRemoteUri + "batch");
    mTransport->setGDataVersionHeader();
    mTransport->setAuthToken(mAuthToken);
    mTransport->setData(encodedContacts);
    mTransport->addHeader("Content-Type", "application/atom+xml; charset=UTF-8; type=feed");

    LOG_DEBUG ("POST DATA:" << encodedContacts);
    mTransport->request(GTransport::POST);
}

void
GRemoteSource::fetchRemoteContacts(const QDateTime &since, bool includeDeleted, int startIndex)
{
    FUNCTION_CALL_TRACE;
    /**
     o Get last sync time
     o Get etag value from local file system (this is a soft etag)
     o Connect finishedRequest to parseResults & network error slots
     o Use mTransport to perform network fetch
    */
    if (since.isValid()) {
        mTransport->setUpdatedMin(since);
    }

    if (startIndex > 1) {
        mTransport->setStartIndex(startIndex);
    }

    mTransport->setMaxResults(GConfig::MAX_RESULTS);
    if (includeDeleted) {
        mTransport->setShowDeleted();
    }

    mTransport->setGDataVersionHeader();
    mTransport->addHeader(QByteArray("Authorization"),
                          QString("Bearer " + mAuthToken).toUtf8());
    mTransport->request(GTransport::GET);
}

QList<UContactInfo> GRemoteSource::toUContacts(const QList<GContactEntry *> gContactList) const
{
    QList<UContactInfo> uContacts;
    foreach(const GContactEntry *entry, gContactList) {
        uContacts << UContactInfo(entry->qContact(), entry->localId(), entry->remoteId(), entry->deleted());
    }
    return uContacts;
}

/**
  * The state machine is pretty much maintained in this method.
  * Maybe it is better to create a separate class that can handle
  * the sync state. It can act of Qt signals that will be emitted
  * by this method
  */
void
GRemoteSource::networkRequestFinished()
{
    FUNCTION_CALL_TRACE;

    // o Error - if network error, set the sync results with the code
    // o Call uninit
    // o Stop sync
    // o If success, invoke the mParser->parse ()
    Sync::SyncStatus syncStatus = Sync::SYNC_ERROR;
    const QNetworkReply* reply = mTransport->reply();
    GTransport::HTTP_REQUEST_TYPE requestType = mTransport->requestType();
    if (reply) {
        QByteArray data = mTransport->replyBody();
        LOG_DEBUG (data);
        if (data.isNull () || data.isEmpty()) {
            LOG_DEBUG ("Nothing returned from server");
            syncStatus = Sync::SYNC_CONNECTION_ERROR;
            goto operationFailed;
        }

        GParseStream parser(false, mSyncTarget);
        GAtom* atom = parser.parse(data);
        if (!atom) {
            LOG_CRITICAL ("NULL atom object. Something wrong with parsing");
            goto operationFailed;
        }

        if ((requestType == GTransport::POST) ||
            (requestType == GTransport::PUT)) {
            LOG_DEBUG ("@@@PREVIOUS REQUEST TYPE=POST");
            // Check if there are any errors in the returned XML
            // buffer
            if (atom->title () == "Error") {
                LOG_CRITICAL ("Error in the request message. Check the data format");
                LOG_CRITICAL (data);
                // No need to signal error for the entire sync.
                // It might be a problem in a specific batch POST request
            }

            QList<GContactEntry*> remoteEntries = atom->entries();
            QList<UContactInfo> remoteContacts = toUContacts(remoteEntries);

            // Since we are POSTing a batch of MAX_RESULTS to
            // the server, we will continue with the next batch
            // if there are any
            bool hasMore = !mContactsToUpload.isEmpty();
            if (hasMore) {
                syncStatus = Sync::SYNC_PROGRESS;
            } else {
                //TODO: avatars
                syncStatus = Sync::SYNC_DONE;
                mState = GRemoteSource::STATE_IDLE;
            }

            emit contactsSaved(remoteContacts, syncStatus);

            if (hasMore) {
                saveContactsContinue();
            }
        } else if (requestType == GTransport::GET) {
            LOG_DEBUG ("@@@PREVIOUS REQUEST TYPE=GET");
            if (mState != GRemoteSource::STATE_FETCHING_CONTACTS) {
                LOG_WARNING("REceived a network request finish but the state is not fetching contacts" << mState);
                return;
            }

            QList<GContactEntry*> remoteEntries = atom->entries();
            QList<UContactInfo> remoteContacts = toUContacts(remoteEntries);
            bool hasMore = (!atom->nextEntriesUrl().isNull() ||
                            !atom->nextEntriesUrl().isEmpty());
            if (hasMore) {
                // Request for the next batch
                // This condition will make this slot to be
                // called again and again until there are no more
                // entries left to be fetched from the server
                mStartIndex += GConfig::MAX_RESULTS;
                mTransport->setUrl(atom->nextEntriesUrl());
                syncStatus = Sync::SYNC_PROGRESS;
                LOG_DEBUG("Has more contacts to retrieve");
            } else {
                LOG_DEBUG("NO contacts to retrieve");
                syncStatus = Sync::SYNC_DONE;
                mState = GRemoteSource::STATE_IDLE;
            }

            LOG_DEBUG("NOTIFY CONTACTS FETCHED:" << remoteContacts.size());
            emit contactsFetched(remoteContacts, syncStatus);

            if (hasMore) {
                LOG_DEBUG("FETCH MORE CONTACTS FROM INDEX:" << mStartIndex);
                fetchRemoteContacts(QDateTime(),
                                    mTransport->showDeleted(),
                                    mStartIndex);
            }
        }
        delete atom;
    }
    return;

operationFailed:
    switch(mState) {
    case GRemoteSource::STATE_FETCHING_CONTACTS:
        contactsFetched(QList<UContactInfo>(), syncStatus);
        break;
    case GRemoteSource::STATE_UPLOADING_CONTACTS:
        contactsSaved(QList<UContactInfo>(), syncStatus);
        break;
    default:
        break;
    }
    mState = GRemoteSource::STATE_IDLE;
}

void
GRemoteSource::networkError(int errorCode)
{
    FUNCTION_CALL_TRACE;

    Sync::SyncStatus syncStatus = Sync::SYNC_ERROR;

    // TODO: If interested, check the value of error. But
    // it is enough to say that it is a SYNC_CONNECTION_ERROR
    //emit syncFinished (Sync::SYNC_CONNECTION_ERROR);
    switch (errorCode)
    {
    case 400:
        // Bad request. Better to bail out, since it could be a problem with the
        // data format of the request/response
        syncStatus = Sync::SYNC_BAD_REQUEST;
        break;
    case 401:
        syncStatus = Sync::SYNC_AUTHENTICATION_FAILURE;
        break;
    case 403:
    case 408:
        syncStatus = Sync::SYNC_ERROR;
        break;
    case 500:
    case 503:
    case 504:
        // Server failures
        syncStatus = Sync::SYNC_SERVER_FAILURE;
        break;
    default:
        break;
    };

    switch(mState) {
    case GRemoteSource::STATE_FETCHING_CONTACTS:
        contactsFetched(QList<UContactInfo>(), syncStatus);
        break;
    case GRemoteSource::STATE_UPLOADING_CONTACTS:
        contactsSaved(QList<UContactInfo>(), syncStatus);
        break;
    default:
        break;
    }
    mState = GRemoteSource::STATE_IDLE;
}


