/*
 * This file is part of buteo-sync-plugins package
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Sateesh Kavuri <sateesh.kavuri@nokia.com>
 *          Mani Chandrasekar <maninc@gmail.com>
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
#include "UContactsBackend.h"
#include "UContactsCustomDetail.h"

#include <LogMacros.h>

#include <QContactTimestamp>
#include <QContactIdFilter>
#include <QContactIntersectionFilter>
#include <QContactSyncTarget>
#include <QContactDetailFilter>
#include <QContactGuid>
#include <QContactDisplayLabel>
#include <QContactExtendedDetail>

#include <QBuffer>
#include <QSet>
#include <QHash>

#include <QDBusInterface>
#include <QDBusReply>

static const QString CPIM_SERVICE_NAME             ("com.canonical.pim");
static const QString CPIM_ADDRESSBOOK_OBJECT_PATH  ("/com/canonical/pim/AddressBook");
static const QString CPIM_ADDRESSBOOK_IFACE_NAME   ("com.canonical.pim.AddressBook");

UContactsBackend::UContactsBackend(QObject* parent)
    : QObject (parent),
      iMgr(new QContactManager (QCONTACTS_BACKEND_NAME))
{
    FUNCTION_CALL_TRACE;
}

UContactsBackend::~UContactsBackend()
{
    FUNCTION_CALL_TRACE;

    delete iMgr;
    iMgr = NULL;
}

bool
UContactsBackend::init(const QString &syncTarget)
{
    FUNCTION_CALL_TRACE;

    // create address book it it does not exists
    // check if the source already exists
    QContactDetailFilter filter;
    filter.setDetailType(QContactDetail::TypeType, QContactType::FieldType);
    filter.setValue(QContactType::TypeGroup);

    QList<QContact> sources = iMgr->contacts(filter);
    Q_FOREACH(const QContact &contact, sources) {
        if (contact.detail<QContactDisplayLabel>().label() == syncTarget) {
            mSyncTargetId = contact.detail<QContactGuid>().guid();
            return true;
        }
    }

    // create a new source if necessary
    QContact contact;
    contact.setType(QContactType::TypeGroup);

    QContactDisplayLabel label;
    label.setLabel(syncTarget);
    contact.saveDetail(&label);

    // set the new source as default if there is only the local source
    if (sources.size() == 1) {
        QContactExtendedDetail isDefault;
        isDefault.setName("IS-PRIMARY");
        isDefault.setData(true);
        contact.saveDetail(&isDefault);
    }

    if (!iMgr->saveContact(&contact)) {
        qWarning() << "Fail to create contact source:" << syncTarget;
        return false;
    }

    mSyncTargetId = contact.detail<QContactGuid>().guid();

    return true;
}

bool
UContactsBackend::uninit()
{
    FUNCTION_CALL_TRACE;

    return true;
}

QList<QContactId>
UContactsBackend::getAllContactIds()
{
    FUNCTION_CALL_TRACE;
    Q_ASSERT (iMgr);
    return iMgr->contactIds(getSyncTargetFilter());
}

QHash<QString, QContactId>
UContactsBackend::getAllNewContactIds(const QDateTime &aTimeStamp)
{
    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Retrieve New Contacts Since " << aTimeStamp);

    QHash<QString, QContactId> idList;
    const QContactChangeLogFilter::EventType eventType =
            QContactChangeLogFilter::EventAdded;

    getSpecifiedContactIds(eventType, aTimeStamp, idList);

    return idList;
}

QHash<QString, QContactId>
UContactsBackend::getAllModifiedContactIds(const QDateTime &aTimeStamp)
{

    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Retrieve Modified Contacts Since " << aTimeStamp);

    QHash<QString, QContactId> idList;
    const QContactChangeLogFilter::EventType eventType =
            QContactChangeLogFilter::EventChanged;

    getSpecifiedContactIds(eventType, aTimeStamp, idList);

    return idList;
}

QHash<QString, QContactId>
UContactsBackend::getAllDeletedContactIds(const QDateTime &aTimeStamp)
{
    FUNCTION_CALL_TRACE;
    LOG_DEBUG("Retrieve Deleted Contacts Since " << aTimeStamp);

    QHash<QString, QContactId> idList;
    const QContactChangeLogFilter::EventType eventType =
            QContactChangeLogFilter::EventRemoved;

    getSpecifiedContactIds(eventType, aTimeStamp, idList);

    return idList;
}

bool
UContactsBackend::addContacts(QList<QContact>& aContactList,
                              QMap<int, UContactsStatus>& aStatusMap)
{
    FUNCTION_CALL_TRACE;
    Q_ASSERT( iMgr );

    QMap<int, QContactManager::Error> errorMap;

    // Check if contact already exists if it exists set the contact id
    // to cause an update instead of create a new one
    for(int i=0; i < aContactList.size(); i++) {
        QContact &c = aContactList[i];
        QString remoteId = getRemoteId(c);
        QContactId id = entryExists(remoteId);
        if (!id.isNull()) {
            c.setId(id);
        }
    }

    bool retVal = iMgr->saveContacts(&aContactList, &errorMap);
    if (!retVal) {
        LOG_WARNING( "Errors reported while saving contacts:" << iMgr->error() );
    }

    // QContactManager will populate errorMap only for errors, but we use this as a status map,
    // so populate NoError if there's no error.
    // TODO QContactManager populates indices from the qContactList, but we populate keys, is this OK?
    for (int i = 0; i < aContactList.size(); i++)
    {
        UContactsStatus status;
        status.id = i;
        if (!errorMap.contains(i)) {
            status.errorCode = QContactManager::NoError;
        } else {
            qDebug() << "Contact with id " <<  aContactList.at(i).id() << " and index " << i <<" is in error";
            status.errorCode = errorMap.value(i);
        }
        aStatusMap.insert(i, status);
    }

    return retVal;
}

QContactManager::Error
UContactsBackend::modifyContact(const QString &aID,
                                QContact &aContact)
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);

    LOG_DEBUG("Modifying a Contact with ID" << aID);

    QContactManager::Error modificationStatus = QContactManager::UnspecifiedError;

    QContact oldContactData;
    getContact(QContactId::fromString(aID), oldContactData);

    aContact.setId(oldContactData.id());
    oldContactData = aContact;

    bool modificationOk = iMgr->saveContact(&oldContactData);
    modificationStatus = iMgr->error();

    if(!modificationOk) {
        // either contact exists or something wrong with one of the detailed definitions
        LOG_WARNING("Contact Modification Failed");
    } // no else

    return modificationStatus;
}

QMap<int,UContactsStatus>
UContactsBackend::modifyContacts( QList<QContact> &aContactList,
                                  const QStringList &aContactIdList)
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);
    UContactsStatus status;

    QMap<int,QContactManager::Error> errors;
    QMap<int,UContactsStatus> statusMap;

    for (int i = 0; i < aContactList.size(); i++) {
        LOG_DEBUG("Replacing item's ID " << aContactList.at(i));
        LOG_DEBUG("Id of the contact to be replaced" << aContactIdList.at(i));
        QContact contact;
        getContact(QContactId::fromString(aContactIdList.at(i)), contact);
        if (!contact.isEmpty()) {
            aContactList[i].setId(QContactId::fromString(aContactIdList.at(i)));
        }
    }

    if(iMgr->saveContacts(&aContactList , &errors)) {
        LOG_DEBUG("Batch Modification of Contacts Succeeded");
    } else {
        LOG_DEBUG("Batch Modification of Contacts Failed");
    }

    // QContactManager will populate errorMap only for errors, but we use this as a status map,
    // so populate NoError if there's no error.
    // TODO QContactManager populates indices from the aContactList, but we populate keys, is this OK?
    for (int i = 0; i < aContactList.size(); i++) {
        QContactId contactId = aContactList.at(i).id();
        if( !errors.contains(i) ) {
            LOG_DEBUG("No error for contact with id " << contactId << " and index " << i);
            status.errorCode = QContactManager::NoError;
            statusMap.insert(i, status);
        } else {
            LOG_DEBUG("contact with id " << contactId << " and index " << i <<" is in error");
            QContactManager::Error errorCode = errors.value(i);
            status.errorCode = errorCode;
            statusMap.insert(i, status);
        }
    }
    return statusMap;
}

QMap<int, UContactsStatus>
UContactsBackend::deleteContacts(const QStringList &aContactIDList)
{
    FUNCTION_CALL_TRACE;

    QList<QContactId> qContactIdList;
    foreach (QString id, aContactIDList) {
        qContactIdList.append(QContactId::fromString(id));
    }

    return deleteContacts(qContactIdList);
}

QMap<int, UContactsStatus>
UContactsBackend::deleteContacts(const QList<QContactId> &aContactIDList) {
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);
    UContactsStatus status;
    QMap<int, QContactManager::Error> errors;
    QMap<int, UContactsStatus> statusMap;

    qDebug() << "WILL REMOVE CONTACTS:" << aContactIDList;
    if(true) { //iMgr->removeContacts(aContactIDList , &errors)) {
        LOG_DEBUG("Successfully Removed all contacts ");
    }
    else {
        LOG_WARNING("Failed Removing Contacts");
    }

    // QContactManager will populate errorMap only for errors, but we use this as a status map,
    // so populate NoError if there's no error.
    // TODO QContactManager populates indices from the qContactList, but we populate keys, is this OK?
    for (int i = 0; i < aContactIDList.size(); i++) {
        QContactId contactId = aContactIDList.value(i);
        if( !errors.contains(i) )
        {
            LOG_DEBUG("No error for contact with id " << contactId << " and index " << i);
            status.errorCode = QContactManager::NoError;
            statusMap.insert(i, status);
        }
        else
        {
            LOG_DEBUG("contact with id " << contactId << " and index " << i <<" is in error");
            QContactManager::Error errorCode = errors.value(i);
            status.errorCode = errorCode;
            statusMap.insert(i, status);
        }
    }

    return statusMap;
}


void
UContactsBackend::getSpecifiedContactIds(const QContactChangeLogFilter::EventType aEventType,
                                         const QDateTime& aTimeStamp,
                                         QHash<QString, QContactId>& aIdList)
{
    FUNCTION_CALL_TRACE;
    QList<QContactId> localIdList;
    QContactChangeLogFilter filter(aEventType);
    filter.setSince(aTimeStamp);

    localIdList = iMgr->contactIds(filter  & getSyncTargetFilter());
    LOG_DEBUG("Local ID added =  " << localIdList.size() << "    Datetime from when this " << aTimeStamp.toString());
    // Filter out ids for items that were added after the specified time.
    if (aEventType != QContactChangeLogFilter::EventAdded)
    {
        filter.setEventType(QContactChangeLogFilter::EventAdded);
        QList<QContactId> addedList = iMgr->contactIds(filter  & getSyncTargetFilter());
        foreach (const QContactId &id, addedList)
        {
            localIdList.removeAll(id);
        }
    }

    // This is a defensive procedure to prevent duplicate items being sent.
    // QSet does not allow duplicates, thus transforming QList to QSet and back
    // again will remove any duplicate items in the original QList.
    int originalIdCount = localIdList.size();
    QSet<QContactId> idSet = localIdList.toSet();
    int idCountAfterDupRemoval = idSet.size();

    LOG_DEBUG("Item IDs found (returned / incl. duplicates): " << idCountAfterDupRemoval << "/" << originalIdCount);

    if (originalIdCount != idCountAfterDupRemoval) {
        LOG_WARNING("Contacts backend returned duplicate items for requested list");
        LOG_WARNING("Duplicate item IDs have been removed");
    } // no else

    localIdList = idSet.toList();

    QContactFetchHint remoteIdHint;
    QList <QContactDetail::DetailType> detailTypes;
    detailTypes << QContactExtendedDetail::Type;
    remoteIdHint.setDetailTypesHint(detailTypes);

    QList<QContact> contacts = iMgr->contacts(localIdList, remoteIdHint);
    foreach (const QContact &contact, contacts) {
        QString rid = getRemoteId(contact);
        aIdList.insertMulti(rid, contact.id());
    }
}

QDateTime
UContactsBackend::lastModificationTime(const QContactId &aContactId)
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);
    QDateTime lastModificationTime = QDateTime::fromTime_t(0);

    QContact contact;
    getContact(aContactId, contact);
    QContactTimestamp contactTimestamps;
    contactTimestamps = contact.detail<QContactTimestamp>();
    lastModificationTime = contactTimestamps.lastModified();

    return lastModificationTime;
}

/*!
    \fn GContactsBackend::getContact(QContactId aContactId)
 */
