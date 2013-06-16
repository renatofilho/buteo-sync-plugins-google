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

#include <QBuffer>
#include <QDebug>
#include <QNetworkProxy>
#include <QDateTime>
#include <LogMacros.h>

const int MAX_RESULTS = 10;
const QString SCOPE_URL("https://www.google.com/m8/feeds/");
const QString GCONTACT_URL(SCOPE_URL + "/contacts/default/");

const QString GDATA_VERSION_TAG = "GData-Version";
const QString GDATA_VERSION = "3.1";
const QString G_DELETE_OVERRIDE_HEADER("X-HTTP-Method-Override: DELETE");
const QString G_ETAG_HEADER("If-Match ");
const QString G_AUTH_HEADER ("Authorization");
const QString G_CONTENT_TYPE_HEADER = "application/atom+xml";

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
    QObject(parent), iNetworkMgr (this), iNetworkRequest (NULL)
{
    FUNCTION_CALL_TRACE;
}

GTransport::GTransport (QUrl url, QList<QPair<QByteArray, QByteArray> > headers) :
    iHeaders (headers), iNetworkMgr (this),
    iNetworkRequest (NULL), iNetworkReply (NULL)
{
    construct (url);
}

GTransport::GTransport (QUrl url, QList<QPair<QByteArray, QByteArray> > headers, QByteArray data) :
    iHeaders (headers), iNetworkMgr (this), iPostData (data),
    iNetworkRequest (NULL), iNetworkReply (NULL)
{
    construct (url);
}

GTransport::~GTransport()
{
    FUNCTION_CALL_TRACE;

    delete iNetworkRequest;
    iNetworkRequest = NULL;

    if (iNetworkReply != NULL)
    {
        iNetworkReply->deleteLater();
        iNetworkReply = NULL;
    }
}

void
GTransport::setUrl (const QString url)
{
    FUNCTION_CALL_TRACE;

    iUrl.setUrl(url, QUrl::StrictMode);
    construct (iUrl);
}

void
GTransport::setData (QByteArray data)
{
    FUNCTION_CALL_TRACE;

    if (!iPostData.isEmpty ())
        iPostData.clear ();

        iPostData = data;
}

void GTransport::setHeaders()
{
    FUNCTION_CALL_TRACE;

    /*
     * A laughable bug in Google (bug#3397). If the "GDataVersion:3.0" tag is not
     * the first header in the list, then google does not consider
     * it as a 3.0 version, and just returns the default format
     * So, the headers order is very sensitive
     */
    for (int i=0; i < iHeaders.count (); i++)
    {
        QPair<QByteArray, QByteArray> headerPair = iHeaders[i];
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
GTransport::setGDataVersionHeader ()
{
    iHeaders.append (QPair<QByteArray, QByteArray> (QByteArray ("GData-Version"), QByteArray ("3.0")));
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

void
GTransport::construct (const QUrl& url)
{
    FUNCTION_CALL_TRACE;

    QObject::connect(&iNetworkMgr, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(finishedSlot(QNetworkReply*)));

    iUrl = url;

    QList<QPair<QString, QString> > queryList = iUrl.queryItems();
    for (int i=0; i < queryList.size(); i++)
    {
        QPair<QString, QString> pair = queryList.at(i);
        QByteArray leftEncode = QUrl::toPercentEncoding(pair.first);
        QByteArray rightEncode = QUrl::toPercentEncoding(pair.second);
        iUrl.removeQueryItem(pair.first);
        iUrl.addEncodedQueryItem(leftEncode, rightEncode);
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

    if (iNetworkRequest == NULL)
    {
        if (iNetworkReply)
        {
            //iNetworkReply->deleteLater ();
            iNetworkReply = NULL;
        }
    }
    iNetworkReplyBody = "";

    iNetworkRequest = new QNetworkRequest ();
    iNetworkRequest->setUrl (iUrl);
    setHeaders ();

    mRequestType = type;
    LOG_DEBUG("++URL:" << iNetworkRequest->url ().toString ());
    switch (type)
    {
        case GET: {
            iNetworkReply = iNetworkMgr.get(*iNetworkRequest);
            LOG_DEBUG ("--- FINISHED GET REQUEST ---");
        }
        break;
        case POST: {
            iNetworkRequest->setHeader (QNetworkRequest::ContentLengthHeader, iPostData.size ());
            iNetworkReply = iNetworkMgr.post(*iNetworkRequest, iPostData);
            LOG_DEBUG ("--- FINISHED POST REQUEST ---");
        }
        break;
        case PUT:
            iNetworkRequest->setHeader (QNetworkRequest::ContentLengthHeader, iPostData.size ());
            iNetworkReply = iNetworkMgr.put(*iNetworkRequest, iPostData);
            LOG_DEBUG ("--- FINISHED PUT REQUEST ---");
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

    QList<QByteArray> headerList = iNetworkRequest->rawHeaderList ();
    for (int i=0; i<headerList.size (); i++)
    {
        LOG_DEBUG ("Header " << i << ":" << headerList.at (i)
                                  << ":" << iNetworkRequest->rawHeader (headerList.at (i)));
    }
    QObject::connect(iNetworkReply, SIGNAL(readyRead ()), this,
                     SLOT(readyRead()));
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
    QByteArray bytes = iNetworkReply->readAll ();
    if (mResponseCode >= 200 && mResponseCode <= 300)
    {
        iNetworkReplyBody += bytes;
    } else
    {
        LOG_DEBUG ("SERVER ERROR:" << bytes);
        emit error (mResponseCode);
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

    emit finishedRequest();
}

void
GTransport::setUpdatedMin (const QDateTime datetime)
{
    FUNCTION_CALL_TRACE;

    mUpdatedMin = datetime;

    if (!iUrl.hasQueryItem (UPDATED_MIN_TAG))
        iUrl.addQueryItem (UPDATED_MIN_TAG,
                           mUpdatedMin.toString (Qt::ISODate));
}

void
GTransport::setMaxResults (unsigned int limit)
{
    FUNCTION_CALL_TRACE;

    if (!iUrl.hasQueryItem (MAX_RESULTS_TAG))
        iUrl.addQueryItem (MAX_RESULTS_TAG, QString::number (limit));
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

void
GTransport::reset ()
{
    iUrl.clear ();
    iHeaders.clear ();
    iPostData.clear ();
}
