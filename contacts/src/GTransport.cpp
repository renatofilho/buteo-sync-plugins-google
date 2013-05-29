/*
 * This file is part of buteo-gcontact-plugin package
 *
 * Copyright (C) 2013 Jolla Ltd. and/or its subsidiary(-ies).
 *
 * Contributors: Sateesh Kavuri <sateesh.kavuri@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "GTransport.h"
#include "GConfig.h"

#include <QBuffer>
#include <QDebug>
#include <QNetworkProxy>
#include <QDateTime>
#include <LogMacros.h>

/*
const int MAX_RESULTS = 10;
const QString SCOPE_URL("https://www.google.com/m8/feeds/");
const QString GCONTACT_URL(SCOPE_URL + "/contacts/default/");

const QString GDATA_VERSION_TAG = QString("GData-Version");
const QString GDATA_VERSION = QString("3.0");
const QString G_DELETE_OVERRIDE_HEADER("X-HTTP-Method-Override: DELETE");
const QString G_ETAG_HEADER("If-Match ");
const QString G_AUTH_HEADER ("Authorization");
*/

/* Query parameters */
/*
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
*/

GTransport::GTransport(QObject *parent) :
    QObject(parent), iNetworkMgr (this), iNetworkRequest (NULL)
{
    FUNCTION_CALL_TRACE;

    addHeader (GDATA_VERSION_TAG.toAscii (), GDATA_VERSION.toAscii ());
    QObject::connect(&iNetworkMgr, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(finishedSlot(QNetworkReply*)));
}

GTransport::~GTransport()
{
    FUNCTION_CALL_TRACE;

    if (iNetworkRequest != NULL)
    {
        delete iNetworkRequest;
        iNetworkRequest = NULL;
    }

    if (iNetworkReply != NULL)
    {
        //iNetworkReply->deleteLater();
        iNetworkReply = NULL;
    }

    /*
    if (iPostData != NULL)
    {
        delete iPostData;
        iPostData = NULL;
    }
    */
}

void
GTransport::setUrl (const QString url)
{
    FUNCTION_CALL_TRACE;

    iUrl.setUrl(url, QUrl::StrictMode);
    encode(iUrl);

    //iNetworkRequest->setUrl (iUrl);
}

void
GTransport::setData (QByteArray data)
{
    FUNCTION_CALL_TRACE;

    QBuffer *buffer = new QBuffer (this);
    buffer->setData (data);
    iPostData = buffer;
}

void GTransport::setHeaders()
{
    FUNCTION_CALL_TRACE;

    for (int i=0; i < iHeaders.size(); i++)
    {
        QPair<QByteArray, QByteArray> headerPair = iHeaders.at(i);
        iNetworkRequest->setRawHeader(headerPair.first, headerPair.second);
    }
}

void
GTransport::addHeader (const QByteArray first, const QByteArray second)
{
    FUNCTION_CALL_TRACE;

    iHeaders.append (QPair<QByteArray, QByteArray> (first, second));
}

void
GTransport::setAuthToken (const QString token)
{
    FUNCTION_CALL_TRACE;

    mAuthToken = token;

    QByteArray header1 = QString(G_AUTH_HEADER).toAscii ();
    addHeader (header1, ("Bearer " + token).toAscii ());
}

void
GTransport::setProxy (QString proxyHost, QString proxyPort)
{
    FUNCTION_CALL_TRACE;

    QNetworkProxy proxy = iNetworkMgr.proxy ();
    proxy.setType (QNetworkProxy::HttpProxy);
    proxy.setHostName (proxyHost);
    proxy.setPort (proxyPort.toInt ());

    iNetworkMgr.setProxy (proxy);
}

