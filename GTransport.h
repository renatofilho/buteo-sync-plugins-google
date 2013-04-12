#ifndef GTRANSPORT_H
#define GTRANSPORT_H

#include <QObject>
#include <QList>
#include <QPair>

class GTransport : public QObject
{
    Q_OBJECT
public:
    explicit GTransport(QObject *parent = 0);

    void init();

    void setHeaders(QList<QPair<QByteArray, QByteArray> > headers);

    const QString POST( const QString message, const QList<QPair<QByteArray, QByteArray> > headers );

    const QString GET( const QString message, const QList<QPair<QByteArray, QByteArray> > headers );

    // Include "X-HTTP-Method-Override: DELETE" in the delete POST method to avoid blocking of HTTP DELETE message by firewalls
    const void DELETE( const QString contactId );

    enum RESPONSE_CODES {
        HTTP_OK = 200,
        HTTP_CONTACT_CREATED = 201,
        HTTP_PRECONDITION_FAILED = 412
    };

private:

    QList<QPair<QByteArray, QByteArray> > iHeaders;

signals:
    
public slots:
    
};

#endif // GTRANSPORT_H
