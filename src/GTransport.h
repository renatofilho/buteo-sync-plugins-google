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

    GTransport(QString url, QList<QPair<QByteArray, QByteArray> > *headers = 0);

    GTransport(const QString url, QByteArray data, QList<QPair<QByteArray, QByteArray> > *headers = 0);

    virtual ~GTransport();

    void setHeaders();

    void request(const HTTP_REQUEST_TYPE type);

    const QNetworkReply* reply() const;

    const QByteArray replyBody() const;

    // Include "X-HTTP-Method-Override: DELETE" in the delete POST method to avoid blocking of HTTP DELETE message by firewalls
    //const void DELETE( const QString contactId );

    typedef enum
    {
        HTTP_OK = 200,
        HTTP_CONTACT_CREATED = 201,
        HTTP_PRECONDITION_FAILED = 412
    } RESPONSE_CODE;

private:

    void encode(QUrl& url);

    QUrl 									iUrl;

    QList<QPair<QByteArray, QByteArray> > 	*iHeaders;

    QByteArray 								*iAuthToken;

    QNetworkAccessManager					iNetworkMgr;

    QNetworkRequest							*iNetworkRequest;

    QNetworkReply							*iNetworkReply;

    QByteArray								iNetworkReplyBody;

    QIODevice								*iPostData;

    QNetworkReply::NetworkError				iNetworkError;

    int                                     mResponseCode;
signals:

    void finishedRequest();

private slots:

    virtual void finishedSlot( QNetworkReply* reply );

    virtual void readyRead();

};

#endif // GTRANSPORT_H
