#include "GTransport.h"

const int MAX_RESULTS = 10;
const QString SCOPE_URL("https://www.google.com/m8/feeds/");
const QString GCONTACT_URL(SCOPE_URL + "/contacts/default/");
const QString GDATA_VERSION_HEADER("GData-Version: 3.0");
const QString DELETE_OVERRIDE_HEADER("X-HTTP-Method-Override: DELETE");
const QString ETAG_HEADER("If-Match: Etag");

const QString JSON_ALT("alt=json");
const QString UPDATED_MIN_TAG("updated-min");
const QString PHOTO_TAG("photos");
const QString MEDIA_TAG("media");
const QString BATCH_TAG("batch");

GTransport::GTransport(QObject *parent) :
    QObject(parent)
{
}