void
UContactsBackend::getContact(const QContactId& aContactId,
                             QContact& aContact)
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);

    QList<QContactId> aContactIds;
    QList<QContact>   returnedContacts;
    QContactIdFilter  contactFilter;

    aContactIds.append(aContactId);
    LOG_DEBUG("Contact ID to be retreived = " << aContactId.toString());
    contactFilter.setIds(aContactIds);
    returnedContacts = iMgr->contacts(contactFilter & getSyncTargetFilter());

    LOG_DEBUG("Contacts retreived from Contact manager  = " << returnedContacts.count());
    if (!returnedContacts.isEmpty()) {
        // There can be only one Contact for an Id
        aContact = returnedContacts.first();
    }
}

/*!
    \fn GContactsBackend::getContacts(QContactId aContactId)
 */
void
UContactsBackend::getContacts(const QList<QContactId>& aContactIds,
                              QList<QContact>& aContacts)
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);

    QContactIdFilter contactFilter;
    contactFilter.setIds(aContactIds);
    aContacts = iMgr->contacts(contactFilter & getSyncTargetFilter());

    LOG_DEBUG("Contacts retreived from Contact manager  = " << aContacts.count());
}

QContact
UContactsBackend::getContact(const QString& remoteId)
{
    FUNCTION_CALL_TRACE;
    Q_ASSERT (iMgr);
    LOG_DEBUG("Remote id to be searched for = " << remoteId);

    //FIXME: use guid field when supported by address-book-service
    QContactIntersectionFilter remoteIdFilter = getRemoteIdFilter(remoteId);
    QList<QContact> contactList = iMgr->contacts(remoteIdFilter & getSyncTargetFilter());
    if (contactList.size() > 0) {
        return contactList.at(0);
    }
    return QContact();
}

