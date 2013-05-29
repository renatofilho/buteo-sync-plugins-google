#include "GWriteStreamTest.h"
#include "GWriteStream.h"
#include "GConfig.h"

#include <QPair>
#include <QDebug>
#include <LogMacros.h>
#include <iostream>

GWriteStreamTest::GWriteStreamTest(QObject* parent) :
    QObject (parent)
{
}

void
GWriteStreamTest::testEncodeContact ()
{
    GWriteStream ws;

    QContactManager mgr;
    QList<QContactLocalId> contactList = mgr.contactIds ();

    QHash<QContactLocalId, GConfig::TRANSACTION_TYPE> contactMap;
    contactMap.insert (contactList.at (1), GConfig::ADD);

    //qDebug() << ws.encodeContact (contactMap);
    //LOG_DEBUG (ws.encodeContact (contactMap));
    //std::cout << ws.encodeContact (contactMap).constData () << "\n";
}

void
GWriteStreamTest::testEncodeMultipleContacts ()
{
    GWriteStream ws;

    QContactManager mgr;
    QList<QContactLocalId> contactList = mgr.contactIds ();

    QHash<QContactLocalId, GConfig::TRANSACTION_TYPE> contactMap;
    contactMap.insert (contactList.at (1), GConfig::ADD);
    contactMap.insert (contactList.at (2), GConfig::DELETE);
    contactMap.insert (contactList.at (3), GConfig::UPDATE);

    //qDebug() << ws.encodeContact (contactMap);
    //LOG_DEBUG (ws.encodeContact (contactMap));
    std::cout << ws.encodeContact (contactMap).constData () << "\n";
}

void
GWriteStreamTest::testEncodeContacts ()
{
    GWriteStream ws;

    QContactManager mgr;
    QList<QContactLocalId> contactList = mgr.contactIds ();

    ws.encodeContacts (contactList, GConfig::UPDATE);
    //std::cout << ws.encodedStream ().constData () << "\n";
    //qDebug() << ws.encodedStream ();
}

void
GWriteStreamTest::testEncodeAllContacts ()
{
    GWriteStream ws;
    ws.encodeAllContacts ();

    //std::cout << ws.encodedStream ().constData () << "\n";
}
