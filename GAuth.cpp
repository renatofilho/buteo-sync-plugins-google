#include "GAuth.h"

const QString AUTH_URL("https://accounts.google.com/o/oauth2/auth");
const QString CLIENT_SECRET("cE6huV6DyPQCKXo5AOg5Balm");
const QString TOKEN_URI("https://accounts.google.com/o/oauth2/token");
const QString CLIENT_ID("340286938476.apps.googleusercontent.com");

GAuth::GAuth(QObject *parent) :
    QObject(parent)
{
}