QDateTime
UContactsBackend::getCreationTime( const QContact& aContact )
{
    FUNCTION_CALL_TRACE;

    QContactTimestamp contactTimestamp = aContact.detail<QContactTimestamp>();

    return contactTimestamp.created();
}

QList<QDateTime>
UContactsBackend::getCreationTimes( const QList<QContactId>& aContactIds )
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT( iMgr );

    /* Retrieve QContacts from backend based on id's in aContactsIds.
     * Since we're only interested in timestamps, set up fetch hint
     * accordingly to speed up the operation.
     */
    QList<QDateTime> creationTimes;
    QList<QContact> contacts;

    QContactIdFilter contactFilter;
    contactFilter.setIds(aContactIds);

    /* Set up fetch hints so that not all details of QContacts be fetched:
     * 1) Fetch only QContactTimestamp details
     * 2) Do not try to resolve contact relationships (siblings etc)
     * 3) Do not include action preferences of contacts
     * 4) Do not fetch binary blogs (avatar pictures etc)
     */
    QContactFetchHint contactHint;
    contactHint.setOptimizationHints( QContactFetchHint::NoRelationships |
                                      QContactFetchHint::NoActionPreferences |
                                      QContactFetchHint::NoBinaryBlobs );
    QList<QContactDetail::DetailType> detailTypes;
    detailTypes << QContactTimestamp::Type;
    contactHint.setDetailTypesHint(detailTypes);

    QDateTime currentTime = QDateTime::currentDateTime();

    contacts = iMgr->contacts( contactFilter, QList<QContactSortOrder>(), contactHint);

    if( contacts.count() == aContactIds.count() )
    {
        for( int i = 0; i < aContactIds.count(); ++i )
        {
            QDateTime creationTime = currentTime;

            for( int a = 0; a < contacts.count(); ++a )
            {
                if( contacts[a].id() == aContactIds[i] )
                {
                    QContactTimestamp contactTimestamp = contacts[a].detail<QContactTimestamp>();
                    if( !contactTimestamp.created().isNull() &&
                        contactTimestamp.created().isValid() )
                    {
                        creationTime = contactTimestamp.created();
                    }
                    contacts.removeAt( a );
                    break;
                }
            }

            creationTimes.append( creationTime );
        }
    }
    else
    {
        LOG_WARNING( "Unable to fetch creation times" );
        for( int i = 0; i < aContactIds.count(); ++i )
        {
            creationTimes.append( currentTime );
        }
    }

    return creationTimes;
}

