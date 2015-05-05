#ifndef GCONFIG_H
#define GCONFIG_H

#include <QString>

class GConfig
{
public:
    static const int MAX_RESULTS;
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
#endif // GCONFIG_H

