/*
 * This file is part of buteo-gcontact-plugin package
 *
 * Copyright (C) 2013 Jolla Ltd. and/or its subsidiary(-ies).
 *
 * Contributors: Sateesh Kavuri <sateesh.kavuri@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "GContactClient.h"
#include <buteosyncfw/LogMacros.h>

GContactClient::GContactClient(const QString& aPluginName,
        const Buteo::SyncProfile& aProfile,
        Buteo::PluginCbInterface *aCbInterface) :
    ClientPlugin(aPluginName, aProfile, aCbInterface) {
    FUNCTION_CALL_TRACE;
}

GContactClient::~GContactClient ()
{
}

bool
GContactClient::init()
{
    // TODO: Perform some initialization tasks
    return false;
}

bool
GContactClient::uninit()
{
    return false;
}

bool
GContactClient::startSync()
{
    /**
     1. If no previous sync, go for slow-sync. Fetch all contacts
        from server
     2. Check if previous sync happened (from SyncLog). If yes,
        fetch the time of last sync
     3. Using the last sync time, retrieve all contacts from server
        that were added/modified/deleted
     4. Fetch all added/modified/deleted items from device
     5. Check for conflicts. Take the default policy as "server-wins"
     6. Save the list from the server to device
     7. Push "client changes" - "conflicting items" to the server
     8. Save the sync log
     */
    return false;
}

void
GContactClient::abortSync(Sync::SyncStatus aStatus)
{
}

Buteo::SyncResults
GContactClient::getSyncResults() const
{
}

bool
GContactClient::cleanUp()
{
    return false;
}

void
GContactClient::connectivityStateChanged( Sync::ConnectivityType aType,
                                           bool aState )
{
}
