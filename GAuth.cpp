#include "GAuth.h"

/* Values obtained after registering with Google */
const QString CLIENT_ID_TAG			("client_id");
const QString SCOPE_TAG				("scope");
const QString AUTH_URL				("https://accounts.google.com/o/oauth2/auth");
const QString CLIENT_SECRET			("cE6huV6DyPQCKXo5AOg5Balm");
const QString TOKEN_URI				("https://accounts.google.com/o/oauth2/token");
const QString CONTACTS_SCOPE_URL	("https://www.google.com/m8/feeds/");
const QString CLIENT_ID				("340286938476.apps.googleusercontent.com");
const QString OAUTH_DEVICE_CODE_URL ("https://accounts.google.com/o/oauth2/device/code");
const QString USER_INFO_PROFILE		("https://www.googleapis.com/auth/userinfo.profile");
const QString AND					("&");

GAuth::GAuth(QObject *parent) :
    QObject(parent)
{
}

bool GAuth::init()
{
}

bool GAuth::auth()
{
}

const QByteArray GAuth::token()
{
}

const QByteArray GAuth::deviceCode()
{
    const QString data = CLIENT_ID_TAG + CLIENT_ID + AND + SCOPE_TAG + CONTACTS_SCOPE_URL + " " + USER_INFO_PROFILE;
    // FIXME: Create a proper GTransport object
    GTransport *transport = new GTransport();
    QString response = transport->GET();

    /*
     Parse the json output and store the values
     {
       "device_code" : "4/OPkob1JigTUheZ72hm4fEmcHXy3E",
       "user_code" : "wkwnjjvj",
       "verification_url" : "http://www.google.com/device",
       "expires_in" : 1800,
       "interval" : 5
     }
    */

}
