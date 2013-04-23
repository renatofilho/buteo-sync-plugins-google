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

#ifndef GCONTACT_H
#define GCONTACT_H

#include <QObject>
#include <QContactManager>

QTM_USE_NAMESPACE

class GContactEntry : public QObject
{
    Q_OBJECT
public:

    explicit GContactEntry(bool generateXmlFlag, QObject *parent = 0);

    void setId(QString id);

    typedef enum
    {
        fullName,
        givenName,
        additionalName,
        familyName,
        namePrefix,
        nameSuffix
    } NAME;

    void setFullName(QString fullName);
    QString getFullName();

    void setAdditionalName(QString additionalName);
    QString getAdditionalName();

    void setFamilyName(QString familyName);
    QString getFamilyName();

    void setNamePrefix(QString namePrefix);
    QString getNamePrefix();

    void setNameSuffix(QString nameSuffix);
    QString getNameSuffix();

    void setGivenName(QString fullName);
    QString getGivenName();

    typedef enum
    {
        name,
        email,
        im,
        phoneNumber,
        structuredPostalAddress,
        deleted,
        groupMembershipInfo,
        where
    } CONTACT_NAMESPACE;

    typedef enum
    {
        address,
        label,
        rel,
        primary,
        protocol,
        uri,
        orgName,
        orgTitle,
        orgDepartment,
        orgSymbol,
        orgJobDescription
    } PROPERTIES;

    void setEmail(QString address, QString label, QString rel, bool primary);

    typedef enum
    {
        AIM,
        MSN,
        YAHOO,
        SKYPE,
        QQ,
        GOOGLE_TALK,
        ICQ,
        JABBER
    } IM_PROTOCOL;

    typedef enum
    {
        assistant,
        callback,
        car,
        company_main,
        isdn,
        main,
        fax,
        home,
        home_fax,
        mobile,
        other,
        other_fax,
        radio,
        telex,
        tty_tdd,
        pager,
        work,
        work_fax,
        work_mobile,
        work_pager
    } PHONENUMBER_TYPES;

private:

    QString mId;

    QString mNote;

    QString mGivenName;
    QString mAdditionalName;
    QString mFamilyName;
    QString mNamePrefix;
    QString mNameSuffix;
    QString mFullName;

    bool mGenerateXmlFlag;

signals:

public slots:

};

#endif // GCONTACT_H
