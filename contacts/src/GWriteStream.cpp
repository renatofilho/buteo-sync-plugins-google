#include "GWriteStream.h"
#include "GContactCustomDetail.h"

#include <QXmlStreamWriter>
#include <QContactUrl>
#include <QContactAnniversary>
#include <QContact>
#include <QContactGuid>
#include <QContactName>
#include <QContactEmailAddress>
#include <QContactBirthday>
#include <QContactGender>
#include <QContactHobby>
#include <QContactNickname>
#include <QContactNote>
#include <QContactOnlineAccount>
#include <QContactOrganization>
#include <QContactPhoneNumber>
#include <QContactAddress>
#include <QContactAvatar>
#include <QContactFamily>
#include <QContactTimestamp>

GWriteStream::GWriteStream (const TRANSACTION_TYPE type) :
    mXmlWriter (&mXmlBuffer), mUpdateType (type)
{
}

QByteArray
GWriteStream::encodeContact (const QList<QContact> qContactList)
{
    mXmlWriter.writeStartDocument ();
    foreach (const QContact& contact, qContactList)
    {
        encodeContact (contact);
    }
    mXmlWriter.writeEndDocument ();

    return mXmlBuffer;
}

QByteArray
GWriteStream::encodeContact (const QContact qContact)
{
    QList<QContactDetail> allDetails = qContact.details ();
    if (allDetails.size () <= 0)
        return mXmlBuffer;

    encodeEntry ();

    if (mUpdateType == ADD)
    {
    } else if (mUpdateType == UPDATE)
    {
        // Etag encoding has to immediately succeed writeStartElement ("atom:entry"),
        // since etag is an attribute of this element
        encodeEtag (qContact);
        encodeId (qContact);
        encodeUpdated (qContact);
    } else if (mUpdateType == DELETE)
    {
    }

    encodeCategory ();

    foreach (const QContactDetail& detail, allDetails)
    {
        if (detail.definitionName () == QContactName::DefinitionName)
            encodeName (detail);
        else if (detail.definitionName () == QContactPhoneNumber::DefinitionName)
            encodePhoneNumber (detail);
        else if (detail.definitionName () == QContactEmailAddress::DefinitionName)
            encodeEmail (detail);
        else if (detail.definitionName () == QContactAddress::DefinitionName)
            encodeAddress (detail);
        else if (detail.definitionName () == QContactUrl::DefinitionName)
            encodeUrl (detail);
        else if (detail.definitionName () == QContactBirthday::DefinitionName)
            encodeBirthDay (detail);
        else if (detail.definitionName () == QContactNote::DefinitionName)
            encodeNote (detail);
        else if (detail.definitionName () == QContactOrganization::DefinitionName)
            encodeOrganization (detail);
        else if (detail.definitionName () == QContactAvatar::DefinitionName)
            encodeAvatar (detail);
        else if (detail.definitionName () == QContactAnniversary::DefinitionName)
            encodeAnniversary (detail);
        else if (detail.definitionName () == QContactNickname::DefinitionName)
            encodeNickname (detail);
        else if (detail.definitionName () == QContactGender::DefinitionName)
            encodeGender (detail);
        else if (detail.definitionName () == QContactOnlineAccount::DefinitionName)
            encodeOnlineAccount(detail);
        else if (detail.definitionName () == QContactFamily::DefinitionName)
            encodeFamily (detail);
        // TODO: handle the custom detail fields. If sailfish UI
        // does not handle these contact details, then they
        // need not be pushed to the server
    }
    mXmlWriter.writeEndElement ();

    return mXmlBuffer;
}

void
GWriteStream::encodeEntry ()
{
    mXmlWriter.writeStartElement ("atom:entry");
    mXmlWriter.writeAttribute ("xmlns:gd", "http://schemas.google.com/g/2005");
    mXmlWriter.writeAttribute ("xmlns:gContact", "http://schemas.google.com/contact/2008");
}

void
GWriteStream::encodeId (const QContact qContact)
{
    QString guid = qContact.detail (
        QContactGuid::DefinitionName).value (QContactGuid::FieldGuid);

    Q_ASSERT (guid.isNull ());
    mXmlWriter.writeTextElement ("id", "http://www.google.com/m8/feeds/contacts/default/base/" + guid);
}

void
GWriteStream::encodeUpdated (const QContact qContact)
{
    QString updated = qContact.detail (
        QContactTimestamp::DefinitionName).value (QContactTimestamp::FieldModificationTimestamp);

    Q_ASSERT (updated.isNull ());
    mXmlWriter.writeTextElement ("updated", updated);
}

void
GWriteStream::encodeEtag (const QContact qContact)
{
    QString etag = qContact.detail (
        GContactCustomDetail::DefinitionName).value (GContactCustomDetail::FieldGContactETag);

    Q_ASSERT (etag.isNull ());
    mXmlWriter.writeAttribute ("gd:etag", etag);
}

void
GWriteStream::encodeCategory ()
{
    mXmlWriter.writeEmptyElement ("atom:category");
    mXmlWriter.writeAttribute ("schema", "http://schemas.google.com/g/2005#kind");
    mXmlWriter.writeAttribute ("term", "http://schemas.google.com/contact/2008#contact");
}

