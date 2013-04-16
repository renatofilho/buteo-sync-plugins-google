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
    explicit GTransport(QObject *parent = 0);

    explicit GTransport(const QString url, QByteArray *data, QList<QPair<QByteArray, QByteArray> > *headers);

    virtual ~GTransport();

    void init();

    void setHeaders(QList<QPair<QByteArray, QByteArray> > headers);

    const QString POST( );

    const QString GET( );

    const QString PUT( );

    // Include "X-HTTP-Method-Override: DELETE" in the delete POST method to avoid blocking of HTTP DELETE message by firewalls
    //const void DELETE( const QString contactId );

    enum RESPONSE_CODES {
        HTTP_OK = 200,
        HTTP_CONTACT_CREATED = 201,
        HTTP_PRECONDITION_FAILED = 412
    };

private:

    void encode(QUrl& url);

    QUrl 									iUrl;

    QList<QPair<QByteArray, QByteArray> > 	*iHeaders;

    QByteArray 								*iAuthToken;

    QNetworkAccessManager					iNetworkMgr;

    QNetworkRequest							*iNetworkRequest;

    QNetworkReply							*iNetworkReply;

    QString									iNetworkReplyBody;

    QIODevice								*iPostData;

signals:
    
public slots:
    
};

#endif // GTRANSPORT_H
