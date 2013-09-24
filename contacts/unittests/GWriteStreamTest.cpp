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
    GWriteStream ws(1);

    QContactManager *mgr = new QContactManager ("org.nemomobile.contacts.sqlite");
    QList<QContactId> contactList = mgr->contactIds ();
    std::cout << " Total Contacts with manager = " << contactList.count() << "\n";

    if (contactList.count() > 0) {
        QHash<QContactId, GConfig::TRANSACTION_TYPE> contactMap;
        contactMap.insert (contactList.at(0), GConfig::ADD);
        qDebug() << ws.encodeContact (contactMap);
        LOG_DEBUG (ws.encodeContact (contactMap));
        std::cout << ws.encodeContact (contactMap).constData () << "\n";
    }
}

void
GWriteStreamTest::testEncodeMultipleContacts ()
{
    GWriteStream ws(1);

    QContactManager *mgr = new QContactManager ("org.nemomobile.contacts.sqlite");
    QList<QContactId> contactList = mgr->contactIds ();

    QHash<QContactId, GConfig::TRANSACTION_TYPE> contactMap;
    if (contactList.count() > 2) {
        contactMap.insert (contactList.at (0), GConfig::ADD);
        contactMap.insert (contactList.at (1), GConfig::DELETE);
        contactMap.insert (contactList.at (2), GConfig::UPDATE);

        std::cout << "encoding multiple contacts";
        std::cout << ws.encodeContact (contactMap).constData() << "\n";
    }
}

void
GWriteStreamTest::testEncodeContacts ()
{
    GWriteStream ws(1);

    QContactManager *mgr = new QContactManager ("org.nemomobile.contacts.sqlite");
    QList<QContactId> contactList = mgr->contactIds ();

    ws.encodeContacts (contactList, GConfig::UPDATE);
    std::cout << ws.encodedStream ().constData () << "\n";
    qDebug() << ws.encodedStream ();
}

void
GWriteStreamTest::testEncodeAllContacts ()
{
    GWriteStream ws(1);
    ws.encodeAllContacts ();

    std::cout << ws.encodedStream ().constData () << "\n";
}