void
GWriteStream::encodeLink ()
{
}

void
GWriteStream::encodeName (const QContactDetail& detail)
{
    QContactName contactName = static_cast<QContactName>(detail);
    if (!contactName.lastName().isEmpty()
        || !contactName.firstName().isEmpty()
        || !contactName.middleName().isEmpty()
        || !contactName.prefix().isEmpty()
        || !contactName.suffix().isEmpty()) {
        mXmlWriter.writeStartElement ("gd:name");
        mXmlWriter.writeTextElement ("gd:givenName", contactName.firstName ());
        mXmlWriter.writeTextElement ("gd:additionalName", contactName.middleName ());
        mXmlWriter.writeTextElement ("gd:familyName", contactName.lastName ());
        mXmlWriter.writeTextElement ("gd:namePrefix", contactName.prefix ());
        mXmlWriter.writeTextElement ("gd:nameSuffix", contactName.suffix ());
        mXmlWriter.writeEndElement ();
    }
}

/*!
 * Encode Phone Number Field Information into the Google contact XML Document
 */
void
GWriteStream::encodePhoneNumber (const QContactDetail& detail)
{
    QContactPhoneNumber phoneNumber = static_cast<QContactPhoneNumber>(detail);
    QStringList subTypes = phoneNumber.subTypes();
    mXmlWriter.writeStartElement ("gd:phoneNumber");

    QString rel = "http://schemas.google.com/g/2005#";
    // TODO: Handle subtype encoding properly
    if (subTypes.contains(QContactPhoneNumber::SubTypeAssistant))
        mXmlWriter.writeAttribute ("rel", rel + "assistant");
    else
        mXmlWriter.writeAttribute ("rel", rel + "mobile");

    mXmlWriter.writeCharacters (phoneNumber.number());
    mXmlWriter.writeEndElement ();
}

/*!
 * Encode Email Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeEmail (const QContactDetail& detail)
{
    QContactEmailAddress emailAddress = static_cast<QContactEmailAddress>(detail);

    // TODO: Handle 'rel' attribute, which is subtype
    mXmlWriter.writeEmptyElement ("gd:email");
    mXmlWriter.writeAttribute ("address", emailAddress.emailAddress ());
}

/*!
 * Encode Address Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeAddress (const QContactDetail& detail)
{
    QContactAddress address = static_cast<QContactAddress>(detail);

    mXmlWriter.writeStartElement ("gd:structuredPostalAddress");
    mXmlWriter.writeTextElement ("gd:street", address.street ());
    mXmlWriter.writeTextElement ("gd:neighborhood", address.locality ());
    mXmlWriter.writeTextElement ("gd:pobox", address.postOfficeBox ());
    mXmlWriter.writeTextElement ("gd:region", address.region ());
    mXmlWriter.writeTextElement ("gd:postcode", address.postcode ());
    mXmlWriter.writeTextElement ("gd:country", address.country ());
    mXmlWriter.writeEndElement ();
}

/*!
 * Encode URL Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeUrl (const QContactDetail& detail)
{
    QContactUrl contactUrl = static_cast<QContactUrl>(detail);
    mXmlWriter.writeEmptyElement ("gContact:website");
    mXmlWriter.writeAttribute ("rel", "home-page");
    mXmlWriter.writeAttribute ("href", contactUrl.url ());
}

/*!
 * Encode BirthDay Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeBirthDay (const QContactDetail& detail)
{
    QContactBirthday bday = static_cast<QContactBirthday>(detail);
    QVariant variant = bday.variantValue(QContactBirthday::FieldBirthday);
    QString value;
    if (variant.type() == QVariant::Date) {
        value = variant.toDate().toString(Qt::ISODate);
    } else if (variant.type() == QVariant::DateTime) {
        value = variant.toDateTime().toString(Qt::ISODate);
    } else {
        return;
    }

    mXmlWriter.writeEmptyElement ("gContact:birthday");
    mXmlWriter.writeAttribute ("when", value);
}

/*!
 * Encode Comment i.e. Note Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeNote (const QContactDetail& detail)
{
    QContactNote contactNote = static_cast<QContactNote>(detail);
    mXmlWriter.writeEmptyElement ("atom:content");
    mXmlWriter.writeAttribute ("type", "text");
    mXmlWriter.writeCharacters (contactNote.note ());
}

/*!
 * Encode Geo Prpoperties Field Information into the Google contact XML Document
 */
void
GWriteStream::encodeGeoLocation (const QContactDetail& detail)
{
    /* TODO: Need to check how to handle GeoRSS geo location
    QContactGeoLocation geoLocation = static_cast<QContactGeoLocation>(detail);
    property.setValue(QStringList() << QString::number(geoLocation.latitude())
                      << QString::number(geoLocation.longitude()));
    */
}

/*!
 * Encode organization properties to the versit document
 */
