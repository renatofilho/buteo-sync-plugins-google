curl -d "client_id=340286938476.apps.googleusercontent.com&scope=https://www.google.com/m8/feeds/ https://www.googleapis.com/auth/userinfo.profile" https://accounts.google.com/o/oauth2/device/code

# output
#{
#  "device_code" : "4/OPkob1JigTUheZ72hm4fEmcHXy3E", => this is used for oauth_get_tokens.sh
#  "user_code" : "wkwnjjvj",
#  "verification_url" : "http://www.google.com/device",
#  "expires_in" : 1800,
#  "interval" : 5
#}