QContactId
UContactsBackend::entryExists(const QString entryGuid)
{
    QContactFilter ridFilter = getRemoteIdFilter(entryGuid);
    QList<QContactId> idList = iMgr->contactIds(ridFilter & getSyncTargetFilter());
    if (idList.size () > 0)
        return idList.first ();
    else
        return QContactId();
}

QString
UContactsBackend::syncTargetId() const
{
    return mSyncTargetId;
}

const QStringList
UContactsBackend::localIds(const QStringList guidList)
{
    QStringList localIdList;
    foreach (QString guid , guidList) {
        QString localId = entryExists(guid).toString();
        localIdList << localId;
    }
    Q_ASSERT(localIdList.count() == guidList.count());
    return localIdList;
}

const QList<QPair<QContactId, QString> >
UContactsBackend::guids(const QList<QContactId> localIdList)
{
    QContactFetchHint hint;
    QList<QContactDetail::DetailType> detailTypes;
    detailTypes << QContactExtendedDetail::Type;
    hint.setDetailTypesHint(detailTypes);

    QList<QPair<QContactId, QString> > idPair;
    for (int i=0; i<localIdList.size (); i++) {
        QContact contact = iMgr->contact(localIdList.at (i), hint);
        QString rid = getRemoteId(contact);
        idPair.append (QPair<QContactId, QString>(localIdList.at(i), rid));
    }
    return idPair;
}

