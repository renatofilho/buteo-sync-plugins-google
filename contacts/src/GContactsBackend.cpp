/*
 * This file is part of buteo-sync-plugins package
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Sateesh Kavuri <sateesh.kavuri@nokia.com>
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
#include "GContactsBackend.h"
#include <LogMacros.h>
#include <QContactTimestamp>
#include <QContactLocalIdFilter>
#include <QContactSyncTarget>
#include <QContactDetailFilter>
#include <QContactGuid>
#include <QBuffer>
#include <QSet>
#include <QHash>

GContactsBackend::GContactsBackend(QObject* parent) :
                    QObject (parent), iMgr(new QContactManager ("org.nemomobile.contacts.sqlite"))
{
    FUNCTION_CALL_TRACE;
}

GContactsBackend::~GContactsBackend()
{
    FUNCTION_CALL_TRACE;

    delete iMgr;
    iMgr = NULL;
}

bool
GContactsBackend::init()
{
    FUNCTION_CALL_TRACE;

    return true;
}

bool
GContactsBackend::uninit()
{
    FUNCTION_CALL_TRACE;

    return true;
}

QList<QContactLocalId>
GContactsBackend::getAllContactIds()
{
    FUNCTION_CALL_TRACE;
    Q_ASSERT (iMgr);

    QList<QContactLocalId> idList = iMgr->contactIds ();
    return idList;
}

QHash<QString, QContactLocalId>
GContactsBackend::getAllNewContactIds(const QDateTime &aTimeStamp)
{
    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Retrieve New Contacts Since " << aTimeStamp);

    QHash<QString, QContactLocalId> idList;
    const QContactChangeLogFilter::EventType eventType =
            QContactChangeLogFilter::EventAdded;

    getSpecifiedContactIds(eventType, aTimeStamp, idList);

    return idList;
}

QHash<QString, QContactLocalId>
GContactsBackend::getAllModifiedContactIds(const QDateTime &aTimeStamp)
{

    FUNCTION_CALL_TRACE;

    LOG_DEBUG("Retrieve Modified Contacts Since " << aTimeStamp);

    QHash<QString, QContactLocalId> idList;
    const QContactChangeLogFilter::EventType eventType =
            QContactChangeLogFilter::EventChanged;

    getSpecifiedContactIds(eventType, aTimeStamp, idList);

    return idList;
}

QHash<QString, QContactLocalId>
GContactsBackend::getAllDeletedContactIds(const QDateTime &aTimeStamp)
{
    FUNCTION_CALL_TRACE;
    // Getting the following error while retrieving deleted contacts
    // So, it is not possible to sync deleted contacts

    // "Warning: libqtcontacts-tracker: querybuilder.cpp:2127:
    // QContactFilter::ChangeLogFilter: Unsupported event type:
    // QContactChangeLogFilter::EventRemoved"


    LOG_DEBUG("Retrieve Deleted Contacts Since " << aTimeStamp);

    QHash<QString, QContactLocalId> idList;
    const QContactChangeLogFilter::EventType eventType =
            QContactChangeLogFilter::EventRemoved;

    getSpecifiedContactIds(eventType, aTimeStamp, idList);

    return idList;
}

bool
GContactsBackend::addContacts( QList<QContact>& aContactList,
                               QMap<int, GContactsStatus>& aStatusMap )
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT( iMgr );

    GContactsStatus status;
    QMap<int, QContactManager::Error> errorMap;

    // TODO: Should a check be made for existing contacts based
    // on guid and filter them out?
    bool retVal = iMgr->saveContacts (&aContactList, &errorMap);

    if (!retVal)
    {
        LOG_WARNING( "Errors reported while saving contacts:" << iMgr->error() );
    }

    // QContactManager will populate errorMap only for errors, but we use this as a status map,
    // so populate NoError if there's no error.
    // TODO QContactManager populates indices from the qContactList, but we populate keys, is this OK?
    for (int i = 0; i < aContactList.size(); i++)
    {
        QContactLocalId contactId = aContactList.at(i).id().localId();
        if (!errorMap.contains(i))
        {
            status.id = (int)contactId;
            status.errorCode = QContactManager::NoError;
            aStatusMap.insert(i, status);
        }
        else
        {
            LOG_WARNING("Contact with id " << contactId << " and index " << i <<" is in error");
            QContactManager::Error errorCode = errorMap.value(i);
            status.id = (int)contactId;
            status.errorCode = errorCode;
            aStatusMap.insert(i, status);
        }
    }

    return retVal;
}

QContactManager::Error
GContactsBackend::modifyContact(const QString &aID,
                                QContact &aContact)
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);

    LOG_DEBUG("Modifying a Contact with ID" << aID);

    QContactManager::Error modificationStatus = QContactManager::UnspecifiedError;

    QContact oldContactData;
    getContact(aID.toUInt(), oldContactData);

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

QMap<int,GContactsStatus>
GContactsBackend::modifyContacts( QList<QContact> &aContactList,
                                  const QStringList &aContactIdList)
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);
    GContactsStatus status;

    QMap<int,QContactManager::Error> errors;
    QMap<int,GContactsStatus> statusMap;

    for (int i = 0; i < aContactList.size(); i++) {
        LOG_DEBUG("Id of the contact to be replaced" << aContactIdList.at(i));
        QContactLocalId managerLocalIdOfItemBeingModified = aContactIdList.at(i).toUInt();

        QContactId uniqueContactItemID;
        uniqueContactItemID.setLocalId(managerLocalIdOfItemBeingModified);
        aContactList[i].setId(uniqueContactItemID);

        LOG_DEBUG("Replacing item's ID " << aContactList.at(i).localId());
    }

    if(iMgr->saveContacts(&aContactList , &errors)) {
        LOG_DEBUG("Batch Modification of Contacts Succeeded");
    }
    else {
        LOG_DEBUG("Batch Modification of Contacts Failed");
    }

    // QContactManager will populate errorMap only for errors, but we use this as a status map,
    // so populate NoError if there's no error.
    // TODO QContactManager populates indices from the aContactList, but we populate keys, is this OK?
    for (int i = 0; i < aContactList.size(); i++) {
        QContactLocalId contactId = aContactList.at(i).id().localId();
        if( !errors.contains(i) )
        {
            LOG_DEBUG("No error for contact with id " << contactId << " and index " << i);
            status.id = (int)contactId;
            status.errorCode = QContactManager::NoError;
            statusMap.insert(i, status);
        }
        else
        {
            LOG_DEBUG("contact with id " << contactId << " and index " << i <<" is in error");
            QContactManager::Error errorCode = errors.value(i);
            status.id = (int)contactId;
            status.errorCode = errorCode;
            statusMap.insert(i, status);
        }
    }

    return statusMap;
}

QMap<int, GContactsStatus>
GContactsBackend::deleteContacts(const QStringList &aContactIDList)
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);
    GContactsStatus status;
    QMap<int, QContactManager::Error> errors;
    QMap<int, GContactsStatus> statusMap;

    QList<QContactLocalId> qContactIdList;
    foreach (QString id, aContactIDList )
    {
        qContactIdList.append(QContactLocalId(id.toUInt()));
    }

    if(iMgr->removeContacts(qContactIdList , &errors)) {
        LOG_DEBUG("Successfully Removed all contacts ");
    }
    else {
        LOG_WARNING("Failed Removing Contacts");
    }

    // QContactManager will populate errorMap only for errors, but we use this as a status map,
    // so populate NoError if there's no error.
    // TODO QContactManager populates indices from the qContactList, but we populate keys, is this OK?
    for (int i = 0; i < qContactIdList.size(); i++) {
        QContactLocalId contactId = qContactIdList.value(i);
        if( !errors.contains(i) )
        {
            LOG_DEBUG("No error for contact with id " << contactId << " and index " << i);
            status.id = (int)contactId;
            status.errorCode = QContactManager::NoError;
            statusMap.insert(i, status);
        }
        else
        {
            LOG_DEBUG("contact with id " << contactId << " and index " << i <<" is in error");
            QContactManager::Error errorCode = errors.value(i);
            status.id = (int)contactId;
            status.errorCode = errorCode;
            statusMap.insert(i, status);
        }
    }

    return statusMap;
}

void
GContactsBackend::getSpecifiedContactIds(const QContactChangeLogFilter::EventType aEventType,
        const QDateTime& aTimeStamp,
        QHash<QString, QContactLocalId>& aIdList)
{
    FUNCTION_CALL_TRACE;

    QList<QContactLocalId> localIdList;

    QContactChangeLogFilter filter(aEventType);
    filter.setSince(aTimeStamp);

    //localIdList = iMgr->contactIds(filter);
    localIdList = iMgr->contactIds(filter & getSyncTargetFilter());

    // Filter out ids for items that were added after the specified time.
    if (aEventType != QContactChangeLogFilter::EventAdded)
    {
        filter.setEventType(QContactChangeLogFilter::EventAdded);
        //QList<QContactLocalId> addedList = iMgr->contactIds (filter);
        QList<QContactLocalId> addedList = iMgr->contactIds(filter & getSyncTargetFilter());
        foreach (const QContactLocalId &id, addedList)
        {
            localIdList.removeAll(id);
        }
    }

    // This is a defensive procedure to prevent duplicate items being sent.
    // QSet does not allow duplicates, thus transforming QList to QSet and back
    // again will remove any duplicate items in the original QList.
    int originalIdCount = localIdList.size();
    QSet<QContactLocalId> idSet = localIdList.toSet();
    int idCountAfterDupRemoval = idSet.size();

    LOG_DEBUG("Item IDs found (returned / incl. duplicates): " << idCountAfterDupRemoval << "/" << originalIdCount);

    if (originalIdCount != idCountAfterDupRemoval) {
        LOG_WARNING("Contacts backend returned duplicate items for requested list");
        LOG_WARNING("Duplicate item IDs have been removed");
    } // no else

    localIdList = idSet.toList();

    QContactFetchHint guidHint;
    guidHint.setDetailDefinitionsHint (QStringList (QContactGuid::DefinitionName));

    QList<QContact> contacts = iMgr->contacts(localIdList, guidHint);

    foreach (QContact contact, contacts)
    {
        aIdList.insert (contact.detail<QContactGuid>().value (QContactGuid::FieldGuid),
                        contact.localId ());
    }
}

QDateTime
GContactsBackend::lastModificationTime(const QContactLocalId &aContactId)
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);
    QDateTime lastModificationTime = QDateTime::fromTime_t(0);

    QContact contact;
    getContact(aContactId, contact);
    QContactTimestamp contactTimestamps;
    QString definitionName = contactTimestamps.definitionName();
    contactTimestamps = (QContactTimestamp)contact.detail(definitionName);
    lastModificationTime = contactTimestamps.lastModified();

    return lastModificationTime;
}

/*!
    \fn GContactsBackend::getContact(QContactLocalId aContactId)
 */
