/*
 * This file is part of buteo-sync-plugins package
 *
 * Copyright (C) 2015 Canoncal LTDA
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

#ifndef UCONTACTINFO_H
#define UCONTACTINFO_H

#include <QtContacts/QContact>

class UContactInfo
{
public:
    UContactInfo(const QtContacts::QContact &contact,
                 const QString &localId,
                 const QString &remoteId,
                 bool deleted);

    QtContacts::QContact contact() const;
    QString remoteId() const;
    QString localId() const;
    bool deleted() const;

private:
    QtContacts::QContact mContact;
    QString mRemoteId;
    QString mLocalId;
    bool mDeleted;
};

inline bool operator==(const UContactInfo &c1, const UContactInfo &c2)
{
    return c1.contact() == c2.contact() &&
           c1.remoteId() == c2.remoteId() &&
           c1.localId() == c2.localId() &&
           c1.deleted() == c2.deleted();
}

inline uint qHash(const UContactInfo &c, uint seed)
{
    return qHash(c.contact(), seed) ^
           qHash(c.remoteId(), seed) ^
           qHash(c.localId(), seed) ^ c.deleted();
}


#endif
