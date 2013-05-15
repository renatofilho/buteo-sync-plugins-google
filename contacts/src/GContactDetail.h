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

#ifndef GCONTACTDETAIL_H
#define GCONTACTDETAIL_H

#include <QContactDetail>

QTM_BEGIN_NAMESPACE

class Q_CONTACTS_EXPORT GContactDetail : public QContactDetail
{
public:
#ifdef Q_DOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldDisplay;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL (GContactDetail, "GoogleContacts")
    Q_DECLARE_LATIN1_CONSTANT (FieldDisplay, "display");
#endif

    void setDisplay (const QString& display) { setValue (FieldDisplay, display); }
    QString display () const {return value (FieldDisplay); }
};

QTM_END_NAMESPACE

#endif // GCONTACTDETAIL_H
