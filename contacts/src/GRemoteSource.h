#include <UAbstractRemoteSource.h>
#include <UContactInfo.h>

#include <QHash>
#include <QScopedPointer>

class GTransport;
class GContactEntry;

class GRemoteSource : public UAbstractRemoteSource
{
    Q_OBJECT
public:
    GRemoteSource(QObject *parent = 0);
    ~GRemoteSource();

    // UAbstractRemoteSource
    bool init(const QVariantMap &properties);
    void abort();
    void fetchContacts(const QDateTime &since, bool includeDeleted);
    void saveContacts(const QList<UContactInfo> contacts);

private slots:
    void networkRequestFinished();
    void networkError(int errorCode);

private:
    enum SyncState {
        STATE_IDLE,
        STATE_FETCHING_CONTACTS,
        STATE_UPLOADING_CONTACTS
    };

    QScopedPointer<GTransport> mTransport;
    QString mRemoteUri;
    QString mAuthToken;
    QString mSyncTarget;
    SyncState mState;
    int mStartIndex;
    QList<UContactInfo> mContactsToUpload;

    void fetchRemoteContacts(const QDateTime &since, bool includeDeleted, int startIndex);
    QList<UContactInfo> toUContacts(const QList<GContactEntry*> gContactList) const;
    void saveContactsContinue();
};
