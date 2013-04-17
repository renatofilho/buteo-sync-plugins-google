#include "GAuth.h"
#include "GTransport.h"

/* Values obtained after registering with Google */
const QString CODE_TAG            	("code");
const QString CLIENT_ID_TAG	        ("client_id");
const QString CLIENT_ID	            ("340286938476.apps.googleusercontent.com");
const QString SCOPE_TAG             ("scope");
const QString CONTACTS_SCOPE_URL	("https://www.google.com/m8/feeds/");
const QString CLIENT_SECRET_TAG		("client_secret");
const QString CLIENT_SECRET			("cE6huV6DyPQCKXo5AOg5Balm");
const QString GRANT_TYPE_TAG		("grant_type");
const QString GRANT_TYPE			("http://oauth.net/grant_type/device/1.0");
const QString AUTH_URL				("https://accounts.google.com/o/oauth2/auth");
const QString TOKEN_URI				("https://accounts.google.com/o/oauth2/token");
const QString OAUTH_DEVICE_CODE_URL ("https://accounts.google.com/o/oauth2/device/code");
const QString USER_INFO_PROFILE		("https://www.googleapis.com/auth/userinfo.profile");
const QString AND					("&");
const QString EQUALS				("=");

GAuth::GAuth(QObject *parent) :
    QObject(parent)
{
}

const QByteArray GAuth::token()
{
    return iToken;
}

void GAuth::authenticate()
{
    deviceAuth();
    getToken();
}

void GAuth::getToken()
{
    if (iDeviceCode == "")
    {
        // FIXME: Error condition
    }
    const QString data = CLIENT_ID_TAG + EQUALS + CLIENT_ID + AND +
                         CLIENT_SECRET_TAG + EQUALS + CLIENT_SECRET + AND +
                         CODE_TAG + EQUALS + iDeviceCode + AND +
                         GRANT_TYPE_TAG + EQUALS + GRANT_TYPE;

    if (iTransport != NULL)
    {
        delete iTransport;
        iTransport = NULL;
    }
    iTransport = new GTransport(TOKEN_URI, data.toAscii(), NULL);
    QObject::connect(iTransport, SIGNAL(finishedRequest()),
                     this, SLOT(tokenResponse()));
    iTransport->request(GTransport::GET);
}

void GAuth::tokenResponse()
{
    if (iTransport != NULL)
    {
        const QNetworkReply* reply = iTransport->reply();
        if (reply->error() == QNetworkReply::NoError)
        {
            QString body = iTransport->replyBody();
            processTokenResponse(body);
        }
    }
}

void GAuth::processTokenResponse(const QString tokenJSON)
{
    /*
    {
      "access_token" : "ya29.AHES6ZRwtsotfOhe3Vu21XM6kCi7R4r-P1Fq19uvtVIaQ50", => this is used for auth header
      "token_type" : "Bearer",
      "expires_in" : 3600,
      "id_token" : "eyJhbGciOiJSUzI1NiIsImtpZCI6ImQ3NzljOTIzZGJkNGNmN2Y4NTc3Y2EyMGIwODFkODEwOTQ1YzMxZDEifQ.eyJpc3MiOiJhY2NvdW50cy5nb29nbGUuY29tIiwiYXVkIjoiMzQwMjg2OTM4NDc2LmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwidG9rZW5faGFzaCI6IlJJcC03b21rZnRCUFZVU0tTbnAyTVEiLCJhdF9oYXNoIjoiUklwLTdvbWtmdEJQVlVTS1NucDJNUSIsImNpZCI6IjM0MDI4NjkzODQ3Ni5hcHBzLmdvb2dsZXVzZXJjb250ZW50LmNvbSIsImF6cCI6IjM0MDI4NjkzODQ3Ni5hcHBzLmdvb2dsZXVzZXJjb250ZW50LmNvbSIsImlkIjoiMTEwMzQ0MzcxNzQyMDg3OTk4MjIwIiwic3ViIjoiMTEwMzQ0MzcxNzQyMDg3OTk4MjIwIiwiaWF0IjoxMzY1NzAwODIzLCJleHAiOjEzNjU3MDQ3MjN9.Z3Bk4W-9q1aHOxQLKxdAjWf9jTYLAcD9uGEmW73FAjdWKNLufw0LCji3SPtSyJg3uMQTueievTp3xjPcVjfKzfokEq3fjUFn5bets7MLZ7Vr8Le6Yb0mhGcO0kefImfcufuVEXt9I-W-1OddCzyj4YfrYxtJ-eYRwXGOOA6dzZM",
      "refresh_token" : "1/I-IcRJHdgy16BZZxSyPYF31OUGIfjLSLSBksU3eJT3Q"
    }
    */

    // FIXME: Retrieve the proper token
    iToken = QString("").toAscii();
}

void GAuth::deviceAuth()
{
    const QString data = CLIENT_ID_TAG + EQUALS + CLIENT_ID + AND +
                         SCOPE_TAG + EQUALS + CONTACTS_SCOPE_URL + " " + USER_INFO_PROFILE;

    // FIXME: Create a proper GTransport object
    iTransport = new GTransport(OAUTH_DEVICE_CODE_URL, data.toAscii(), NULL);

    QObject::connect(iTransport, SIGNAL(finishedRequest()),
                     this, SLOT(deviceCodeResponse()));

    iTransport->request(GTransport::GET);
}

void GAuth::deviceCodeResponse()
{
    if (iTransport != NULL)
    {
        const QNetworkReply* reply = iTransport->reply();
        if (reply->error() == QNetworkReply::NoError)
        {
            QString body = iTransport->replyBody();
            processDeviceCode(body);
        }
    }
}

void GAuth::processDeviceCode(const QString deviceCodeJSON)
{
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

    // FIXME: Do a raw processing or use some JSON library?
    iDeviceCode = "";
}
