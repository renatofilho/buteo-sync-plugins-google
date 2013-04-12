#ifndef GCONTACT_H
#define GCONTACT_H

#include <QObject>
#include <QContact>
#include <QContactManager>

QTM_USE_NAMESPACE;

class GContact : public QObject
{
    Q_OBJECT
public:
    explicit GContact(QObject *parent = 0);

    const QContact decode( const QString json );

    const QString encode( const QContact &contact );

private:

    QContactManager		*iMgr;
    
signals:
    
public slots:
    
};

#endif // GCONTACT_H
