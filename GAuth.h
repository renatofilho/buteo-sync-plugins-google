#ifndef GAUTH_H
#define GAUTH_H

#include <QObject>
#include "GTransport.h"

class GAuth : public QObject
{
    Q_OBJECT
public:
    explicit GAuth(QObject *parent = 0);

    const QByteArray token();

private:
    bool init();

    bool auth();

    const QByteArray deviceCode();

    GTransport	*iTransport;
    QString	iDeviceCode;
    QString iUserCode;
    QString iVerificationURL;

signals:
    
public slots:
    
};

#endif // GAUTH_H
