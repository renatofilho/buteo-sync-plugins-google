#include "GWriteStreamTest.h"
#include "GWriteStream.h"

#include <QDebug>

GWriteStreamTest::GWriteStreamTest(QObject* parent) :
    QObject (parent)
{
}

void
GWriteStreamTest::testEncodeContact ()
{
    GWriteStream ws (GWriteStream::ADD);

    QContactManager mgr;
    QList<QContact> contactList = mgr.contacts ();

   qDebug() << ws.encodeContact (contactList.at (0));
}

void
GWriteStreamTest::testEncodeContacts ()
{
    GWriteStream ws (GWriteStream::ADD);

    QContactManager mgr;
    QList<QContact> contactList = mgr.contacts ();

   qDebug() << ws.encodeContact (contactList);
}
