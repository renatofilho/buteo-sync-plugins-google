#include "GWriteStreamTest.h"
#include "GWriteStream.h"
#include "GConfig.h"

#include <QPair>
#include <QDebug>

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

    qDebug() << ws.encodeContact (contactMap);
}

void
GWriteStreamTest::testEncodeContacts ()
{
    GWriteStream ws;

    QContactManager mgr;
    QList<QContactLocalId> contactList = mgr.contactIds ();

    ws.encodeContacts (contactList, GConfig::UPDATE);
    qDebug() << ws.encodedStream ();
}

void
GWriteStreamTest::encodeAllContacts ()
{
    GWriteStream ws;
    ws.encodeAllContacts ();

    qDebug() << ws.encodedStream ();
}
