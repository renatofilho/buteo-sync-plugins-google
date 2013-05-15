device_code=$1
curl -d "client_id=340286938476.apps.googleusercontent.com&client_secret=cE6huV6DyPQCKXo5AOg5Balm&code=$1&grant_type=http://oauth.net/grant_type/device/1.0" https://accounts.google.com/o/oauth2/token > auth_result.txt

grep access_token auth_result.txt | cut -f4 -d'"' > access_token.txt

cp access_token.txt /tmp

#output
#{
#  "access_token" : "ya29.AHES6ZRwtsotfOhe3Vu21XM6kCi7R4r-P1Fq19uvtVIaQ50", => this is used for auth header
#  "token_type" : "Bearer",
#  "expires_in" : 3600,
#  "id_token" : "eyJhbGciOiJSUzI1NiIsImtpZCI6ImQ3NzljOTIzZGJkNGNmN2Y4NTc3Y2EyMGIwODFkODEwOTQ1YzMxZDEifQ.eyJpc3MiOiJhY2NvdW50cy5nb29nbGUuY29tIiwiYXVkIjoiMzQwMjg2OTM4NDc2LmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwidG9rZW5faGFzaCI6IlJJcC03b21rZnRCUFZVU0tTbnAyTVEiLCJhdF9oYXNoIjoiUklwLTdvbWtmdEJQVlVTS1NucDJNUSIsImNpZCI6IjM0MDI4NjkzODQ3Ni5hcHBzLmdvb2dsZXVzZXJjb250ZW50LmNvbSIsImF6cCI6IjM0MDI4NjkzODQ3Ni5hcHBzLmdvb2dsZXVzZXJjb250ZW50LmNvbSIsImlkIjoiMTEwMzQ0MzcxNzQyMDg3OTk4MjIwIiwic3ViIjoiMTEwMzQ0MzcxNzQyMDg3OTk4MjIwIiwiaWF0IjoxMzY1NzAwODIzLCJleHAiOjEzNjU3MDQ3MjN9.Z3Bk4W-9q1aHOxQLKxdAjWf9jTYLAcD9uGEmW73FAjdWKNLufw0LCji3SPtSyJg3uMQTueievTp3xjPcVjfKzfokEq3fjUFn5bets7MLZ7Vr8Le6Yb0mhGcO0kefImfcufuVEXt9I-W-1OddCzyj4YfrYxtJ-eYRwXGOOA6dzZM",
#  "refresh_token" : "1/I-IcRJHdgy16BZZxSyPYF31OUGIfjLSLSBksU3eJT3Q"
#}
