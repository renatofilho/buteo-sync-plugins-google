#include "GTransport.h"

#include <QBuffer>

const int MAX_RESULTS = 10;
const QString SCOPE_URL("https://www.google.com/m8/feeds/");
const QString GCONTACT_URL(SCOPE_URL + "/contacts/default/");
const QString GDATA_VERSION_HEADER("GData-Version: 3.0");
const QString DELETE_OVERRIDE_HEADER("X-HTTP-Method-Override: DELETE");
const QString ETAG_HEADER("If-Match: Etag");

/* Query parameters */
const QString QUERY_TAG("q");
const QString MAX_RESULTS_TAG("max-results");
const QString START_INDEX_TAG("start-index");
const QString UPDATED_MIN_TAG("updated-min");
const QString ORDERBY_TAG("orderby");
const QString SHOW_DELETED_TAG("showdeleted");
const QString REQUIRE_ALL_DELETED("requirealldeleted");
const QString SORTORDER_TAG("sortorder");

const QString PHOTO_TAG("photos");
const QString MEDIA_TAG("media");
const QString BATCH_TAG("batch");

GTransport::GTransport(QObject *parent) :
    QObject(parent)
{
}

GTransport::GTransport(const QString url, QByteArray *data, QList<QPair<QByteArray, QByteArray> > *headers) :
        iNetworkMgr(this), iNetworkRequest(NULL), iNetworkReply(NULL), iUrl(url), iHeaders(headers)
{
    QBuffer *buffer = new QBuffer(this);
    buffer->setBuffer(data);
    iPostData = buffer;

    QObject::connect(&iNetworkMgr, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(finishedSlot(QNetworkReply*)));
    iUrl.setUrl(url, QUrl::StrictMode);
    encode(iUrl);
}

GTransport::~GTransport()
{
    if (iNetworkRequest != NULL)
    {
        delete iNetworkRequest;
        iNetworkRequest = NULL;
    }

    if (iNetworkReply != NULL)
    {
        iNetworkReply->deleteLater();
        iNetworkReply = NULL;
    }

    if (iPostData != NULL)
    {
        delete iPostData;
        iPostData = NULL;
    }
}

void GTransport::setHeaders(QList<QPair<QByteArray, QByteArray> > headers)
{
    for (int i=0; i<headers.size(); i++)
    {
        QPair<QByteArray, QByteArray> headerPair = headers.at(i);
        iNetworkRequest->setRawHeader(headerPair.first, headerPair.second);
    }
}

void GTransport::encode(QUrl& url)
{
    QList<QPair<QString, QString> > queryList = url.queryItems();
    for (int i=0; i<queryList.size(); i++)
    {
        QPair<QString, QString> pair = queryList.at(i);
        QByteArray leftEncode = QUrl::toPercentEncoding(pair.first);
        QByteArray rightEncode = QUrl::toPercentEncoding(pair.second);
        url.removeQueryItem(pair.first);
        url.addEncodedQueryItem(leftEncode, rightEncode);
    }
}
