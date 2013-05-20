#include "GWriteStreamTest.h"
#include "GWriteStream.h"

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
    QList<QContact> contactList = mgr.contacts ();

    QList<QPair<QContact, GWriteStream::TRANSACTION_TYPE> > qContactList;
    QPair<QContact, GWriteStream::TRANSACTION_TYPE> contactPair;
    contactPair.first = contactList.at (1);
    contactPair.second = GWriteStream::ADD;
    qContactList.append (contactPair);

    qDebug() << ws.encodeContact (qContactList);
}

void
GWriteStreamTest::testEncodeContacts ()
{
    GWriteStream ws;

    QContactManager mgr;
    QList<QContact> contactList = mgr.contacts ();

    QList<QPair<QContact, GWriteStream::TRANSACTION_TYPE> > qContactList;
    QPair<QContact, GWriteStream::TRANSACTION_TYPE> contactPair;
    for (int i=0; i<contactList.size (); i++)
    {
        contactPair.first = contactList.at (i);
        contactPair.second = GWriteStream::ADD;
        qContactList.append (contactPair);
    }
    qDebug() << ws.encodeContact (qContactList);
}

void
GWriteStreamTest::testEncodeContactWithIds ()
{
    GWriteStream ws;

    QContactManager mgr;
    QList<QContactLocalId> idList = mgr.contactIds ();
    idList.removeFirst ();
    ws.encodeContacts (idList, GWriteStream::UPDATE);

    qDebug() << ws.encodedStream ();
}

void
GWriteStreamTest::encodeAllContacts ()
{
    GWriteStream ws;
    ws.encodeAllContacts ();

    qDebug() << ws.encodedStream ();
}