void GTransport::encode(QUrl& url)
{
    FUNCTION_CALL_TRACE;

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

void
GTransport::request(const HTTP_REQUEST_TYPE type)
{
    FUNCTION_CALL_TRACE;

    LOG_DEBUG ("Request type:" << type);
    if (iNetworkRequest)
    {
        delete iNetworkRequest;
        iNetworkRequest = NULL;
    }

    iNetworkRequest = new QNetworkRequest ();
    iNetworkRequest->setUrl (iUrl);
    mRequestType = type;

    iNetworkReplyBody = "";

    for (int i=0; i<iHeaders.size (); i++)
    {
        LOG_DEBUG ("Header " << i << ":" << iHeaders.at (i).first
                                  << ":" << iHeaders.at (i).second);
        iNetworkRequest->setRawHeader (iHeaders.at (i).first,
                                       iHeaders.at (i).second);
    }
    switch (type)
    {
        case GET:
            setMaxResults (GConfig::MAX_RESULTS);
            setShowDeleted ();
            iNetworkRequest->setUrl (iUrl);
            LOG_DEBUG("++URL:" << iNetworkRequest->url ().toString ());
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
            // FIXME: signal the error
        break;
    }

    QObject::connect(iNetworkReply, SIGNAL(readyRead ()), this,
                     SLOT(readyRead()));

    iNetworkError = iNetworkReply->error();
}

const
QNetworkReply *GTransport::reply() const
{
    FUNCTION_CALL_TRACE;

    return iNetworkReply;
}

const
QByteArray GTransport::replyBody() const
{
    FUNCTION_CALL_TRACE;

    return iNetworkReplyBody;
}

void
GTransport::readyRead()
{
    FUNCTION_CALL_TRACE;

    mResponseCode = iNetworkReply->attribute (
                QNetworkRequest::HttpStatusCodeAttribute).toInt ();
    LOG_DEBUG ("++RESPONSE CODE:" << mResponseCode);
    if (mResponseCode >= 200 && mResponseCode <= 300)
    {
        QByteArray bytes = iNetworkReply->readAll ();
        iNetworkReplyBody = iNetworkReplyBody + bytes;
    }
}

void
GTransport::finishedSlot(QNetworkReply *reply)
{
    FUNCTION_CALL_TRACE;

    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    QVariant redirectionUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    iNetworkError = reply->error();

    if (iNetworkError != QNetworkReply::NoError)
    {
        emit error (iNetworkError);
    }

    reply->deleteLater ();

    emit finishedRequest();

    /*
    if (iPostData != NULL)
    {
        delete iPostData;
        iPostData = NULL;
    }
    */
}

void
GTransport::handleNetworkError (QNetworkReply::NetworkError networkError)
{
    FUNCTION_CALL_TRACE;

    emit error (networkError);
}

void
GTransport::setUpdatedMin (const QDateTime datetime)
{
    FUNCTION_CALL_TRACE;

    mUpdatedMin = datetime;

    if (!iUrl.hasQueryItem (UPDATED_MIN_TAG))
        iUrl.addQueryItem (UPDATED_MIN_TAG, mUpdatedMin.toString ());
}

void
GTransport::setMaxResults (unsigned int limit)
{
    FUNCTION_CALL_TRACE;

    if (!iUrl.hasQueryItem (MAX_RESULTS_TAG))
        iUrl.addQueryItem (MAX_RESULTS_TAG, QString::number (limit));
    LOG_DEBUG ("Final url" << iUrl.toString ());
}

void
GTransport::setShowDeleted ()
{
    FUNCTION_CALL_TRACE;

    if (!iUrl.hasQueryItem (SHOW_DELETED_TAG))
        iUrl.addQueryItem (SHOW_DELETED_TAG, "true");
}

void
GTransport::setStartIndex (const int index)
{
    FUNCTION_CALL_TRACE;

    if (iUrl.hasQueryItem ("start-index"))
        iUrl.removeQueryItem ("start-index");

    iUrl.addQueryItem ("start-index", QString::number (index));
}

GTransport::HTTP_REQUEST_TYPE
GTransport::requestType ()
{
    return mRequestType;
}