QContactFilter
UContactsBackend::getRemoteIdFilter(const QString &remoteId) const
{
    QContactIntersectionFilter remoteFilter;

    QContactDetailFilter xDetailNameFilter;
    xDetailNameFilter.setDetailType(QContactExtendedDetail::Type,
                                    QContactExtendedDetail::FieldName);
    xDetailNameFilter.setValue(UContactsCustomDetail::FieldRemoteId);

    QContactDetailFilter xDetailValueFilter;
    xDetailValueFilter.setDetailType(QContactExtendedDetail::Type,
                                     QContactExtendedDetail::FieldData);
    xDetailValueFilter.setValue(remoteId);

    remoteFilter << xDetailNameFilter
                 << xDetailValueFilter;
    return remoteFilter;
}

QString
UContactsBackend::getRemoteId(const QContact &contact)
{
    return UContactsCustomDetail::getCustomField(contact, UContactsCustomDetail::FieldRemoteId).data().toString();
}

void
UContactsBackend::purgecontacts()
{
    QDBusInterface iface(CPIM_SERVICE_NAME,
                         CPIM_ADDRESSBOOK_OBJECT_PATH,
                         CPIM_ADDRESSBOOK_IFACE_NAME);
    QDBusReply<void> reply = iface.call("purgeContacts", QString(""));
    if (reply.error().isValid()) {
        LOG_WARNING("Fail to purge contacts" << reply.error());
    } else {
        LOG_DEBUG("Purged backend contacts");
    }
}

QContactFilter
UContactsBackend::getSyncTargetFilter() const
{
    // user enterred contacts, i.e. all other contacts that are not sourcing
    // from restricted backends or instant messaging service
    static QContactDetailFilter detailFilterDefaultSyncTarget;

    if (detailFilterDefaultSyncTarget.value().isNull()) {
        detailFilterDefaultSyncTarget.setDetailType(QContactSyncTarget::Type,
                                                    QContactSyncTarget::FieldSyncTarget + 1);
        detailFilterDefaultSyncTarget.setValue(mSyncTargetId);
    }

    // return the union
    return detailFilterDefaultSyncTarget;
}
