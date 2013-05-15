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

#ifndef GTRANSPORT_H
#define GTRANSPORT_H

#include <QObject>
#include <QList>
#include <QDateTime>
#include <QPair>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class GTransport : public QObject
{
    Q_OBJECT
public:

    typedef enum {
        GET,
        POST,
        DELETE,
        PUT,
        HEAD
    } HTTP_REQUEST_TYPE;

    explicit GTransport(QObject *parent = 0);

    /*! \brief Constructor to be used along with setUrl()
      Same object can be used any number of time for
      network request
     */
    GTransport(QList<QPair<QByteArray, QByteArray> > *headers);

    GTransport(QString url, QList<QPair<QByteArray, QByteArray> > *headers = 0);

    GTransport(const QString url, QByteArray data, QList<QPair<QByteArray, QByteArray> > *headers = 0);

    virtual ~GTransport();

    void setUrl (const QString url);

    void setData (QByteArray data);

    void setHeaders ();

    void addHeader (const QPair<QByteArray, QByteArray> header);

    void setAuthToken (const QString token);

    void setProxy (QString proxyHost, QString proxyPort);

    void request(const HTTP_REQUEST_TYPE type);

    const QNetworkReply* reply() const;

    const QByteArray replyBody() const;

    void setUpdatedMin (const QDateTime datetime);

    // Include "X-HTTP-Method-Override: DELETE" in the delete POST method to avoid blocking of HTTP DELETE message by firewalls
    //const void DELETE( const QString contactId );

    typedef enum
    {
        HTTP_OK = 200,
        HTTP_CONTACT_CREATED = 201,
        HTTP_PRECONDITION_FAILED = 412
    } RESPONSE_CODE;

    static const QString GDATA_VERSION_HEADER;

private:

    void encode(QUrl& url);

    QUrl 									iUrl;

    QList<QPair<QByteArray, QByteArray> > 	*iHeaders;

    QByteArray 								*iAuthToken;

    QNetworkAccessManager					iNetworkMgr;

    QIODevice								*iPostData;

    QNetworkRequest							*iNetworkRequest;

    QNetworkReply							*iNetworkReply;

    QByteArray								iNetworkReplyBody;

    QNetworkReply::NetworkError				iNetworkError;

    int                                     mResponseCode;

    QString                                 mAuthToken;

    QDateTime                               mUpdatedMin;

signals:

    void finishedRequest ();

    void error (QNetworkReply::NetworkError networkError);

private slots:

    virtual void finishedSlot( QNetworkReply* reply );

    virtual void readyRead();

    void handleNetworkError (QNetworkReply::NetworkError networkError);
};

#endif // GTRANSPORT_H