void
GWriteStream::encodeOrganization (const QContactDetail& detail)
{
    QContactOrganization organization = static_cast<QContactOrganization>(detail);
    mXmlWriter.writeStartElement ("gd:organization");
    if (organization.title().length () > 0)
        mXmlWriter.writeTextElement ("gd:orgTitle", organization.title ());
    if (organization.name().length() > 0 )
        mXmlWriter.writeTextElement ("gd:orgName", organization.name ());
    if (organization.department ().length () > 0)
    {
        QString departmets;
        foreach (const QString dept, organization.department ())
            departmets.append (dept);

        mXmlWriter.writeTextElement ("gd:orgDepartment", departmets);
    }

    mXmlWriter.writeEndElement ();
}

/*!
 * Encode avatar URIs into the Google contact XML Document
 */
void
GWriteStream::encodeAvatar (const QContactDetail &detail)
{
    /*
    property.setName(QLatin1String("PHOTO"));
    QContactAvatar contactAvatar = static_cast<QContactAvatar>(detail);
    QUrl imageUrl(contactAvatar.imageUrl());
    // XXX: fix up this mess: checking the scheme here and in encodeContentFromFile,
    // organisation logo and ringtone are QStrings but avatar is a QUrl
    if (!imageUrl.scheme().isEmpty()
            && !imageUrl.host().isEmpty()
            && imageUrl.scheme() != QLatin1String("file")) {
        property.insertParameter(QLatin1String("VALUE"), QLatin1String("URL"));
        property.setValue(imageUrl.toString());
        *generatedProperties << property;
        *processedFields << QContactAvatar::FieldImageUrl;
    } else {
        if (encodeContentFromFile(contactAvatar.imageUrl().toLocalFile(), property)) {
            *generatedProperties << property;
            *processedFields << QContactAvatar::FieldImageUrl;
        }
    }
    */
}

/*!
 * Encode gender property information into Google contact XML Document
 */
void
GWriteStream::encodeGender (const QContactDetail &detail)
{
    QContactGender gender = static_cast<QContactGender>(detail);
    mXmlWriter.writeEmptyElement ("gContact:gender");
    mXmlWriter.writeCharacters (gender.gender ());
}

/*!
 * Encodes nickname property information into the Google contact XML Document
 */
void
GWriteStream::encodeNickname (const QContactDetail &detail)
{
    QContactNickname nicknameDetail = static_cast<QContactNickname>(detail);
    mXmlWriter.writeTextElement ("gContact:nickname", nicknameDetail.nickname ());
}

/*!
 * Encode anniversary information into Google contact XML Document
 */
void
GWriteStream::encodeAnniversary (const QContactDetail &detail)
{
    QContactAnniversary anniversary = static_cast<QContactAnniversary>(detail);
    mXmlWriter.writeStartElement ("gContact:event");
    if ((anniversary.event () == "anniversary") || (anniversary.event () == "other"))
        mXmlWriter.writeAttribute ("rel", anniversary.event ());
    mXmlWriter.writeTextElement ("gd:when", anniversary.originalDate ().toString (Qt::ISODate));
    mXmlWriter.writeEndElement ();
}

/*!
 * Encode online account information into the Google contact XML Document
 */
void
GWriteStream::encodeOnlineAccount (const QContactDetail &detail)
{
    QContactOnlineAccount onlineAccount = static_cast<QContactOnlineAccount>(detail);
    QStringList subTypes = onlineAccount.subTypes();
    QString protocol = onlineAccount.protocol();

    QString propertyName;

    if (protocol == QContactOnlineAccount::ProtocolJabber) {
        propertyName = "JABBER";
    } else if (protocol == QContactOnlineAccount::ProtocolAim) {
        propertyName = "AIM";
    } else if (protocol == QContactOnlineAccount::ProtocolIcq) {
        propertyName = "ICQ";
    } else if (protocol == QContactOnlineAccount::ProtocolMsn) {
        propertyName = "MSN";
    } else if (protocol == QContactOnlineAccount::ProtocolQq) {
        propertyName = "QQ";
    } else if (protocol == QContactOnlineAccount::ProtocolYahoo) {
        propertyName = "YAHOO";
    } else if (protocol == QContactOnlineAccount::ProtocolSkype) {
        propertyName = "SKYPE";
    } else
        propertyName = protocol;

    mXmlWriter.writeEmptyElement ("gd:im");
    mXmlWriter.writeAttribute ("protocol", "http://schemas.google.com/g/2005#" + propertyName);
    mXmlWriter.writeAttribute ("address", onlineAccount.accountUri ());
}

/*!
 * Encode family versit property if its supported in Google contact XML Document
 */
void
GWriteStream::encodeFamily (const QContactDetail &detail)
{
    QContactFamily family = static_cast<QContactFamily>(detail);

    if (family.spouse().length () > 0) {
        mXmlWriter.writeEmptyElement ("gContact:relation");
        mXmlWriter.writeAttribute ("rel", "spouse");
        mXmlWriter.writeCharacters (family.spouse ());
    }

    foreach (const QString member, family.children ())
    {
        mXmlWriter.writeEmptyElement ("gContact:relation");
        mXmlWriter.writeAttribute ("rel", "child");
        mXmlWriter.writeCharacters (member);
    }
}
