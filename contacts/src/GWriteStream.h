#ifndef GWRITESTREAM_H
#define GWRITESTREAM_H

#include <QContact>
#include <QXmlStreamWriter>

QTM_USE_NAMESPACE

class GWriteStream
{
public:

    typedef enum
    {
        NONE = 0,
        ADD,
        UPDATE,
        DELETE
    } TRANSACTION_TYPE;

    GWriteStream (const TRANSACTION_TYPE type);

    QByteArray encodeContact (const QList<QContact> qContactList);

    QByteArray encodeContact (const QContact qContact);

private:

    void encodeEntry ();
    void encodeId (const QContact qContact);
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
    void encodeGeoLocation (const QContactDetail& detail);
    void encodeOrganization (const QContactDetail& detail);
    void encodeAvatar (const QContactDetail &detail);
    void encodeGender (const QContactDetail &detail);
    void encodeNickname (const QContactDetail &detail);
    void encodeAnniversary (const QContactDetail &detail);
    void encodeOnlineAccount (const QContactDetail &detail);
    void encodeFamily (const QContactDetail &detail);
    void encodeDisplayLabel (const QContactDetail &detail);

    QByteArray       mXmlBuffer;

    QXmlStreamWriter mXmlWriter;

    TRANSACTION_TYPE mUpdateType;
};

#endif // GWRITESTREAM_H
