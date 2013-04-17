#ifndef GAUTH_H
#define GAUTH_H

#include <QObject>
#include "GTransport.h"

class GAuth : public QObject
{
    Q_OBJECT
public:
    explicit GAuth(QObject *parent = 0);

    void authenticate();

    const QByteArray token();

private:
    void getToken();

    void processTokenResponse(const QString tokenJSON);

    void deviceAuth();

    void processDeviceCode(const QString deviceCodeJSON);

    GTransport* iTransport;

    QString	iDeviceCode;

    QString iUserCode;

    QString iVerificationURL;

    QByteArray iToken;

private slots:

    void deviceCodeResponse();

    void tokenResponse();
    
public slots:
    
};

#endif // GAUTH_H
