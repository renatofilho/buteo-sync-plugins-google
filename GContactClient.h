#ifndef GCONTACTCLIENT_H
#define GCONTACTCLIENT_H

#include "buteo-gcontact-plugin_global.h"
#include <ClientPlugin.h>
#include "GTransport.h"

class BUTEOGCONTACTPLUGINSHARED_EXPORT GContactClient : Buteo::ClientPlugin
{
public:
    Q_OBJECT;
public:

    /*! \brief Constructor
     *
     * @param aPluginName Name of this client plugin
     * @param aProfile Sync profile
     * @param aCbInterface Pointer to the callback interface
     */
    GContactClient( const QString& aPluginName,
                  const Buteo::SyncProfile& aProfile,
                  Buteo::PluginCbInterface *aCbInterface );

    /*! \brief Destructor
     *
     * Call uninit before destroying the object.
     */
    virtual ~GContactClient();

    //! @see SyncPluginBase::init
    virtual bool init();

    //! @see SyncPluginBase::uninit
    virtual bool uninit();

    //! @see ClientPlugin::startSync
    virtual bool startSync();

    //! @see SyncPluginBase::abortSync
    virtual void abortSync(Sync::SyncStatus aStatus = Sync::SYNC_ABORTED);

    //! @see SyncPluginBase::getSyncResults
    virtual Buteo::SyncResults getSyncResults() const;

    //! @see SyncPluginBase::cleanUp
    virtual bool cleanUp();

public slots:

    //! @see SyncPluginBase::connectivityStateChanged
    virtual void connectivityStateChanged( Sync::ConnectivityType aType,
                                           bool aState );

protected slots:


private:

    bool initAgent();

    void closeAgent();

    bool initTransport();

    void closeTransport();

    bool initConfig();

    void closeConfig();

    /**
     * \brief Subroutine for http transport initiation
     * @return True is success, false if not
     */
    bool initHttpTransport();

    void generateResults( bool aSuccessful );

#ifndef QT_NO_DEBUG
    // helper function for debugging
    // does nothing in release mode
    QString toText( const DataSync::SyncState& aState );
#endif //#ifndef QT_NO_DEBUG

    GTransport*                 iTransport;

    Buteo::SyncResults          iResults;

    quint32                     iCommittedItems;

};

/*! \brief Creates SyncML client plugin
 *
 * @param aPluginName Name of this client plugin
 * @param aProfile Profile to use
 * @param aCbInterface Pointer to the callback interface
 * @return Client plugin on success, otherwise NULL
 */
extern "C" GContactClient* createPlugin( const QString& aPluginName,
                                       const Buteo::SyncProfile& aProfile,
                                       Buteo::PluginCbInterface *aCbInterface );

/*! \brief Destroys SyncML client plugin
 *
 * @param aServer SyncML client plugin instance to destroy
 */
extern "C" void destroyPlugin( GContactClient *aClient );


#endif // GCONTACTCLIENT_H