void
GContactsBackend::getContact(const QContactLocalId& aContactId,
                             QContact& aContact)
{
    FUNCTION_CALL_TRACE;

    QList<QContactLocalId> contactId;
    contactId.append(aContactId);
    QList<QContact>        returnedContacts;

    getContacts(contactId, returnedContacts);

    if (!returnedContacts.isEmpty()) {
        aContact = returnedContacts.first();
    }
}

/*!
    \fn GContactsBackend::getContacts(QContactLocalId aContactId)
 */
void
GContactsBackend::getContacts(const QList<QContactLocalId>& aContactIds,
                              QList<QContact>& aContacts)
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);

    QContactLocalIdFilter contactFilter;
    contactFilter.setIds(aContactIds);

    aContacts = iMgr->contacts(contactFilter);
}

QDateTime
GContactsBackend::getCreationTime( const QContact& aContact )
{
    FUNCTION_CALL_TRACE;

    QContactTimestamp contactTimestamp = aContact.detail<QContactTimestamp>();

    return contactTimestamp.created();
}

QList<QDateTime>
GContactsBackend::getCreationTimes( const QList<QContactLocalId>& aContactIds )
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT( iMgr );

    /* Retrieve QContacts from backend based on id's in aContactsIds.
     * Since we're only interested in timestamps, set up fetch hint
     * accordingly to speed up the operation.
     */
    QList<QDateTime> creationTimes;
    QList<QContact> contacts;

    QContactLocalIdFilter contactFilter;
    contactFilter.setIds(aContactIds);

    QContactTimestamp contactTimestampDef;
    QString definitionName = contactTimestampDef.definitionName();

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

    QStringList definitionNames;
    definitionNames.append( definitionName );
    contactHint.setDetailDefinitionsHint( definitionNames );

    QDateTime currentTime = QDateTime::currentDateTime();

    contacts = iMgr->contacts( contactFilter, QList<QContactSortOrder>(), contactHint );

    if( contacts.count() == aContactIds.count() )
    {
        for( int i = 0; i < aContactIds.count(); ++i )
        {
            QDateTime creationTime = currentTime;

            for( int a = 0; a < contacts.count(); ++a )
            {
                if( contacts[a].id().localId() == aContactIds[i] )
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

QContactFilter
GContactsBackend::getSyncTargetFilter() const
{
    // user enterred contacts, i.e. all other contacts that are not sourcing
    // from restricted backends or instant messaging service
    QContactDetailFilter detailFilterDefaultSyncTarget;
    detailFilterDefaultSyncTarget.setDetailDefinitionName(QContactSyncTarget::DefinitionName,
                                         QContactSyncTarget::FieldSyncTarget);
    detailFilterDefaultSyncTarget.setValue(QLatin1String("buteo-google-contacts"));

    // return the union
    return detailFilterDefaultSyncTarget;
}

QContactLocalId
GContactsBackend::entryExists (const QString entryGuid)
{
    /*
    QContactFetchHint hint;
    hint.setDetailDefinitionsHint (QStringList(QContactGuid::DefinitionName));
    */

    QContactDetailFilter guidFilter;
    guidFilter.setDetailDefinitionName (QContactGuid::DefinitionName);
    guidFilter.setValue (entryGuid);
    guidFilter.setMatchFlags (QContactFilter::MatchExactly);

    QList<QContactLocalId> idList = iMgr->contactIds (guidFilter & getSyncTargetFilter ());

    if (idList.size () > 0)
        return idList.first ();
    else
        return 0;
}

const
QStringList GContactsBackend::localIds(const QStringList guidList)
{
    QContactFetchHint hint;
    hint.setDetailDefinitionsHint (QStringList(QContactGuid::DefinitionName));

    QContactDetailFilter guidFilter;
    guidFilter.setDetailDefinitionName (QContactGuid::DefinitionName);
    guidFilter.setMatchFlags (QContactFilter::MatchExactly);

    QStringList localIdList;
    for (int i=0; i<guidList.size (); i++)
    {
        guidFilter.setValue (guidList.at (i));
        QList<QContact> tempContacts =
                iMgr->contacts (guidFilter & getSyncTargetFilter (), QList<QContactSortOrder>(), hint);
        if (tempContacts.size () > 0)
            localIdList.append (QString::number (tempContacts.at (0).localId ()));
    }

    return localIdList;
}

const QList<QPair<QContactLocalId, QString> >
GContactsBackend::guids (const QList<QContactLocalId> localIdList)
{
    QContactFetchHint hint;
    hint.setDetailDefinitionsHint (QStringList (QContactGuid::DefinitionName));

    QList<QPair<QContactLocalId, QString> > idPair;
    for (int i=0; i<localIdList.size (); i++)
    {
        QString guid = iMgr->contact (localIdList.at (i), hint).detail<QContactGuid>().value (QContactGuid::FieldGuid);
        idPair.append (QPair<QContactLocalId, QString>(localIdList.at (i), guid));
    }
    return idPair;
}
