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
#include "GContactsBackend.h"
#include <LogMacros.h>
#include <QContactTimestamp>
#include <QContactIdFilter>
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

QList<QContactId>
GContactsBackend::getAllContactIds()
{
    FUNCTION_CALL_TRACE;
    Q_ASSERT (iMgr);

    QList<QContactId> idList = iMgr->contactIds ();
    return idList;
}

QHash<QString, QContactId>
GContactsBackend::getAllNewContactIds(const QDateTime &aTimeStamp)
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
GContactsBackend::getAllModifiedContactIds(const QDateTime &aTimeStamp)
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
GContactsBackend::getAllDeletedContactIds(const QDateTime &aTimeStamp)
{
    FUNCTION_CALL_TRACE;
    // Getting the following error while retrieving deleted contacts
    // So, it is not possible to sync deleted contacts

    // "Warning: libqtcontacts-tracker: querybuilder.cpp:2127:
    // QContactFilter::ChangeLogFilter: Unsupported event type:
    // QContactChangeLogFilter::EventRemoved"


    LOG_DEBUG("Retrieve Deleted Contacts Since " << aTimeStamp);

    QHash<QString, QContactId> idList;
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
        QContactId contactId = aContactList.at(i).id();
        if (!errorMap.contains(i))
        {
            status.errorCode = QContactManager::NoError;
            aStatusMap.insert(i, status);
        }
        else
        {
            qDebug() << "Contact with id " << contactId << " and index " << i <<" is in error";
            QContactManager::Error errorCode = errorMap.value(i);
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
        LOG_DEBUG("Replacing item's ID " << aContactList.at(i));
        LOG_DEBUG("Id of the contact to be replaced" << aContactIdList.at(i));
        aContactList[i].setId(QContactId::fromString(aContactIdList.at(i)));
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
        QContactId contactId = aContactList.at(i).id();
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

QMap<int, GContactsStatus>
GContactsBackend::deleteContacts(const QStringList &aContactIDList)
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);
    GContactsStatus status;
    QMap<int, QContactManager::Error> errors;
    QMap<int, GContactsStatus> statusMap;

    QList<QContactId> qContactIdList;
    foreach (QString id, aContactIDList )
    {
        qContactIdList.append(QContactId::fromString(id));
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
        QContactId contactId = qContactIdList.value(i);
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
GContactsBackend::getSpecifiedContactIds(const QContactChangeLogFilter::EventType aEventType,
        const QDateTime& aTimeStamp,
        QHash<QString, QContactId>& aIdList)
{
    FUNCTION_CALL_TRACE;

    QList<QContactId> localIdList;

    QContactChangeLogFilter filter(aEventType);
    filter.setSince(aTimeStamp);

    localIdList = iMgr->contactIds(filter & getSyncTargetFilter());

    // Filter out ids for items that were added after the specified time.
    if (aEventType != QContactChangeLogFilter::EventAdded)
    {
        filter.setEventType(QContactChangeLogFilter::EventAdded);
        QList<QContactId> addedList = iMgr->contactIds(filter & getSyncTargetFilter());
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

    QContactFetchHint guidHint;
    QList <QContactDetail::DetailType> detailTypes;
    detailTypes << QContactGuid::Type;
    guidHint.setDetailTypesHint(detailTypes);

    QList<QContact> contacts = iMgr->contacts(localIdList, guidHint);

    foreach (QContact contact, contacts)
    {
        aIdList.insert (contact.detail<QContactGuid>().value(QContactGuid::Type).toString(),
                        contact.id ());
    }
}

QDateTime
GContactsBackend::lastModificationTime(const QContactId &aContactId)
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
GContactsBackend::getContact(const QContactId& aContactId,
                             QContact& aContact)
{
    FUNCTION_CALL_TRACE;

    QList<QContactId> contactId;
    contactId.append(aContactId);
    LOG_DEBUG("Contact ID to be retreived = " << aContactId.toString());
    QList<QContact>        returnedContacts;

    getContacts(contactId, returnedContacts);

    if (!returnedContacts.isEmpty()) {
        aContact = returnedContacts.first();
    }
}

/*!
    \fn GContactsBackend::getContacts(QContactId aContactId)
 */
void
GContactsBackend::getContacts(const QList<QContactId>& aContactIds,
                              QList<QContact>& aContacts)
{
    FUNCTION_CALL_TRACE;

    Q_ASSERT (iMgr);

    QContactIdFilter contactFilter;
    contactFilter.setIds(aContactIds);    
    aContacts = iMgr->contacts(contactFilter & getSyncTargetFilter());

    LOG_DEBUG("Contacts retreived from Contact manager  = " << aContacts.count());
}

QContact GContactsBackend::getContact(const QString& aGuid) {
    FUNCTION_CALL_TRACE;
    Q_ASSERT (iMgr);
    LOG_DEBUG("GUID to be searched for = " << aGuid);

    QContact retContact;
    QtContacts::QContactDetailFilter guidFilter;
    guidFilter.setDetailType (QtContacts::QContactDetail::TypeGuid);
    guidFilter.setValue (aGuid);
    guidFilter.setMatchFlags (QtContacts::QContactFilter::MatchExactly);

    QList<QContact> contactList = iMgr->contacts(guidFilter & getSyncTargetFilter());

    foreach(QContact contact , contactList) {
        QString guid = contact.detail<QtContacts::QContactGuid>().guid();
        if (guid.compare(aGuid) == 0) {
            retContact = contact;
            break;
        }
    }

    return retContact;
}

QDateTime
GContactsBackend::getCreationTime( const QContact& aContact )
{
    FUNCTION_CALL_TRACE;

    QContactTimestamp contactTimestamp = aContact.detail<QContactTimestamp>();

    return contactTimestamp.created();
}

QList<QDateTime>
GContactsBackend::getCreationTimes( const QList<QContactId>& aContactIds )
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

QContactFilter
GContactsBackend::getSyncTargetFilter() const
{
    // user enterred contacts, i.e. all other contacts that are not sourcing
    // from restricted backends or instant messaging service
    QContactDetailFilter detailFilterDefaultSyncTarget;
    detailFilterDefaultSyncTarget.setDetailType(QContactSyncTarget::Type,
                                                QContactSyncTarget::FieldSyncTarget);
    detailFilterDefaultSyncTarget.setValue(QLatin1String("buteo"));

    // return the union
    return detailFilterDefaultSyncTarget;
}

QContactId
GContactsBackend::entryExists (const QString entryGuid)
{
    QContactDetailFilter guidFilter;
    guidFilter.setDetailType(QContactGuid::Type);
    guidFilter.setMatchFlags (QContactFilter::MatchExactly);
    guidFilter.setValue (entryGuid);

    QList<QContactId> idList = iMgr->contactIds (guidFilter & getSyncTargetFilter ());

    if (idList.size () > 0)
        return idList.first ();
    else
        return QContactId();
}

const
QStringList GContactsBackend::localIds(const QStringList guidList)
{
    QStringList localIdList;
    foreach (QString guid , guidList) {
        localIdList << entryExists(guid).toString();
    }
    Q_ASSERT(localIdList.count() == guidList.count());
    return localIdList;
}

const QList<QPair<QContactId, QString> >
GContactsBackend::guids (const QList<QContactId> localIdList)
{
    QContactFetchHint hint;
    QList<QContactDetail::DetailType> detailTypes;
    detailTypes << QContactGuid::Type;
    hint.setDetailTypesHint(detailTypes);

    QList<QPair<QContactId, QString> > idPair;
    for (int i=0; i<localIdList.size (); i++)
    {
        QString guid = iMgr->contact (localIdList.at (i), hint).detail<QContactGuid>().value (QContactGuid::FieldGuid).toString();
        idPair.append (QPair<QContactId, QString>(localIdList.at (i), guid));
    }
    return idPair;
}
