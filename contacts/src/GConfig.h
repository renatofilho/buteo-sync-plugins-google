#ifndef GCONFIG_H
#define GCONFIG_H

class GConfig
{
public:

    static const int MAX_RESULTS = 10;
    static const QString SCOPE_URL;
    static const QString GCONTACT_URL;

    static const QString GDATA_VERSION_TAG;
    static const QString GDATA_VERSION;
    static const QString G_DELETE_OVERRIDE_HEADER;
    static const QString G_ETAG_HEADER;
    static const QString G_AUTH_HEADER;

    /* Query parameters */
    static const QString QUERY_TAG;
    static const QString MAX_RESULTS_TAG;
    static const QString START_INDEX_TAG;
    static const QString UPDATED_MIN_TAG;
    static const QString ORDERBY_TAG;
    static const QString SHOW_DELETED_TAG;
    static const QString REQUIRE_ALL_DELETED;
    static const QString SORTORDER_TAG;

    static const QString PHOTO_TAG;
    static const QString MEDIA_TAG;
    static const QString BATCH_TAG;

    typedef enum
    {
        NONE = 0,
        ADD,
        UPDATE,
        DELETE
    } TRANSACTION_TYPE;
};

static const QString SCOPE_URL = "https://www.google.com/m8/feeds/";
static const QString GCONTACT_URL = SCOPE_URL + "/contacts/default/";

static const QString GDATA_VERSION_TAG = "GData-Version";
static const QString GDATA_VERSION = "3.0";
static const QString G_DELETE_OVERRIDE_HEADER = "X-HTTP-Method-Override: DELETE";
static const QString G_ETAG_HEADER = "If-Match";
static const QString G_AUTH_HEADER = "Authorization";

/* Query parameters */
static const QString QUERY_TAG = "q";
static const QString MAX_RESULTS_TAG = "max-results";
static const QString START_INDEX_TAG = "start-index";
static const QString UPDATED_MIN_TAG = "updated-min";
static const QString ORDERBY_TAG = "orderby";
static const QString SHOW_DELETED_TAG = "showdeleted";
static const QString REQUIRE_ALL_DELETED = "requirealldeleted";
static const QString SORTORDER_TAG = "sortorder";

static const QString PHOTO_TAG = "photos";
static const QString MEDIA_TAG = "media";
static const QString BATCH_TAG = "batch";

#endif // GCONFIG_H

