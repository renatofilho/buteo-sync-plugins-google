/*
 * This file is part of buteo-gcontact-plugin package
 *
 * Copyright (C) 2013 Jolla Ltd. and/or its subsidiary(-ies).
 *
 * Contributors: Sateesh Kavuri <sateesh.kavuri@gmail.com>
 *               Mani Chandrasekar <maninc@gmail.com>
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

#include "UContactsCustomDetail.h"

// Ubuntu fields
const QString UContactsCustomDetail::FieldContactETag = "X-ETAG";
const QString UContactsCustomDetail::FieldGroupMembershipInfo = "X-GROUP-ID";
const QString UContactsCustomDetail::FieldRemoteId = "X-REMOTE-ID";


QContactExtendedDetail
UContactsCustomDetail::getCustomField(const QContact &contact, const QString &name)
{
    foreach (QContactExtendedDetail xd, contact.details<QContactExtendedDetail>()) {
        if (xd.name() == name) {
            return xd;
        }
    }
    QContactExtendedDetail xd;
    xd.setName(name);
    return xd;
}
