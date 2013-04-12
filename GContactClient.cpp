#include "GContactClient.h"
#include <buteosyncfw/LogMacros.h>

GContactClient::GContactClient(const QString& aPluginName,
        const Buteo::SyncProfile& aProfile,
        Buteo::PluginCbInterface *aCbInterface) :
    ClientPlugin(aPluginName, aProfile, aCbInterface) {
    FUNCTION_CALL_TRACE;
}

