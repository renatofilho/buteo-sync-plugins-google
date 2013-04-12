#ifndef GAUTH_H
#define GAUTH_H

#include <QObject>

class GAuth : public QObject
{
    Q_OBJECT
public:
    explicit GAuth(QObject *parent = 0);

    const QString token();

private:
    void init();

    bool auth();

signals:
    
public slots:
    
};

#endif // GAUTH_H
