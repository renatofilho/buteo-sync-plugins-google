#include "GConfig.h"

const int GConfig::MAX_RESULTS = 10;
const QString GConfig::SCOPE_URL = "https://www.google.com/m8/feeds/";
const QString GConfig::GCONTACT_URL = SCOPE_URL + "/contacts/default/";

const QString GConfig::GDATA_VERSION_TAG = "GData-Version";
const QString GConfig::GDATA_VERSION = "3.0";
const QString GConfig::G_DELETE_OVERRIDE_HEADER = "X-HTTP-Method-Override: DELETE";
const QString GConfig::G_ETAG_HEADER = "If-Match";
const QString GConfig::G_AUTH_HEADER = "Authorization";

/* Query parameters */
const QString GConfig::QUERY_TAG = "q";
const QString GConfig::MAX_RESULTS_TAG = "max-results";
const QString GConfig::START_INDEX_TAG = "start-index";
const QString GConfig::UPDATED_MIN_TAG = "updated-min";
const QString GConfig::ORDERBY_TAG = "orderby";
const QString GConfig::SHOW_DELETED_TAG = "showdeleted";
const QString GConfig::REQUIRE_ALL_DELETED = "requirealldeleted";
const QString GConfig::SORTORDER_TAG = "sortorder";

const QString GConfig::PHOTO_TAG = "photos";
const QString GConfig::MEDIA_TAG = "media";
const QString GConfig::BATCH_TAG = "batch";
