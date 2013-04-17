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

GTransport::GTransport(const QString url, QByteArray data, QList<QPair<QByteArray, QByteArray> > *headers) :
        iUrl(url), iHeaders(headers), iNetworkMgr(this), iNetworkRequest(NULL), iNetworkReply(NULL)
{
    QBuffer *buffer = new QBuffer(this);
    buffer->setData(data);
    iPostData = buffer;

    if (headers != NULL)
    {
        setHeaders();
    }

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

void GTransport::setHeaders()
{
    for (int i=0; i < iHeaders->size(); i++)
    {
        QPair<QByteArray, QByteArray> headerPair = iHeaders->at(i);
        iNetworkRequest->setRawHeader(headerPair.first, headerPair.second);
    }
}

void GTransport::encode(QUrl& url)
{
    QList<QPair<QString, QString> > queryList = url.queryItems();
    for (int i=0; i < queryList.size(); i++)
    {
        QPair<QString, QString> pair = queryList.at(i);
        QByteArray leftEncode = QUrl::toPercentEncoding(pair.first);
        QByteArray rightEncode = QUrl::toPercentEncoding(pair.second);
        url.removeQueryItem(pair.first);
        url.addEncodedQueryItem(leftEncode, rightEncode);
    }
}

void GTransport::request(const HTTP_REQUEST_TYPE type)
{
    iNetworkReplyBody = "";
    iNetworkRequest = new QNetworkRequest;
    iNetworkRequest->setUrl(iUrl);
    setHeaders();

    switch (type)
    {
        case GET:
            iNetworkReply = iNetworkMgr.get(*iNetworkRequest);
        break;
        case POST:
            iNetworkReply = iNetworkMgr.post(*iNetworkRequest, iPostData);
        break;
        case PUT:
            iNetworkReply = iNetworkMgr.put(*iNetworkRequest, iPostData);
        break;
        case DELETE:
            iNetworkReply = iNetworkMgr.deleteResource(*iNetworkRequest);
        break;
        case HEAD:
            // Nothing
        break;
        default:
            // FIXME: Handle the error
        break;
    }

    QObject::connect(iNetworkReply, SIGNAL(readyRead()), this,
                     SLOT(readyRead()));

    iNetworkError = iNetworkReply->error();
}

const QNetworkReply *GTransport::reply() const
{
    return iNetworkReply;
}

const QString GTransport::replyBody() const
{
    return iNetworkReplyBody;
}

void GTransport::readyRead()
{
    QByteArray bytes = iNetworkReply->readAll();
    iNetworkReplyBody = iNetworkReplyBody + bytes;
}

void GTransport::finishedSlot(QNetworkReply *reply)
{
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    QVariant redirectionUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    iNetworkError = reply->error();

    if (iNetworkError != QNetworkReply::NoError)
    {
        // report error
    }

    emit finishedRequest();

    delete iPostData;
    iPostData = NULL;
}
