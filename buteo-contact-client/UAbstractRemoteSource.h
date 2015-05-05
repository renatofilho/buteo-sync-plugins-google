#ifndef UABSTRACTREMOTESOURCE_H
#define UABSTRACTREMOTESOURCE_H

#include <QObject>
#include <QDateTime>
#include <QVariantMap>

#include <SyncCommonDefs.h>

#include "UContactInfo.h"

class UAbstractRemoteSource : public QObject
{
    Q_OBJECT
public:
    UAbstractRemoteSource(QObject *parent = 0);
    ~UAbstractRemoteSource();

    virtual void abort() = 0;
    virtual bool init(const QVariantMap &properties) = 0;
    virtual void fetchContacts(const QDateTime &since, bool includeDeleted) = 0;
    virtual void saveContacts(const QList<UContactInfo> contacts) = 0;

signals:
    void contactsFetched(const QList<UContactInfo> contacts, Sync::SyncStatus status);
    void contactsSaved(const QList<UContactInfo> contacts, Sync::SyncStatus status);

};

#endif
