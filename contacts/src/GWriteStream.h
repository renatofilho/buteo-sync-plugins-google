#ifndef GWRITESTREAM_H
#define GWRITESTREAM_H

#include "GConfig.h"

#include <UContactInfo.h>

#include <QContact>
#include <QXmlStreamWriter>

QTCONTACTS_USE_NAMESPACE

class GWriteStream
{
public:

    GWriteStream(const QString &accountDisplayName);
    ~GWriteStream();

    QByteArray encodeContacts(const QHash<UContactInfo, GConfig::TRANSACTION_TYPE> &qContactMap);

    void encodeContacts(const QList<QContactId> idList,
                        GConfig::TRANSACTION_TYPE type);

    QByteArray encodeContact(QHash<QContactId, GConfig::TRANSACTION_TYPE> qContactMap);
    QByteArray encodedStream();
    QList<QContactId>& contactsWithAvatars();

private:
    QByteArray encodeContact (const UContactInfo &uContact,
                              const GConfig::TRANSACTION_TYPE updateType,
                              const bool batch);
    void startBatchFeed ();
    void endBatchFeed ();
    void encodeBatchTag (const GConfig::TRANSACTION_TYPE type, const QString batchId);
    void encodeId (const UContactInfo &qContact);
    void encodeUpdated (const QContact qContact);
    void encodeEtag (const QContact qContact);
    void encodeCategory ();
    void encodeLink ();
    void encodeName (const QContactDetail& detail);
    void encodePhoneNumber (const QContactDetail& detail);
    void encodeEmail (const QContactDetail& detail);
    void encodeAddress (const QContactDetail& detail);
    void encodeUrl (const QContactDetail& detail);
    void encodeBirthDay (const QContactDetail& detail);
    void encodeNote (const QContactDetail& detail);
    void encodeHobby (const QContactDetail& detail);
    void encodeGeoLocation (const QContactDetail& detail);
    void encodeOrganization (const QContactDetail& detail);
    void encodeAvatar (const QContactDetail &detail, const UContactInfo &uContact);
    void encodeGender (const QContactDetail &detail);
    void encodeNickname (const QContactDetail &detail);
    void encodeAnniversary (const QContactDetail &detail);
    void encodeOnlineAccount (const QContactDetail &detail);
    void encodeFamily (const QContactDetail &detail);
    void encodeDisplayLabel (const QContactDetail &detail);
    void encodeExtendedDetail (const QContactDetail &detail);
    void encodeGroupMembership (const QString &groupName);

    QString          mAccountDisplayName;
    QByteArray       mXmlBuffer;
    QXmlStreamWriter mXmlWriter;
    QList<QContactId>   mContactsWithAvatars;
};

#endif // GWRITESTREAM_H
